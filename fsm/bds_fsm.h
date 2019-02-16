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

/**
 * @file
 * @brief Finite-state-machine (FSM) module
 *
 * @author Jason Graham <jgraham@compukix.net>
 *
 * Provides support for implementing a FSM.
 *
 * The FSM implemented here is defined as a quintuple @f$ (S, F, s_0, f, \sigma) @f$ with
 *
 *  - @f$ S  @f$, the set of available states
 *  - @f$ F \subseteq S  @f$, the set of valid final states
 *  - @f$ s_0  @f$, the initial state
 *  - @f$ f  @f$, the destination (final) state
 *  - @f$ \sigma : S \times F \rightarrow S @f$, the transition function
 *
 * The transition functions are stored in a table, @f$ T @f$, whose
 * look up indices are the current and destination states.
 *
 * <b> FSM State Change:</b>
 * @f{algorithmic}{
 *    \STATE $s \gets s_0$
 *    \WHILE {$s \neq f$}
 *      \STATE $s_{old} \gets s$
 *      \STATE $\sigma \gets T[f][s]$
 *      \STATE $s \gets \sigma( s, f )$
 *      \IF {$s == s_{old}$}
 *          \STATE circular error state
 *      \ELSIF {$s < 0$ }
 *	    \STATE error state
 *	\ENDIF
 *    \ENDWHILE
 * @f}
 */

#ifndef __BDS_FSM_H__
#define __BDS_FSM_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*bds_fsm_transition_t)(void *param, int state, int *dst_state);

struct bds_fsm {
        int num_states;
        int state;
	int prev_state;
        bds_fsm_transition_t **transition_table;
};

void bds_fsm_ctor(struct bds_fsm *fsm, int init_state, int num_states, bds_fsm_transition_t *transition_table);

void bds_fsm_dtor(struct bds_fsm *fsm);

struct bds_fsm *bds_fsm_alloc(int init_state, int num_states, bds_fsm_transition_t *transition_table);

void bds_fsm_free(struct bds_fsm **fsm);

int bds_fsm_transition(struct bds_fsm *fsm, void *param, int dst_state);

int bds_fsm_err_transition(void *param, int state, int *dst_state);

int bds_fsm_self_transition(void *param, int state, int *dst_state);

int bds_fsm_direct_transition(void *param, int state, int *dst_state);

#ifdef __cplusplus
}
#endif

#endif
