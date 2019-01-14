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
#ifndef __HEAP_UTIL_H__
#define __HEAP_UTIL_H__

#define PARENT(i) (((i + 1) >> 1) - 1)
#define LEFT(i) (((i + 1) << 1) - 1)
#define RIGHT(i) ((i + 1) << 1)
#define PTR(heap, i) (heap->data + heap->elem_len * (i))

#endif // __HEAP_UTIL_H__
