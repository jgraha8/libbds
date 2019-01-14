/*
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
#include <string.h>
#include <libbds/bds_serialize.h>

#define ARRAY_SIZE(a) ( sizeof(a) / sizeof((a)[0]) )

struct data {
        int num_dims;
	long (*bounds)[2];
	long *c_idx;
};

size_t get_bounds_len(struct data *object)
{
	return object->num_dims * sizeof(*object->bounds);
}
size_t get_c_idx_len(struct data *object)
{
	return object->num_dims * sizeof(*object->c_idx);
}

struct data __data_template;
struct bds_object_member data_members[] = {
    BDS_OBJECT_MEMBER(&__data_template, num_dims, BDS_OBJECT_DATA, NULL),
    BDS_OBJECT_MEMBER(&__data_template, bounds, BDS_OBJECT_PTR_DATA, (void *)get_bounds_len),
    BDS_OBJECT_MEMBER(&__data_template, c_idx, BDS_OBJECT_PTR_DATA, (void *)get_c_idx_len)};

struct bds_object_desc data_desc = { sizeof(struct data), ARRAY_SIZE(data_members), &data_members[0] };

int check_data( struct data *data, struct data *serial_data)
{
	if( serial_data->num_dims != data->num_dims ) {
		return 1;
	}

	if( memcmp(serial_data->bounds, data->bounds, serial_data->num_dims * sizeof(*serial_data->bounds)) != 0 ) {
		return 1;
	}

	if( memcmp(serial_data->c_idx, data->c_idx, serial_data->num_dims * sizeof(*serial_data->c_idx)) != 0 ) {
		return 1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	struct data d;

	d.num_dims = 100;
	d.bounds = calloc(d.num_dims, sizeof(*d.bounds));
	d.c_idx = calloc(d.num_dims, sizeof(*d.c_idx));
	
	for( int i=0; i<d.num_dims; ++i ) {
		d.bounds[i][0] = i + 100;
		d.bounds[i][1] = i + 101;

		d.c_idx[i] = i + 200;
	}

	size_t serial_len;
	void *serial_data;

	bds_serialize(&d, &data_desc, &serial_len, &serial_data);
	if( check_data(&d, (struct data *)serial_data) != 0 )
		return 1;

	struct data dd = {0};
	bds_deserialize(serial_data, &data_desc, &dd);

	if( check_data(&dd, (struct data *)serial_data) != 0 )
		return 1;

	void *serial_data2 = calloc(1, serial_len);
	memcpy(serial_data2, serial_data, serial_len);

	bds_update_serial_ptrs(serial_data2, &data_desc);
	bds_deserialize(serial_data2, &data_desc, &dd);

	if( check_data(&dd, (struct data *)serial_data2) != 0 )
		return 1;

	free(d.bounds);
	free(d.c_idx);
	free(dd.bounds);
	free(dd.c_idx);
	free(serial_data);
	free(serial_data2);


	return 0;
}
