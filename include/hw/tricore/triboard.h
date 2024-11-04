/*
 * Infineon TriBoard System emulation.
 *
 * Copyright (c) 2020 Andreas Konopik
 * Copyright (c) 2020 David Brenken
 * Copyright (c) 2024 Georg Hofstetter <georg.hofstetter@efs-techhub.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

#include "qapi/error.h"
#include "hw/boards.h"
#include "sysemu/sysemu.h"
#include "exec/address-spaces.h"
#include "qom/object.h"

#include "hw/tricore/tc1798_soc.h"
#include "hw/tricore/tc27xd_soc.h"
#include "hw/tricore/tc39xb_soc.h"

#define TYPE_TRIBOARD_MACHINE MACHINE_TYPE_NAME("triboard")
typedef struct TriBoardMachineState TriBoardMachineState;
typedef struct TriBoardMachineClass TriBoardMachineClass;
DECLARE_OBJ_CHECKERS(TriBoardMachineState, TriBoardMachineClass,
                     TRIBOARD_MACHINE, TYPE_TRIBOARD_MACHINE)


struct TriBoardMachineState {
    MachineState parent;

    TC1798SoCState tc1798_soc;
    TC27XDSoCState tc27xd_soc;
    TC39XBSoCState tc39xb_soc;
};

struct TriBoardMachineClass {
    MachineClass parent_obj;

    const char *name;
    const char *desc;
    const char *soc_name;
};
