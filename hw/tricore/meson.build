tricore_ss = ss.source_set()
tricore_ss.add(when: 'CONFIG_TRICORE_TESTBOARD', if_true: files('tricore_testboard.c'))
tricore_ss.add(when: 'CONFIG_TRICORE_TESTBOARD', if_true: files('tricore_testdevice.c'))
tricore_ss.add(when: 'CONFIG_TRICORE_SCU', if_true: files('tricore_scu.c'))
tricore_ss.add(when: 'CONFIG_TRICORE_VIRT', if_true: files('tricore_virt.c'))
tricore_ss.add(when: 'CONFIG_TRICORE_IRBUS', if_true: files('tricore_ir.c'))
tricore_ss.add(when: 'CONFIG_TRICORE_SFR', if_true: files('tricore_sfr.c'))
tricore_ss.add(when: 'CONFIG_TRIBOARD', if_true: files('triboard.c'))
tricore_ss.add(when: 'CONFIG_TC1798_SOC', if_true: files('tc1798_soc.c'))
tricore_ss.add(when: 'CONFIG_TC27X_SOC', if_true: files('tc27xd_soc.c'))
tricore_ss.add(when: 'CONFIG_TC39X_SOC', if_true: files('tc39xb_soc.c'))

hw_arch += {'tricore': tricore_ss}
