/* See COPYRIGHT for copyright information. */

#include <inc/mmu.h>
#include <inc/memlayout.h>
#include <inc/trap.h>

#include <kern/picirq.h>


###################################################################
# exceptions/interrupts
###################################################################

/* The TRAPHANDLER macro defines a globally-visible function for handling
 * a trap.  It pushes a trap number onto the stack, then jumps to _alltraps.
 * Use TRAPHANDLER for traps where the CPU automatically pushes an error code.
 */ 
#define TRAPHANDLER(name, num)						\
	.globl name;		/* define global symbol for 'name' */	\
	.type name, @function;	/* symbol type is function */		\
	.align 2;		/* align function definition */		\
	name:			/* function starts here */		\
	pushl $(num);							\
	jmp _alltraps

/* Use TRAPHANDLER_NOEC for traps where the CPU doesn''t push an error code.
 * It pushes a 0 in place of the error code, so the trap frame has the same
 * format in either case.
 */
#define TRAPHANDLER_NOEC(name, num)					\
	.globl name;							\
	.type name, @function;						\
	.align 2;							\
	name:								\
	pushl $0;							\
	pushl $(num);							\
	jmp _alltraps

.text

# http://www.read.seas.harvard.edu/~kohler/class/aosref/i386/s09_10.htm
TRAPHANDLER_NOEC(i_divide_hnd, T_DIVIDE)
TRAPHANDLER_NOEC(i_debug_hnd, T_DEBUG)
TRAPHANDLER_NOEC(i_nmi_hnd, T_NMI)
TRAPHANDLER_NOEC(i_brkpt_hnd, T_BRKPT)
TRAPHANDLER_NOEC(i_oflow_hnd, T_OFLOW)
TRAPHANDLER_NOEC(i_bound_hnd, T_BOUND)
TRAPHANDLER_NOEC(i_illop_hnd, T_ILLOP)
TRAPHANDLER_NOEC(i_device_hnd, T_DEVICE)
TRAPHANDLER(i_dblflt_hnd, T_DBLFLT)
TRAPHANDLER_NOEC(i_tss_hnd, T_TSS)
TRAPHANDLER(i_segnp_hnd, T_SEGNP)
TRAPHANDLER(i_stack_hnd, T_STACK)
TRAPHANDLER(i_gpflt_hnd, T_GPFLT)
TRAPHANDLER(i_pgflt_hnd, T_PGFLT)
TRAPHANDLER_NOEC(i_fperr_hnd, T_FPERR)
TRAPHANDLER_NOEC(i_align_hnd, T_ALIGN)
TRAPHANDLER_NOEC(i_mchk_hnd, T_MCHK)
TRAPHANDLER_NOEC(i_simderr_hnd, T_SIMDERR)
TRAPHANDLER_NOEC(i_syscall_hnd, T_SYSCALL)
TRAPHANDLER_NOEC(i_default_hnd, T_DEFAULT)

#IRQ
TRAPHANDLER_NOEC(i_irq_tmr, IRQ_OFFSET + IRQ_TIMER)
TRAPHANDLER_NOEC(i_irq_kbd, IRQ_OFFSET + IRQ_KBD)
TRAPHANDLER_NOEC(i_irq_spr, IRQ_OFFSET + IRQ_SPURIOUS)
TRAPHANDLER_NOEC(i_irq_ide, IRQ_OFFSET + IRQ_IDE)

_alltraps:
	pushl %ds
	pushl %es
	pushal
	movw $GD_KD,%ax
	movw %ax,%es
	movw %ax,%ds
	pushl %esp
	movl $0,%ebp
	call trap