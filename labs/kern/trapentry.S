/* See COPYRIGHT for copyright information. */

#include <inc/mmu.h>
#include <inc/memlayout.h>
#include <inc/trap.h>



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
TRAPHANDLER_NOEC(i_divide_hnd, 0)
TRAPHANDLER_NOEC(i_debug_hnd, 1)
TRAPHANDLER_NOEC(i_nmi_hnd, 2)
TRAPHANDLER_NOEC(i_brkpt_hnd, 3)
TRAPHANDLER_NOEC(i_oflow_hnd, 4)
TRAPHANDLER_NOEC(i_bound_hnd, 5)
TRAPHANDLER_NOEC(i_illop_hnd, 6)
TRAPHANDLER_NOEC(i_device_hnd, 7)
TRAPHANDLER(i_dblflt_hnd, 8)
TRAPHANDLER_NOEC(i_tss_hnd, 10)
TRAPHANDLER(i_segnp_hnd, 11)
TRAPHANDLER(i_stack_hnd, 12)
TRAPHANDLER(i_gpflt_hnd, 13)
TRAPHANDLER(i_pgflt_hnd, 14)
TRAPHANDLER_NOEC(i_fperr_hnd, 16)
TRAPHANDLER_NOEC(i_align_hnd, 17)
TRAPHANDLER_NOEC(i_mchk_hnd, 18)
TRAPHANDLER_NOEC(i_simderr_hnd, 19)
TRAPHANDLER_NOEC(i_syscall_hnd, 48)
TRAPHANDLER_NOEC(i_default_hnd, 500)

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