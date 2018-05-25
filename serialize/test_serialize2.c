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

struct bds_object_desc data_desc = { sizeof(struct data), ARRAY_SIZE(data_members), &data_members };

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

	int rc=0;
	struct data *s_d = (struct data *)serial_data;

	if( s_d->num_dims != d.num_dims ) {
		rc=1;
		goto finish;
	}

	if( memcmp(s_d->bounds, d.bounds, s_d->num_dims * sizeof(*s_d->bounds)) != 0 ) {
		rc = 1;
		goto finish;
	}

	if( memcmp(s_d->c_idx, d.c_idx, s_d->num_dims * sizeof(*s_d->c_idx)) != 0 ) {
		rc = 1;
		goto finish;
	}
	
finish:
	free(serial_data);
	free(d.bounds);
	free(d.c_idx);

	return rc;
}
