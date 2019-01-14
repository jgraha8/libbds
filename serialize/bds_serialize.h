/**
 * @file
 * @brief Data-structure serialization module
 *
 * Copyright (C) 2018 Jason Graham <jgraham@compukix.net> 
 *
 * This file is part of libbds.
 *
 * libbds is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * libbds is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with libbds. If not, see
 * <https://www.gnu.org/licenses/>.
 */
#ifndef __BDS_SERIALIZE_H__
#define __BDS_SERIALIZE_H__

#include <stdlib.h>
#include <unistd.h>

#ifndef BDS_SERIAL_ALIGNMENT
#define BDS_SERIAL_ALIGNMENT 16
#endif

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

size_t bds_serial_alignment(ssize_t alignment);
	
void bds_serialize(const void *object, const struct bds_object_desc *object_desc,
                   size_t *serial_len, void **serial_object);

void bds_update_serial_ptrs(void *serial_object, const struct bds_object_desc *object_desc );

void bds_deserialize(const void *serial_object, const struct bds_object_desc *object_desc,
                     void *object);

#ifdef __cplusplus
}
#endif

#endif
