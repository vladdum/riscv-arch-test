/* rvmodel_macros.h — kronos DUT macros for ACT4
 * Halt: write 0/1 to 0x40000000. Console: write bytes to 0x10000000. */
#ifndef _RVMODEL_MACROS_H
#define _RVMODEL_MACROS_H

#define RVMODEL_DATA_SECTION

/* Install a trap handler that skips illegal instructions (mepc += 4, mret).
 * This is required because ACT4 tests use .align directives that fill padding
 * with zeros (decoded as illegal instructions). Reference simulators (Spike,
 * Sail) have a built-in skip handler; we must provide one in RVMODEL_BOOT.
 * Use .option arch +zicsr so CSR instructions assemble even with -march=rv64i. */
#define RVMODEL_BOOT                                                         \
  .option push                                                              ;\
  .option norvc                                                             ;\
  .option arch, +zicsr                                                      ;\
  la      t0, _kronos_trap_handler                                          ;\
  csrw    mtvec, t0                                                         ;\
  j       _kronos_boot_done                                                 ;\
_kronos_trap_handler:                                                        \
  csrr    t0, mepc                                                          ;\
  addi    t0, t0, 4                                                         ;\
  csrw    mepc, t0                                                          ;\
  mret                                                                      ;\
_kronos_boot_done:                                                           \
  .option pop                                                               ;
#define RVMODEL_HALT_PASS             \
  li t0, 0x40000000               ;  \
  sw x0, 0(t0)                    ;  \
  1: j 1b                         ;

#define RVMODEL_HALT_FAIL             \
  li t0, 0x40000000               ;  \
  li t1, 1                        ;  \
  sw t1, 0(t0)                    ;  \
  1: j 1b                         ;

#define RVMODEL_IO_INIT(_R1, _R2, _R3)

#define RVMODEL_IO_WRITE_STR(_R1, _R2, _R3, _STR_PTR)  \
  li _R2, 0x10000000              ;                     \
  1:                              ;                     \
    lbu _R1, 0(_STR_PTR)          ;                     \
    beqz _R1, 2f                  ;                     \
    sb  _R1, 0(_R2)               ;                     \
    addi _STR_PTR, _STR_PTR, 1   ;                     \
    j 1b                          ;                     \
  2:

#define RVMODEL_ACCESS_FAULT_ADDRESS 0x00000000
#define RVMODEL_MTIME_ADDRESS        0x0200BFF8
#define RVMODEL_MTIMECMP_ADDRESS     0x02004000
#define RVMODEL_INTERRUPT_LATENCY    10
#define RVMODEL_TIMER_INT_SOON_DELAY 100
#define CLINT_BASE_ADDRESS           0x02000000
#define MSIP_ADDRESS                 (CLINT_BASE_ADDRESS + 0x0)
#define RVMODEL_SET_MEXT_INT(_R1, _R2)
#define RVMODEL_CLR_MEXT_INT(_R1, _R2)
#define RVMODEL_SET_MSW_INT(_R1, _R2)  \
  li _R1, 1;                           \
  li _R2, MSIP_ADDRESS;                \
  sw _R1, 0(_R2);
#define RVMODEL_CLR_MSW_INT(_R1, _R2)  \
  li _R2, MSIP_ADDRESS;                \
  sw zero, 0(_R2);
#define RVMODEL_SET_SEXT_INT(_R1, _R2)
#define RVMODEL_CLR_SEXT_INT(_R1, _R2)
#define RVMODEL_SET_SSW_INT(_R1, _R2)
#define RVMODEL_CLR_SSW_INT(_R1, _R2)

/* Override LA to avoid 0x0000 rvc alignment padding at rvmodel_boot */
#undef LA
#define LA(reg, val)              \
  .ifnc(reg, X0)                 ;\
    .option push                 ;\
    .option norvc                ;\
    la reg, val                  ;\
    .option pop                  ;\
  .endif

#endif /* _RVMODEL_MACROS_H */
