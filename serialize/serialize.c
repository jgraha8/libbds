#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bds_serialize.h"

struct dumb {
        int i;
        long *stuff;
        int num_stuff;
};

size_t get_stuff_len(struct dumb *dumb) { return dumb->num_stuff * sizeof(*dumb->stuff); }

struct dumb __dumb;
struct bds_object_member dumb_members[] = {BDS_OBJECT_MEMBER(&__dumb, __dumb.i, BDS_OBJECT_DATA, NULL),
                                           BDS_OBJECT_MEMBER(&__dumb, __dumb.stuff, BDS_OBJECT_PTR_DATA, get_stuff_len),
                                           BDS_OBJECT_MEMBER(&__dumb, __dumb.num_stuff, BDS_OBJECT_DATA, NULL)};

static const struct bds_object_desc dumb_desc = {sizeof(struct dumb), sizeof(dumb_members) / sizeof(*dumb_members),
                                                 &dumb_members[0]};

struct device_desc {
        long size;
        long (*bounds)[2];
        long *c_idx;
        int num_dims;
        struct dumb *dumb_ptr;
        struct dumb dumb;
};

size_t get_bounds_len(struct device_desc *desc) { return desc->num_dims * sizeof(*desc->bounds); }
size_t get_dims_len(struct device_desc *desc) { return desc->num_dims * sizeof(long); }

static struct device_desc __dd;
struct bds_object_member dd_members[] = {
    BDS_OBJECT_MEMBER(&__dd, __dd.size, BDS_OBJECT_DATA, NULL),
    BDS_OBJECT_MEMBER(&__dd, __dd.bounds, BDS_OBJECT_PTR_DATA, (void *)get_bounds_len),
    BDS_OBJECT_MEMBER(&__dd, __dd.c_idx, BDS_OBJECT_PTR_DATA, (void *)get_dims_len),
    BDS_OBJECT_MEMBER(&__dd, __dd.num_dims, BDS_OBJECT_DATA, NULL),
    BDS_OBJECT_MEMBER(&__dd, __dd.dumb_ptr, BDS_OBJECT_PTR_OBJECT, (void *)&dumb_desc),
    BDS_OBJECT_MEMBER(&__dd, __dd.dumb, BDS_OBJECT_DATA_OBJECT, (void *)&dumb_desc),
};

static const struct bds_object_desc dd_desc = {sizeof(struct device_desc), sizeof(dd_members) / sizeof(*dd_members),
                                               &dd_members[0]};

static void add_serial_len(const char *object, const struct bds_object_desc *object_desc, size_t *members_len,
                           size_t *alloc_len)
{
        const char *alloc_data;

        if (members_len) {
                *members_len += object_desc->o_len;
        }

        // First get the storage requirements for the members
        for (size_t i = 0; i < object_desc->num_members; ++i) {

                const struct bds_object_member *member = object_desc->members + i;
                switch (member->o_type) {
                case BDS_OBJECT_DATA:
                        break;
                case BDS_OBJECT_PTR_DATA:
                        alloc_data = *(char **)(object + member->o_offset);
                        if (member->o_config && alloc_data) {
                                *alloc_len += ((size_t(*)(const void *))member->o_config)(object);
                        }
                        break;
                case BDS_OBJECT_DATA_OBJECT:
                        if (member->o_config) {
                                add_serial_len(object + member->o_offset, (struct bds_object_desc *)member->o_config,
                                               NULL, alloc_len);
                        }
                        break;
                case BDS_OBJECT_PTR_OBJECT:
                        alloc_data = *(char **)(object + member->o_offset);
                        if (member->o_config && alloc_data) {
                                add_serial_len(alloc_data, (struct bds_object_desc *)member->o_config, alloc_len,
                                               alloc_len);
                        }
                        break;
                default:
                        fprintf(stderr, "bds::serialize::add_serial_len: incorrect object member type %d\n",
                                member->o_type);
                        exit(EXIT_FAILURE);
                }
        }
}

