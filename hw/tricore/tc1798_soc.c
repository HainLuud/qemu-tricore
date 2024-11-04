/*
 * Infineon tc1798 SoC System emulation.
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

#include "qemu/osdep.h"
#include "qapi/error.h"
#include "hw/sysbus.h"
#include "hw/loader.h"
#include "qemu/units.h"
#include "hw/misc/unimp.h"

#include "hw/tricore/tc1798_soc.h"
#include "hw/tricore/triboard.h"

const MemmapEntry tc1798_soc_memmap[] = {
    // [TC1798_DSPR2]     = { 0x50000000,            120 * KiB },
    // [TC1798_DCACHE2]   = { 0x5001E000,              8 * KiB },
    // [TC1798_DTAG2]     = { 0x500C0000,                0xC00 },
    // [TC1798_PSPR2]     = { 0x50100000,             32 * KiB },
    // [TC1798_PCACHE2]   = { 0x50108000,             16 * KiB },
    // [TC1798_PTAG2]     = { 0x501C0000,               0x1800 },
    // [TC1798_DSPR1]     = { 0x60000000,            120 * KiB },
    // [TC1798_DCACHE1]   = { 0x6001E000,              8 * KiB },
    // [TC1798_DTAG1]     = { 0x600C0000,                0xC00 },
    // [TC1798_PSPR1]     = { 0x60100000,             32 * KiB },
    // [TC1798_PCACHE1]   = { 0x60108000,             16 * KiB },
    // [TC1798_PTAG1]     = { 0x601C0000,               0x1800 },
    // [TC1798_DSPR0]     = { 0x70000000,            112 * KiB },
    // [TC1798_PSPR0]     = { 0x70100000,             24 * KiB },
    // [TC1798_PCACHE0]   = { 0x70106000,              8 * KiB },
    // [TC1798_PTAG0]     = { 0x701C0000,                0xC00 },
    [TC1798_PFLASH0_C] = { 0x80000000,              2 * MiB },
    [TC1798_PFLASH1_C] = { 0x80800000,              2 * MiB },
    // [TC1798_EXTEBU_C]  = { 0x83000000,            192 * MiB },
    [TC1798_OLDA_C]    = { 0x8FE70000,             32 * KiB },
    [TC1798_BROM_C]    = { 0x8FFFC000,             16 * KiB },
    [TC1798_LMURAM_C]  = { 0x90000000,            128 * KiB },
    [TC1798_EMEM_C]    = { 0x9F000000,            768 * KiB },
    [TC1798_PFLASH0_U] = { 0xA0000000,                  0x0 },
    [TC1798_PFLASH1_U] = { 0xA0800000,                  0x0 },
    // [TC1798_EXTEBU_C]  = { 0xA3000000,                  0x0 },
    [TC1798_DFLASH0]   = { 0xAF000000,             96 * KiB },
    [TC1798_DFLASH1]   = { 0xAF080000,             96 * KiB },
    [TC1798_OLDA_U]    = { 0xAFE70000,                  0x0 },
    [TC1798_BROM_U]    = { 0xAFFFC000,                  0x0 },
    [TC1798_LMURAM_U]  = { 0xB0000000,                  0x0 },
    [TC1798_EMEM_U]    = { 0xBF000000,                  0x0 },
    [TC1798_PSPR0]     = { 0xC0000000,             32 * KiB }, 
    [TC1798_PCACHE0]   = { 0xC0200000,             16 * KiB },
    [TC1798_PTAG0]     = { 0xC0300000,                0x200 },
    [TC1798_PSPR1]     = { 0xC8000000,             32 * KiB },
    [TC1798_PCACHE1]   = { 0xC8200000,             16 * KiB }, 
    [TC1798_PTAG1]     = { 0xC8300000,                0x200 },
    [TC1798_DSPR0]     = { 0xD0000000,            128 * KiB },
    [TC1798_DCACHE0]   = { 0xD0200000,             16 * KiB },
    [TC1798_DTAG0]     = { 0xD0300000,                0x200 },
    [TC1798_DSPR1]     = { 0xD8000000,            128 * KiB },
    [TC1798_DCACHE1]   = { 0xD8200000,             16 * KiB },
    [TC1798_DTAG1]     = { 0xD8300000,                0x200 },

    [TC1798_VIRT]      = { 0xBF000000,                  0x0 },

    [TC1798_SFR]       = { 0xF0000000,                  0x0 },
    [TC1798_STM]       = { 0xF0000000,                  0x0 },
    [TC1798_SCU]       = { 0xF0000500,                  0x0 },
    [TC1798_ASCLIN]    = { 0xF0000A00,                  0x0 },
    
    // TODO: Figure out what to put here
    // There's no IRBUS in TC1798, though there is ICU which I try locate here
    // ICU Interrupt Control Register located at 0xF7E1FE2C. Alternative would be 
    // to use the Peripheral Control Unit's Interrupt Control Unit (PICU) somewhere around address 0xF0043F00
    [TC1798_IRBUS]     = { 0xF7E10000,                  0x0 },  
};

/*
 * Initialize the auxiliary ROM region @mr and map it into
 * the memory map at @base.
 */
