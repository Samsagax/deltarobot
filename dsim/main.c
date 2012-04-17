/*
 * Copyright (c) 2012, Joaquín Ignacio Aramendía
 * Author: Joaquín Ignacio Aramendía <samsagax [at] gmail [dot] com>
 *
 * This file is part of PROJECTNAME.
 *
 * PROJECTNAME is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PROJECTNAME is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PROJECTNAME. If not, see <http://www.gnu.org/licenses/>.
 */

#include "dsim_common.h"
#include "dsim_calc.h"

static const DGeometry robot = {40.0f, 50.0f, 20.0f, 30.0f};
static DPos point = { 0, 0, 50 };

void
print_robot(const DGeometry* r) {
    printf("Robot:\n");
    printf("Moving platform length: %f\n", r->h);
    printf("Fixed platform length: %f\n", r->r);
    printf("Motor limb length: %f\n", r->a);
    printf("Free limb length: %f\n", r->b);
}

void
print_DPos(const DPos* p) {
    printf("Position:\n");
    printf("X: %f\n", p->x);
    printf("Y: %f\n", p->y);
    printf("Z: %f\n", p->z);
}
void
print_DAxes(const DAxes* r) {
    printf("Axis: %f; %f; %f\n", r->rax[0], r->rax[1], r->rax[2]);
}

void
print_DExtendedAxes(const DExtendedAxes* extaxes) {
    int i;
    for ( i = 0; i < 3 ; i++ ) {
        printf("Limb %i: ", i);
        print_DAxes(&(extaxes->limb[i]));
    }
}

int
main (int argc, char* argv[]) {
    print_DPos(&point);
    DAxes rb;
    DExtendedAxes extax;
    solve_inverse(&robot, &point, &rb, &extax);
    print_DExtendedAxes(&extax);
    DPos np;
    solve_direct(&robot, &rb, &np);
    print_DPos(&np);
    return 0;
}
