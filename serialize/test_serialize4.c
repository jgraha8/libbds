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
	int rc=0;
	struct data d;

	d.num_dims = 2;
	d.bounds = calloc(d.num_dims, sizeof(*d.bounds));
	d.c_idx = calloc(d.num_dims, sizeof(*d.c_idx));
	
	for( int i=0; i<d.num_dims; ++i ) {
		d.bounds[i][0] = i + 100;
		d.bounds[i][1] = i + 101;

		d.c_idx[i] = i + 200;
	}

	size_t serial_len;
	void *serial_data;

	bds_serialize_rel_addr(&d, &data_desc, &serial_len, &serial_data);
	bds_convert_abs_addr(serial_data, &data_desc );
	
	rc = check_data(&d, (struct data *)serial_data);

	if( rc != 0 )
		goto finish;


	bds_convert_rel_addr(serial_data, &data_desc );			
	
	struct data dd = {0};
	bds_deserialize(serial_data, &data_desc, &dd);

	rc = check_data(&dd, &d);

	if( rc != 0 ) {
		goto finish;
	}
	free(d.bounds);
	free(d.c_idx);
	free(dd.bounds);
	free(dd.c_idx);
	
finish:
	free(serial_data);

	return rc;
}
