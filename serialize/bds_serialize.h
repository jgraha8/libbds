/** @file
 *  @brief Data-structure serialization module
 *
 *  @author Jason Graham <jgraham@compukix.net>
 */

#ifndef __BDS_SERIALIZE_H__
#define __BDS_SERIALIZE_H__

#ifdef __cplusplus
extern "C" {
#endif

#define BDS_OBJECT_MEMBER(obj, member, type, config)                                                                   \
        {                                                                                                              \
                (void *)&member - (void *)obj, sizeof(member), type, config                                            \
        }

enum bds_object_type {
        BDS_OBJECT_DATA,
        BDS_OBJECT_DATA_OBJECT,
        BDS_OBJECT_PTR_DATA,
        BDS_OBJECT_PTR_OBJECT,
};

struct bds_object_member {
        size_t o_offset;
        size_t o_len;
        enum bds_object_type o_type;
        void *o_config;
};

struct bds_object_desc {
        size_t num_members;
        struct bds_object_member *members;
};

void bds_serialize(const void *object, const struct bds_object_desc *object_desc, size_t *serial_len,
                   void **serial_object);

#ifdef __cplusplus
}
#endif

#endif