static void make_rom(MemoryRegion *mr, const char *name,
                     hwaddr base, hwaddr size)
{
    memory_region_init_rom(mr, NULL, name, size, &error_fatal);
    memory_region_add_subregion(get_system_memory(), base, mr);
}

/*
 * Initialize the auxiliary RAM region @mr and map it into
 * the memory map at @base.
 */
static void make_ram(MemoryRegion *mr, const char *name,
                     hwaddr base, hwaddr size)
{
    memory_region_init_ram(mr, NULL, name, size, &error_fatal);
    memory_region_add_subregion(get_system_memory(), base, mr);
}

/*
 * Create an alias of an entire original MemoryRegion @orig
 * located at @base in the memory map.
 */
static void make_alias(MemoryRegion *mr, const char *name,
                           MemoryRegion *orig, hwaddr base)
{
    memory_region_init_alias(mr, NULL, name, orig, 0,
                             memory_region_size(orig));
    memory_region_add_subregion(get_system_memory(), base, mr);
}


static void tc1798_soc_init_memory_mapping(DeviceState *dev_soc)
{
    TC1798SoCState *s = TC1798_SOC(dev_soc);
    TC1798SoCClass *sc = TC1798_SOC_GET_CLASS(s);

    /* shortcuts to clean up code */
    const MemmapEntry *map = sc->memmap;
    TC1798SoCCPUMemState *c0 = &s->cpu0mem;
    TC1798SoCCPUMemState *c1 = &s->cpu1mem;
    TC1798SoCFlashMemState *f = &s->flashmem;

    make_ram(&c0->dspr, "CPU0.DSPR", map[TC1798_DSPR0].base, map[TC1798_DSPR0].size);
    make_ram(&c0->pspr, "CPU0.PSPR", map[TC1798_PSPR0].base, map[TC1798_PSPR0].size);
    make_ram(&c1->dspr, "CPU1.DSPR", map[TC1798_DSPR1].base, map[TC1798_DSPR1].size);
    make_ram(&c1->pspr, "CPU1.PSPR", map[TC1798_PSPR1].base, map[TC1798_PSPR1].size);
    // make_ram(&c2->dspr, "CPU2.DSPR", map[TC1798_DSPR2].base, map[TC1798_DSPR2].size);
    // make_ram(&c2->pspr, "CPU2.PSPR", map[TC1798_PSPR2].base, map[TC1798_PSPR2].size);

    /* TODO: Control Cache mapping with Memory Test Unit (MTU) */
    // make_ram(&c2->dcache, "CPU2.DCACHE", map[TC1798_DCACHE2].base, map[TC1798_DCACHE2].size);
    // make_ram(&c2->dtag,   "CPU2.DTAG", map[TC1798_DTAG2].base, map[TC1798_DTAG2].size);
    // make_ram(&c2->pcache, "CPU2.PCACHE", map[TC1798_PCACHE2].base, map[TC1798_PCACHE2].size);
    // make_ram(&c2->ptag,   "CPU2.PTAG", map[TC1798_PTAG2].base, map[TC1798_PTAG2].size);
    make_ram(&c1->dcache, "CPU1.DCACHE", map[TC1798_DCACHE1].base, map[TC1798_DCACHE1].size);
    make_ram(&c1->dtag,   "CPU1.DTAG", map[TC1798_DTAG1].base, map[TC1798_DTAG1].size);
    make_ram(&c1->pcache, "CPU1.PCACHE", map[TC1798_PCACHE1].base, map[TC1798_PCACHE1].size);
    make_ram(&c1->ptag,   "CPU1.PTAG", map[TC1798_PTAG1].base, map[TC1798_PTAG1].size);
    make_ram(&c0->dcache, "CPU0.DCACHE", map[TC1798_DCACHE0].base, map[TC1798_DCACHE0].size);
    make_ram(&c0->dtag,   "CPU0.DTAG", map[TC1798_DTAG0].base, map[TC1798_DTAG0].size);
    make_ram(&c0->pcache, "CPU0.PCACHE", map[TC1798_PCACHE0].base, map[TC1798_PCACHE0].size);
    make_ram(&c0->ptag,   "CPU0.PTAG", map[TC1798_PTAG0].base, map[TC1798_PTAG0].size);

    /*
     * TriCore QEMU executes CPU0 only, thus it is sufficient to map
     * LOCAL.PSPR/LOCAL.DSPR exclusively onto PSPR0/DSPR0.
     */
    // make_alias(&s->psprX, "LOCAL.PSPR", &c0->pspr, map[TC1798_PSPRX].base);
    // make_alias(&s->dsprX, "LOCAL.DSPR", &c0->dspr, map[TC1798_DSPRX].base);

    make_ram(&f->pflash0_c, "PF0", map[TC1798_PFLASH0_C].base, map[TC1798_PFLASH0_C].size);
    make_ram(&f->pflash1_c, "PF1", map[TC1798_PFLASH1_C].base, map[TC1798_PFLASH1_C].size);
    make_ram(&f->dflash0,   "DF0", map[TC1798_DFLASH0].base, map[TC1798_DFLASH0].size);
    make_ram(&f->dflash1,   "DF1", map[TC1798_DFLASH1].base, map[TC1798_DFLASH1].size);
    make_ram(&f->olda_c,    "OLDA", map[TC1798_OLDA_C].base, map[TC1798_OLDA_C].size);
    make_rom(&f->brom_c,    "BROM", map[TC1798_BROM_C].base, map[TC1798_BROM_C].size);
    make_ram(&f->lmuram_c,  "LMURAM", map[TC1798_LMURAM_C].base, map[TC1798_LMURAM_C].size);
    make_ram(&f->emem_c,    "EMEM", map[TC1798_EMEM_C].base, map[TC1798_EMEM_C].size);

    make_alias(&f->pflash0_u, "PF0.U",    &f->pflash0_c, map[TC1798_PFLASH0_U].base);
    make_alias(&f->pflash1_u, "PF1.U",    &f->pflash1_c, map[TC1798_PFLASH1_U].base);
    make_alias(&f->olda_u,    "OLDA.U",   &f->olda_c, map[TC1798_OLDA_U].base);
    make_alias(&f->brom_u,    "BROM.U",   &f->brom_c, map[TC1798_BROM_U].base);
    make_alias(&f->lmuram_u,  "LMURAM.U", &f->lmuram_c, map[TC1798_LMURAM_U].base);
    make_alias(&f->emem_u,    "EMEM.U",   &f->emem_c, map[TC1798_EMEM_U].base); // Commented out in tc27xd for some reason
}

