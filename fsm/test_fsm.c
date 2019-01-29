/*
 * Copyright (C) 2019 Jason Graham <jgraham@compukix.net>
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
#include <stdlib.h>

#include <libbds/bds_fsm.h>

// #define OPEN_WITH_C4

// Flags
#define DOOR_OPEN 0
#define DOOR_CLOSED (1 << 0)
#define DOOR_LOCKED (1 << 1)

#define NUM_STATES (1 << 2)

// States
#define ST_OPENED DOOR_OPEN
#define ST_CLOSED DOOR_CLOSED
#define ST_CLOSED_LOCKED (DOOR_CLOSED | DOOR_LOCKED)

#define CLR_BITS(a, b) (a ^= (b))
#define SET_BITS(a, b) (a |= (b))

bds_fsm_transition_t transition_table[NUM_STATES][NUM_STATES];

int open_door(void *param, int state, int *dst_state);
int close_door(void *param, int state, int *dst_state);
int lock_door(void *param, int state, int *dst_state);
int unlock_door(void *param, int state, int *dst_state);

void setup_transition_table()
{
        for (int i = 0; i < NUM_STATES; ++i) {
                for (int j = 0; j < NUM_STATES; ++j) {
                        transition_table[i][j] = (i == j ? bds_fsm_self_transition : bds_fsm_err_transition);
                }
        }

#ifdef OPEN_WITH_C4
        transition_table[ST_OPENED][ST_CLOSED_LOCKED] = bds_fsm_direct_transition;
        transition_table[ST_OPENED][ST_CLOSED]        = bds_fsm_direct_transition;
#else
        transition_table[ST_OPENED][ST_CLOSED_LOCKED] = unlock_door;
        transition_table[ST_OPENED][ST_CLOSED]        = open_door;
#endif
        transition_table[ST_CLOSED][ST_OPENED]        = close_door;
        transition_table[ST_CLOSED][ST_CLOSED_LOCKED] = unlock_door;

        transition_table[ST_CLOSED_LOCKED][ST_OPENED] = close_door;
        transition_table[ST_CLOSED_LOCKED][ST_CLOSED] = lock_door;
}

int open_door(void *param, int state, int *dst_state)
{
        assert((state & DOOR_LOCKED) == 0);
        CLR_BITS(state, DOOR_CLOSED);
        return SET_BITS(state, DOOR_OPEN);
}

int close_door(void *param, int state, int *dst_state)
{
        assert(state == DOOR_OPEN);
        return SET_BITS(state, DOOR_CLOSED);
}

int lock_door(void *param, int state, int *dst_state)
{
        assert((state & DOOR_LOCKED) == 0);
        return SET_BITS(state, DOOR_LOCKED);
}

int unlock_door(void *param, int state, int *dst_state)
{
        assert(state & DOOR_LOCKED);
        return CLR_BITS(state, DOOR_LOCKED);
}

int main(int argc, char **argv)
{
        setup_transition_table();

        struct bds_fsm *fsm = bds_fsm_alloc(ST_OPENED, NUM_STATES, transition_table[0]);

        assert(bds_fsm_transition(fsm, NULL, ST_CLOSED) == ST_CLOSED);
        assert(bds_fsm_transition(fsm, NULL, ST_CLOSED_LOCKED) == ST_CLOSED_LOCKED);
        assert(bds_fsm_transition(fsm, NULL, ST_OPENED) == ST_OPENED);

        bds_fsm_free(&fsm);
}