static void serialize_object(const char *object, const struct bds_object_desc *object_desc, char **members_seg,
                             char **alloc_seg)
{
        char *members_seg_start = *members_seg;

        // First get the storage requirements for the members
        for (size_t i = 0; i < object_desc->num_members; ++i) {

                const struct bds_object_member *member = object_desc->members + i;

                long int incr;
                if (i < object_desc->num_members - 1) {
                        incr = (member + 1)->o_offset - member->o_offset;
                } else {
                        incr = object_desc->o_len - member->o_offset;
                }

                switch (member->o_type) {

                case BDS_OBJECT_DATA:
                        memcpy(*members_seg, object + member->o_offset, member->o_len);
                        *members_seg += incr;

                        break;
                case BDS_OBJECT_PTR_DATA:
                        assert(member->o_len == sizeof(*members_seg));
                        // Set pointer to NULL
                        memset(*members_seg, 0, member->o_len);

                        char *alloc_data = *(char **)(object + member->o_offset);
                        if (member->o_config && alloc_data) {
                                memcpy(*members_seg, (char *)alloc_seg, member->o_len);

                                size_t alloc_len = ((size_t(*)(const void *))member->o_config)(object);
                                memcpy(*alloc_seg, alloc_data, alloc_len);

                                *alloc_seg += alloc_len;
                        }
                        *members_seg += incr;

                        break;
                case BDS_OBJECT_DATA_OBJECT:
                        if (member->o_config) {
                                serialize_object(object + member->o_offset, (struct bds_object_desc *)member->o_config,
                                                 members_seg, alloc_seg);
                        } else {
                                memcpy(*members_seg, object + member->o_offset, member->o_len);
                                *members_seg += incr;
                        }
                        break;
                case BDS_OBJECT_PTR_OBJECT:
                        assert(member->o_len == sizeof(*members_seg));
                        memset(*members_seg, 0, member->o_len);

                        char *alloc_object = *(char **)(object + member->o_offset);
                        if (member->o_config && alloc_object) {

                                size_t member_len = 0;
                                size_t alloc_len  = 0;

                                add_serial_len(alloc_object, (struct bds_object_desc *)member->o_config, &member_len,
                                               &alloc_len);

                                char *__members_seg   = *alloc_seg;
                                char *members_seg_end = (*alloc_seg += member_len);
                                char *alloc_seg_end   = *alloc_seg + alloc_len;

                                memcpy(*members_seg, &__members_seg, member->o_len);
                                serialize_object(alloc_object, (struct bds_object_desc *)member->o_config,
                                                 &__members_seg, alloc_seg);

                                assert(members_seg_end == __members_seg);
                                assert(alloc_seg_end == *alloc_seg);
                        }

                        *members_seg += incr;
                        break;
                default:
                        fprintf(stderr, "bds::serialize::add_serial_len: incorrect object member type %d\n",
                                member->o_type);
                        exit(EXIT_FAILURE);
                }
        }

        assert(object_desc->o_len == (*members_seg - members_seg_start));
}

void bds_serialize(const void *object, const struct bds_object_desc *object_desc, size_t *serial_len,
                   void **serial_object)
{
        size_t members_len = 0;
        size_t alloc_len   = 0;

        add_serial_len(object, object_desc, &members_len, &alloc_len);

        *serial_len    = members_len + alloc_len;
        *serial_object = calloc(1, *serial_len);

        char *members_seg = (char *)(*serial_object);
        char *alloc_seg   = members_seg + members_len;

        serialize_object(object, object_desc, &members_seg, &alloc_seg);

        assert(*serial_object + members_len == members_seg);
        assert(*serial_object + *serial_len == alloc_seg);
}

int main(int argc, char **argv)
{
        /* struct device_desc { */
        /*         long size; */
        /*         long (*bounds)[2]; */
        /*         long *c_idx; */
        /*         int num_dims; */
        /*         struct dumb *dumb_ptr; */
        /*         struct dumb dumb; */
        /* }; */
        /* struct dumb { */
        /*         int i; */
        /*         long *stuff; */
        /*         int num_stuff; */
        /* }; */

        struct device_desc dd = {0};

        dd.size           = 42;
        dd.num_dims       = 3;
        dd.bounds         = malloc(3 * sizeof(*dd.bounds));
        dd.c_idx          = malloc(3 * sizeof(*dd.c_idx));
        dd.dumb_ptr       = malloc(sizeof(*dd.dumb_ptr));
        dd.dumb.i         = 91;
        dd.dumb.num_stuff = 2;
        dd.dumb.stuff     = malloc(2 * sizeof(*dd.dumb.stuff));

        dd.dumb_ptr->i         = 104;
        dd.dumb_ptr->num_stuff = 4;
        dd.dumb_ptr->stuff     = malloc(4 * sizeof(*dd.dumb_ptr->stuff));

        int k = 0;
        for (int i = 0; i < 3; ++i) {
                dd.c_idx[i] = (k + 1) * 2;
                for (int j = 0; j < 2; ++j) {
                        dd.bounds[i][j] = ++k;
                }
        }

        dd.dumb.stuff[0] = 99;
        dd.dumb.stuff[1] = 100;

        dd.dumb_ptr->stuff[0] = 199;
        dd.dumb_ptr->stuff[1] = 200;

        void *s_dd = NULL;
        size_t s_dd_len;

        bds_serialize(&dd, &dd_desc, &s_dd_len, &s_dd);

        assert(s_dd);

        struct device_desc *serial_dd = (struct device_desc *)s_dd;

        return 0;
}