static void tc1798_soc_realize(DeviceState *dev_soc, Error **errp)
{
    TC1798SoCState *s = TC1798_SOC(dev_soc);
    TC1798SoCClass *sc = TC1798_SOC_GET_CLASS(s);
    Error *err = NULL;

    qdev_realize(DEVICE(&s->cpu), NULL, &err);
    if (err) {
        error_propagate(errp, err);
        return;
    }

    tc1798_soc_init_memory_mapping(dev_soc);

    /* now init peripherals */
    MemoryRegion *sysmem = get_system_memory();
    
    /* Create interrupt router */
    s->cpu_irq = tricore_cpu_ir_init(&s->cpu);

    /* Register: Interrupt Router Bus (IRBUS) */
    s->irbus = TRICORE_IRBUS(object_new(TYPE_TRICORE_IRBUS));
    s->asclin = TRICORE_ASCLIN(object_new(TYPE_TRICORE_ASCLIN));
    s->virt = TRICORE_VIRT(object_new(TYPE_TRICORE_VIRT));
    s->scu = TRICORE_SCU(object_new(TYPE_TRICORE_SCU));
    s->stm = TRICORE_STM(object_new(TYPE_TRICORE_STM));
    s->sfr = TRICORE_SFR(object_new(TYPE_TRICORE_SFR));
    
    /* setup links*/
    object_property_add_const_link(OBJECT(s->irbus), "cpu", OBJECT(&s->cpu));
    object_property_add_const_link(OBJECT(s->scu), "cpu", OBJECT(&s->cpu));
    object_property_add_const_link(OBJECT(s->stm), "scu", OBJECT(s->scu));
    qdev_prop_set_chr(DEVICE(s->asclin), "chardev", serial_hd(0));

    /* realize devices */
    sysbus_realize_and_unref(SYS_BUS_DEVICE(s->sfr), &error_fatal);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(s->irbus), &error_fatal);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(s->virt), &error_fatal);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(s->scu), &error_fatal);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(s->stm), &error_fatal);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(s->asclin), &error_fatal);

    /* attach interrupt router to the CPUs interrupt line */
    // TODO: check if interrupt logic is handled correctly
    sysbus_connect_irq(SYS_BUS_DEVICE(s->irbus), 0, s->cpu_irq[0]);
    for (int i = 0; i < IR_SRC_COUNT; i++) {
        s->irq[i] = qdev_get_gpio_in(DEVICE(s->irbus), i);
    }

    /* wire up ASCLIN interrupts */        
    sysbus_connect_irq(SYS_BUS_DEVICE(s->asclin), 0, s->irq[IR_SRC_ASCLIN0RX]);
    sysbus_connect_irq(SYS_BUS_DEVICE(s->asclin), 1, s->irq[IR_SRC_ASCLIN0TX]);
    sysbus_connect_irq(SYS_BUS_DEVICE(s->asclin), 2, s->irq[IR_SRC_ASCLIN0EX]);

    /* wire up STM interrupts */
    sysbus_connect_irq(SYS_BUS_DEVICE(s->stm), 0, s->irq[IR_SRC_STM0_SR0]);

    /* wire up SCU interrupts */
    sysbus_connect_irq(SYS_BUS_DEVICE(s->scu), 0, s->irq[IR_SRC_RESET]);

    /* finally map memory regions */
    memory_region_add_subregion(sysmem, sc->memmap[TC1798_SFR].base, &s->sfr->iomem);
    memory_region_add_subregion(sysmem, sc->memmap[TC1798_IRBUS].base, &s->irbus->srvcontrolregs);
    memory_region_add_subregion(sysmem, sc->memmap[TC1798_ASCLIN].base, &s->asclin->iomem);
    memory_region_add_subregion(sysmem, sc->memmap[TC1798_VIRT].base, &s->virt->iomem);
    memory_region_add_subregion(sysmem, sc->memmap[TC1798_SCU].base, &s->scu->iomem);
    memory_region_add_subregion(sysmem, sc->memmap[TC1798_STM].base, &s->stm->iomem);
}

