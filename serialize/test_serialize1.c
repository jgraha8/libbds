#include <string.h>
#include <libbds/bds_serialize.h>

struct data {
        int i;
        float f;
        double d;
};

struct data __data_template;
struct bds_object_member data_members[] = {
    BDS_OBJECT_MEMBER(&__data_template, i, BDS_OBJECT_DATA, NULL),
    BDS_OBJECT_MEMBER(&__data_template, f, BDS_OBJECT_DATA, NULL),
    BDS_OBJECT_MEMBER(&__data_template, d, BDS_OBJECT_DATA, NULL)};

#define ARRAY_SIZE(a) ( sizeof(a) / sizeof((a)[0]) )

int main(int argc, char **argv)
{
	int rc=0;
	struct data d = { 1, 2.0f, 3.0 };
	struct bds_object_desc data_desc = { sizeof(struct data), ARRAY_SIZE(data_members), &data_members[0] };

	size_t serial_len = 0;
	void *serial_data;

	bds_serialize(&d, &data_desc, &serial_len, &serial_data);

	if( memcmp(&d, serial_data, sizeof(d)) != 0 ) {
		rc=1;
	}

	struct data dd;
	bds_deserialize(serial_data, &data_desc, &dd );

	if( memcmp(&dd, serial_data, sizeof(d)) != 0 ) {
		rc=1;
	}
	
	free(serial_data);
	
	return rc;
}
