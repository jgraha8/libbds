/*
 * Copyright (C) 2017-2019 Jason Graham <jgraham@compukix.net>
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
#include <stdio.h>
#include <stdlib.h>

#include <libbds/bds_fsm.h>

#include "memutil.h"

void bds_fsm_ctor(struct bds_fsm *fsm, int init_state, int num_states, bds_fsm_transition_t *transition_table)
{
        memset(fsm, 0, sizeof(*fsm));

        fsm->state      = init_state;
        fsm->num_states = num_states;

        int i;
        fsm->transition_table = xalloc(num_states * sizeof(*fsm->transition_table));
        for (i = 0; i < num_states; ++i) {
                fsm->transition_table[i] = xalloc(num_states * sizeof(**fsm->transition_table));
        }

        if (transition_table) {
                bds_fsm_transition_t(*tt)[num_states] = (bds_fsm_transition_t(*)[num_states])transition_table;
                for (i = 0; i < num_states; ++i) {
                        memcpy(fsm->transition_table[i], tt[i], num_states * sizeof(bds_fsm_transition_t));
                }
        }
}

void bds_fsm_dtor(struct bds_fsm *fsm)
{
        if (fsm->transition_table) {
                int i;
                for (i = 0; i < fsm->num_states; ++i) {
                        free(fsm->transition_table[i]);
                }
                free(fsm->transition_table);
        }
        memset(fsm, 0, sizeof(*fsm));
}

struct bds_fsm *bds_fsm_alloc(int init_state, int num_states, bds_fsm_transition_t *transition_table)
{
        struct bds_fsm *fsm = malloc(sizeof(*fsm));
        assert(fsm);

        bds_fsm_ctor(fsm, init_state, num_states, transition_table);

        return fsm;
}

void bds_fsm_free(struct bds_fsm **fsm)
{
        if (*fsm == NULL)
                return;

        bds_fsm_dtor(*fsm);
        *fsm = NULL;
}

int bds_fsm_transition(struct bds_fsm *fsm, void *param, int dst_state)
{
        int new_state;
        bds_fsm_transition_t trans;
	int prev_dst_state = dst_state;
	
	fsm->prev_state = fsm->state;
	
        while (fsm->state != dst_state) {
		
                trans = fsm->transition_table[dst_state][fsm->state];
                assert(trans);

                new_state = trans(param, fsm->state, &dst_state);

                if (new_state < 0) // Error occurred
                        break;
		
		if( new_state == fsm->state &&
		    prev_dst_state == dst_state ) { // No transition (results in infinite loop)
			break; 
		}

                assert(new_state < fsm->num_states);

                fsm->state = new_state;
		prev_dst_state = dst_state;
        }

	return fsm->state;
}

int bds_fsm_self_transition(void *param, int state, int *dst_state)
{
        return state;
}

int bds_fsm_err_transition(void *param, int state, int *dst_state)
{
        return -1;
}

int bds_fsm_direct_transition(void *param, int state, int *dst_state)
{
	return *dst_state;
}