static void tc1798_soc_reset(DeviceState *dev_soc)
{
    TC1798SoCState *s = TC1798_SOC(dev_soc);
    
    cpu_state_reset(&s->cpu.env);
}

static void tc1798_soc_init(Object *obj)
{
    TC1798SoCState *s = TC1798_SOC(obj);
    TC1798SoCClass *sc = TC1798_SOC_GET_CLASS(s);

    object_initialize_child(obj, "tc1798", &s->cpu, sc->cpu_type);
}

static Property tc1798_soc_properties[] = {
    DEFINE_PROP_END_OF_LIST(),
};

static void tc1798_soc_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->realize = tc1798_soc_realize;
    // dc->reset = tc1798_soc_reset; // Deprecated
    dc->legacy_reset = tc1798_soc_reset;// TODO: Temporary workaround. See qdev_core.h L 155
    device_class_set_props(dc, tc1798_soc_properties);
}

// TODO: is this necessary?
static void tc1798_gen_soc_class_init(ObjectClass *oc, void *data)
{
    TC1798SoCClass *sc = TC1798_SOC_CLASS(oc);

    sc->name         = "tc1798-instance-soc";
    sc->cpu_type     = TRICORE_CPU_TYPE_NAME("tc1798");
    sc->memmap       = tc1798_soc_memmap;
    sc->num_cpus     = 1;
}

static const TypeInfo tc1798_soc_types[] = {
    {
        .name          = "tc1798-instance-soc",
        .parent        = TYPE_TC1798_SOC,
        .class_init    = tc1798_gen_soc_class_init,
    }, {
        .name          = TYPE_TC1798_SOC,
        .parent        = TYPE_SYS_BUS_DEVICE,
        .instance_size = sizeof(TC1798SoCState),
        .instance_init = tc1798_soc_init,
        .class_size    = sizeof(TC1798SoCClass),
        .class_init    = tc1798_soc_class_init,
        .abstract      = true,
    },
};

DEFINE_TYPES(tc1798_soc_types)
