/* Copyright (c) 2015 Alinson Xavier
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PROJECT_GTSP_COLS_H
#define PROJECT_GTSP_COLS_H

#include "lp.h"
#include "gtsp.h"

int GTSP_find_columns(struct LP *lp, struct GTSP *data);

int GTSP_add_column(struct LP *lp, struct Edge *e);

#endif //PROJECT_GTSP_COLS_H
