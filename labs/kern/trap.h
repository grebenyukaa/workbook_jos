/* See COPYRIGHT for copyright information. */

#ifndef JOS_KERN_TRAP_H
#define JOS_KERN_TRAP_H
#ifndef JOS_KERNEL
# error "This is a JOS kernel header; user programs should not #include it"
#endif

#include <inc/trap.h>
#include <inc/mmu.h>

/* The kernel's interrupt descriptor table */
extern struct Gatedesc idt[];

void idt_init(void);
void print_regs(struct PushRegs *regs);
void print_trapframe(struct Trapframe *tf);
void page_fault_handler(struct Trapframe *);
void backtrace(struct Trapframe *);

void i_divide_hnd(void);
void i_debug_hnd(void);
void i_nmi_hnd(void);
void i_brkpt_hnd(void);
void i_oflow_hnd(void);
void i_bound_hnd(void);
void i_illop_hnd(void);
void i_device_hnd(void);
void i_dblflt_hnd(void);
void i_tss_hnd(void);
void i_segnp_hnd(void);
void i_stack_hnd(void);
void i_gpflt_hnd(void);
void i_pgflt_hnd(void);
void i_fperr_hnd(void);
void i_align_hnd(void);
void i_mchk_hnd(void);
void i_simderr_hnd(void);
void i_syscall_hnd(void);
void i_default_hnd(void);

void i_irq_tmr(void);
void i_irq_kbd(void);
void i_irq_spr(void);
void i_irq_ide(void);
#endif /* JOS_KERN_TRAP_H */