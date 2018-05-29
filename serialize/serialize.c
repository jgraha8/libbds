/** @file
 *  @brief Stack data structure module
 *
 *  @author Jason Graham <jgraha8@gmail.com>
 */

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memutil.h"
#include <libbds/bds_serialize.h>

static void add_serial_len(const char *object, const struct bds_object_desc *object_desc,
                           size_t *members_len, size_t *alloc_len)
{
        const char *alloc_data;

        if (members_len) {
                *members_len += object_desc->object_len;
        }

        // First get the storage requirements for the members
        for (size_t i = 0; i < object_desc->num_members; ++i) {

                const struct bds_object_member *member = object_desc->members + i;
                const char *object_member              = object + member->offset;

                switch (member->type) {
                case BDS_OBJECT_DATA:
                        break;
                case BDS_OBJECT_PTR_DATA:
                        alloc_data = *(char **)(object_member);
                        if (member->config && alloc_data) {
                                *alloc_len += ((size_t(*)(const void *))member->config)(object);
                        }
                        break;
                case BDS_OBJECT_DATA_OBJECT:
                        if (member->config) {
                                add_serial_len(object_member,
                                               (struct bds_object_desc *)member->config, NULL,
                                               alloc_len);
                        }
                        break;
                case BDS_OBJECT_PTR_OBJECT:
                        alloc_data = *(char **)(object_member);
                        if (member->config && alloc_data) {
                                add_serial_len(alloc_data, (struct bds_object_desc *)member->config,
                                               alloc_len, alloc_len);
                        }
                        break;
                default:
                        fprintf(stderr,
                                "bds::serialize::add_serial_len: incorrect object member type %d\n",
                                member->type);
                        exit(EXIT_FAILURE);
                }
        }
}

static void serialize_object(const char *object, const struct bds_object_desc *object_desc,
                             char **members_seg, char **alloc_seg)
{
        // First get the storage requirements for the members
        for (size_t i = 0; i < object_desc->num_members; ++i) {

                const struct bds_object_member *member = object_desc->members + i;

                const char *object_member = object + member->offset;
                char *serial_member       = *members_seg + member->offset;

                size_t member_len = 0;
                size_t alloc_len  = 0;
                char *__members_seg;

                char *alloc_object;
                char *alloc_data;

                switch (member->type) {
                case BDS_OBJECT_DATA:
                        memcpy(serial_member, object_member, member->len);
                        break;
                case BDS_OBJECT_PTR_DATA:
                        assert(member->len == sizeof(serial_member));
                        // Set pointer to NULL
                        memset(serial_member, 0, member->len);

                        alloc_data = *(char **)(object_member);
                        if (member->config && alloc_data) {
                                memcpy(serial_member, (char *)alloc_seg, member->len);

                                alloc_len = ((size_t(*)(const void *))member->config)(object);
                                memcpy(*alloc_seg, alloc_data, alloc_len);

                                *alloc_seg += alloc_len;
                        }

                        break;
                case BDS_OBJECT_DATA_OBJECT:
                        if (member->config) {
                                __members_seg = serial_member;
                                serialize_object(object_member,
                                                 (struct bds_object_desc *)member->config,
                                                 &__members_seg, alloc_seg);
                        } else {
                                memcpy(serial_member, object_member, member->len);
                        }
                        break;
                case BDS_OBJECT_PTR_OBJECT:
                        assert(member->len == sizeof(*members_seg));
                        memset(serial_member, 0, member->len);

                        alloc_object = *(char **)(object_member);
                        if (member->config && alloc_object) {
                                memcpy(serial_member, (char *)alloc_seg, member->len);

                                member_len = 0;
                                alloc_len  = 0;
                                add_serial_len(alloc_object,
                                               (struct bds_object_desc *)member->config,
                                               &member_len, &alloc_len);

                                __members_seg = *alloc_seg;
                                *alloc_seg += member_len;

                                serialize_object(alloc_object,
                                                 (struct bds_object_desc *)member->config,
                                                 &__members_seg, alloc_seg);
                        }

                        break;
                default:
                        fprintf(stderr,
                                "bds::serialize::add_serial_len: incorrect object member type %d\n",
                                member->type);
                        exit(EXIT_FAILURE);
                }
        }

        *members_seg += object_desc->object_len;
}

static void deserialize_object(const void *serial_object, const struct bds_object_desc *object_desc,
                               void *object)
{
        // First get the storage requirements for the members
        for (size_t i = 0; i < object_desc->num_members; ++i) {

                const struct bds_object_member *member = object_desc->members + i;

                char *object_member       = object + member->offset;
                const char *serial_member = serial_object + member->offset;

                size_t alloc_len;
                char *member_ptr;
                char *serial_ptr;
                struct bds_object_desc *member_desc;

                switch (member->type) {
                case BDS_OBJECT_DATA:
                        memcpy(object_member, serial_member, member->len);
                        break;
                case BDS_OBJECT_PTR_DATA:
                        assert(member->len == sizeof(object_member));

                        member_ptr = *(char **)(object_member);
                        serial_ptr = *(char **)(serial_member);

                        if (member->config && serial_ptr) {
                                alloc_len = ((size_t(*)(const void *))member->config)(object);
                                if (member_ptr == NULL) {
                                        assert( member_ptr = calloc(1, alloc_len) );
                                        memcpy(object_member, &member_ptr, sizeof(member->len));
                                }
                                memcpy(member_ptr, serial_ptr, alloc_len);
                        }
                        break;
                case BDS_OBJECT_DATA_OBJECT:
                        if (member->config) {
                                deserialize_object(serial_ptr,
                                                   (struct bds_object_desc *)member->config,
                                                   member_ptr);
                        } else {
                                // Treating the same as data
                                memcpy(member_ptr, serial_ptr, member->len);
                        }
                        break;
                case BDS_OBJECT_PTR_OBJECT:
                        assert(member->len == sizeof(object_member));

                        member_ptr = *(char **)(object_member);
                        serial_ptr = *(char **)(serial_member);

                        if (member->config && serial_ptr) {
                                member_desc = (struct bds_object_desc *)member->config;
                                alloc_len   = member_desc->object_len;

                                if (member_ptr == NULL) {
                                        assert( member_ptr = calloc(1,alloc_len) );
                                        memcpy(object_member, &member_ptr, sizeof(member->len));
                                }

                                deserialize_object(serial_ptr, member_desc, member_ptr);
                        }

                        break;
                default:
                        fprintf(
                            stderr,
                            "bds::serialize::deserialize_object: incorrect object member type %d\n",
                            member->type);
                        exit(EXIT_FAILURE);
                }
        }
}

void bds_serialize(const void *object, const struct bds_object_desc *object_desc,
                   size_t *serial_len, void **serial_object)
{
        size_t members_len = 0;
        size_t alloc_len   = 0;

        add_serial_len(object, object_desc, &members_len, &alloc_len);

        *serial_len    = members_len + alloc_len;
        *serial_object = xalloc(*serial_len);

        char *members_seg = (char *)(*serial_object);
        char *alloc_seg   = members_seg + members_len;

        serialize_object(object, object_desc, &members_seg, &alloc_seg);

        // Assert ending conditions
        assert(*serial_object + members_len == members_seg);
        assert(*serial_object + *serial_len == alloc_seg);
}

void bds_deserialize(const void *serial_object, const struct bds_object_desc *object_desc,
                     void *object)
{
        deserialize_object(serial_object, object_desc, object);
}
