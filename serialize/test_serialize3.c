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
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include <libbds/bds_serialize.h>

#define N 2

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

                if ((a && !b) || (!a && b))
                        return 1;
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

        node->id = 0;
        for (int i = 1; i < N; ++i) {
                node->next = calloc(1, sizeof(*node));
                node       = node->next;
                node->id   = i;
        }

        size_t serial_len;
        void *serial_data;

        list_node_desc.object_len  = sizeof(struct list_node);
        list_node_desc.num_members = 2;
        list_node_desc.members     = &list_node_members[0];

	printf("Current alignment: %zd\n", bds_serial_alignment(0));
	printf("New alignment: %zd\n", bds_serial_alignment(16384));
	printf("Default alignment: %zd\n", bds_serial_alignment(-1));	
	printf("New alignment: %zd\n", bds_serial_alignment(16384));
	
        bds_serialize(&head, &list_node_desc, &serial_len, &serial_data);

	struct list_node *__node = (struct list_node *)serial_data;
	while( __node ) {
		assert( (long)__node % bds_serial_alignment(0) == 0 );
		__node = __node->next;
	}
	
        if (check_data(&head, (struct list_node *)serial_data) != 0)
                return 1;

        struct list_node head2 = {0};
        bds_deserialize(serial_data, &list_node_desc, &head2);

        if (check_data(&head2, (struct list_node *)serial_data) != 0)
                return 1;
        if (check_data(&head2, &head) != 0)
                return 1;

        void *serial_data2 = calloc(1, serial_len);
        memcpy(serial_data2, serial_data, serial_len);

        bds_update_serial_ptrs(serial_data2, &list_node_desc);
        bds_deserialize(serial_data2, &list_node_desc, &head2);

        if (check_data(&head2, (struct list_node *)serial_data2) != 0)
                return 1;
        if (check_data(&head2, &head) != 0)
                return 1;

        free_list(head.next);
        free_list(head2.next);
        free(serial_data);
        free(serial_data2);

        return 0;
}
