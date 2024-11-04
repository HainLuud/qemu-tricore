/*
 * Infineon TC1798 SoC System emulation.
 *
 * Copyright (c) 2024 Hain Luud
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

#ifndef TC1798_SOC_H
#define TC1798_SOC_H

#include "hw/sysbus.h"
#include "target/tricore/cpu.h"
#include "qom/object.h"

#include "hw/tricore/tricore.h"
#include "hw/tricore/tricore_virt.h"
#include "hw/tricore/tricore_ir.h"
#include "hw/tricore/tricore_scu.h"
#include "hw/tricore/tricore_sfr.h"
#include "hw/intc/tricore_irbus.h"
#include "hw/timer/tricore_stm.h"
#include "hw/char/tricore_asclin.h"
#include "hw/tricore/tc_soc.h"

#define TYPE_TC1798_SOC ("tc1798-soc")
OBJECT_DECLARE_TYPE(TC1798SoCState, TC1798SoCClass, TC1798_SOC)

typedef struct TC1798SoCCPUMemState {

    MemoryRegion dspr;
    MemoryRegion pspr;

    MemoryRegion dcache;
    MemoryRegion dtag;
    MemoryRegion pcache;
    MemoryRegion ptag;

} TC1798SoCCPUMemState;

typedef struct TC1798SoCFlashMemState {

    MemoryRegion pflash0_c;
    MemoryRegion pflash1_c;
    MemoryRegion pflash0_u;
    MemoryRegion pflash1_u;
    MemoryRegion dflash0;
    MemoryRegion dflash1;
    MemoryRegion olda_c;
    MemoryRegion olda_u;
    MemoryRegion brom_c;
    MemoryRegion brom_u;
    MemoryRegion lmuram_c;
    MemoryRegion lmuram_u;
    MemoryRegion emem_c;
    MemoryRegion emem_u;

} TC1798SoCFlashMemState;

typedef struct TC1798SoCState {
    /*< private >*/
    SysBusDevice parent_obj;

    /*< public >*/
    TriCoreCPU cpu;

    // MemoryRegion dsprX;
    // MemoryRegion psprX;

    TC1798SoCCPUMemState cpu0mem;
    TC1798SoCCPUMemState cpu1mem;
    // TC1798SoCCPUMemState cpu2mem;

    TriCoreIRBUSState *irbus;
    TriCoreVIRTState *virt;
    TriCoreSCUState *scu;
    TriCoreSTMState *stm;
    TriCoreASCLINState *asclin;
    TriCoreSFRState *sfr;

    qemu_irq irq[IR_SRC_COUNT];
    qemu_irq *cpu_irq;

    TC1798SoCFlashMemState flashmem;

} TC1798SoCState;

typedef struct TC1798SoCClass {
    DeviceClass parent_class;

    const char *name;
    const char *cpu_type;
    const MemmapEntry *memmap;
    uint32_t num_cpus;
} TC1798SoCClass;

enum {
    TC1798_PFLASH0_C,
    TC1798_PFLASH1_C,
    TC1798_OLDA_C,
    TC1798_BROM_C,
    TC1798_LMURAM_C,
    TC1798_EMEM_C,
    TC1798_PFLASH0_U,
    TC1798_PFLASH1_U,
    TC1798_DFLASH0,
    TC1798_DFLASH1,
    TC1798_OLDA_U,
    TC1798_BROM_U,
    TC1798_LMURAM_U,
    TC1798_EMEM_U,
    // TC1798_PSPRX,
    // TC1798_DSPRX,
    TC1798_PSPR0,
    TC1798_PCACHE0,
    TC1798_PTAG0,
    TC1798_PSPR1,
    TC1798_PCACHE1,
    TC1798_PTAG1,
    TC1798_DSPR0,
    TC1798_DCACHE0,
    TC1798_DTAG0,
    TC1798_DSPR1,
    TC1798_DCACHE1,
    TC1798_DTAG1,

    TC1798_SFR,
    TC1798_VIRT,
    TC1798_IRBUS,
    TC1798_SCU,
    TC1798_STM,
    TC1798_ASCLIN,
};

#endif
