/** @file
 *  @brief Data-structure serialization module
 *
 *  @author Jason Graham <jgraham@compukix.net>
 */

#ifndef __BDS_SERIALIZE_H__
#define __BDS_SERIALIZE_H__

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BDS_OBJECT_MEMBER(obj_ptr, member, type, config)                                           \
        {                                                                                          \
                (void *)&(obj_ptr)->member - (void *)(obj_ptr), sizeof((obj_ptr)->member), type,   \
                    config                                                                         \
        }

enum bds_object_type {
        BDS_OBJECT_DATA,
        BDS_OBJECT_DATA_OBJECT,
        BDS_OBJECT_PTR_DATA,
        BDS_OBJECT_PTR_OBJECT,
};

struct bds_object_member {
        size_t offset;
        size_t len;
        enum bds_object_type type;
        void *config;
};

struct bds_object_desc {
        size_t object_len;
        size_t num_members;
        struct bds_object_member *members;
};

void bds_serialize(const void *object, const struct bds_object_desc *object_desc,
                   size_t *serial_len, void **serial_object);

void bds_serialize_rel_addr(const void *object, const struct bds_object_desc *object_desc,
                           size_t *serial_len, void **serial_object);

void bds_convert_abs_addr(void *serial_object, const struct bds_object_desc *object_desc );
void bds_convert_rel_addr(void *serial_object, const struct bds_object_desc *object_desc );	

void bds_deserialize(const void *serial_object, const struct bds_object_desc *object_desc,
                     void *object);

#ifdef __cplusplus
}
#endif

#endif
