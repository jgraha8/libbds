#include <libbds/bds_serialize.h>

struct list_node {
        int id;
        struct list_node *next;
};

struct list_node __list_node;

static struct bds_object_desc list_node_desc;

struct bds_object_member list_node_members[] = {
    BDS_OBJECT_MEMBER(&__list_node, id, BDS_OBJECT_DATA, NULL),
    BDS_OBJECT_MEMBER(&__list_node, next, BDS_OBJECT_PTR_OBJECT, (void *)&list_node_desc)};

int check_data(struct list_node *a, struct list_node *b)
{
        while (a && b) {
                if (a->id != b->id)
                        return 1;
                a = a->next;
                b = b->next;
        }
        return 0;
}

void free_list(struct list_node *node)
{
        while (node) {
                struct list_node *next = node->next;
                free(node);
                node = next;
        }
}

int main(int argc, char **argv)
{
        struct list_node head  = {0};
        struct list_node *node = &head;

        for (int i = 0; i < 100; ++i) {
                node->id   = i;
                node->next = calloc(1, sizeof(*node));
                node       = node->next;
        }

        size_t serial_len;
        void *serial_data;

        list_node_desc.object_len  = sizeof(struct list_node);
        list_node_desc.num_members = 2;
        list_node_desc.members     = &list_node_members[0];

        bds_serialize(&head, &list_node_desc, &serial_len, &serial_data);

        if (check_data(&head, (struct list_node *)serial_data) != 0)
                return 1;
	free_list(head.next);

        struct list_node dhead = {0};
        bds_deserialize(serial_data, &list_node_desc, &dhead);

        if (check_data(&dhead, (struct list_node *)serial_data) != 0)
                return 1;

	free_list(dhead.next);
	free(serial_data);

        return 0;
}
