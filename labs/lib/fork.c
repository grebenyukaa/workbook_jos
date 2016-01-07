// implement fork from user space

#include <inc/string.h>
#include <inc/lib.h>

// PTE_COW marks copy-on-write page table entries.
// It is one of the bits explicitly allocated to user processes (PTE_AVAIL).
#define PTE_COW		0x800

#define CHECK_FAIL_FORK(x)\
{\
	int errno = (x);\
	if (errno)\
	{\
		panic("[%s:%s] %e", __FILE__, __LINE__, errno);\
	}\
}

//
// Custom page fault handler - if faulting page is copy-on-write,
// map in our own private writable copy.
//
static void
pgfault(struct UTrapframe *utf)
{
	cprintf("[fork - pgfault]\n");
	void *addr = (void *)utf->utf_fault_va;
	uint32_t err = utf->utf_err;

	// Check that the faulting access was (1) a write, and (2) to a
	// copy-on-write page.  If not, panic.
	// Hint:
	//   Use the read-only page table mappings at vpt
	//   (see <inc/memlayout.h>).
	int pn = PPN(addr);
	int perm = vpt[pn] & PTE_USER;
	if (!(err & FEC_WR && perm & PTE_COW))
		panic("[fork - pgfault] not writing or or not writing to COW page\n");

	// Allocate a new page, map it at a temporary location (PFTEMP),
	// copy the data from the old page to the new page, then move the new
	// page to the old page's address.
	// Hint:
	//   You should make three system calls.
	//   No need to explicitly delete the old page's mapping.
	physaddr_t paddr = PTE_ADDR(addr);
	perm &= ~PTE_W;
	perm |= PTE_COW;

	CHECK_FAIL_FORK(sys_page_alloc(0, (void*)PFTEMP, perm));
	memmove((void*)PFTEMP, (void*)paddr, PGSIZE);

	CHECK_FAIL_FORK(sys_page_map(0, (void*)PFTEMP, 0, (void*)paddr, perm));
	CHECK_FAIL_FORK(sys_page_unmap(0, (void*)PFTEMP));
}

//
// Map our virtual page pn (address pn*PGSIZE) into the target envid
// at the same virtual address.  If the page is writable or copy-on-write,
// the new mapping must be created copy-on-write, and then our mapping must be
// marked copy-on-write as well.  (Exercise: Why might we need to mark ours
// copy-on-write again if it was already copy-on-write at the beginning of
// this function?)
//
// Returns: 0 on success, < 0 on error.
// It is also OK to panic on error.
// 
static int
duppage(envid_t envid, unsigned pn)
{
	void* va = (void*)(pn*PGSIZE);

	int perm = vpt[pn] & PTE_USER;
	if (perm & PTE_W || perm & PTE_COW)
	{
		perm &= ~PTE_W;
		perm |= PTE_COW;
		CHECK_FAIL_FORK(sys_page_map(0, va, 0, va, perm));
	}
	return sys_page_map(0, va, envid, va, perm);
}

//
// User-level fork with copy-on-write.
// Set up our page fault handler appropriately.
// Create a child.
// Copy our address space and page fault handler setup to the child.
// Then mark the child as runnable and return.
//
// Returns: child's envid to the parent, 0 to the child, < 0 on error.
// It is also OK to panic on error.
//
// Hint:
//   Use vpd, vpt, and duppage.
//   Remember to fix "env" and the user exception stack in the child process.
//   Neither user exception stack should ever be marked copy-on-write,
//   so you must allocate a new page for the child's user exception stack.
//
envid_t
fork(void)
{
	extern unsigned char end[];

	set_pgfault_handler(NULL);
	set_pgfault_handler(pgfault);

	envid_t envid = sys_exofork();
	if (envid < 0)
		panic("[fork] sys_exofork: %e", envid);
	if (envid == 0)
	{
		envid_t cenvid = sys_getenvid();
		cprintf("[fork] child! envid = %d\n", cenvid);
		env = &envs[ENVX(cenvid)];
		return 0;
	}

	cprintf("[fork] parent! envid = %d\n", envid);
	for (int pdi = 0; pdi < VPD(end); ++pdi)
	{
		if (vpd[pdi] & PTE_P & PTE_U)
		{
			for (int pti = 0; pti < NPTENTRIES; ++pti)
			{
				int pn = pdi * NPDENTRIES + pti;
				if (pn == PPN(UXSTACKTOP - PGSIZE))
					continue;

				if (vpt[pti] & PTE_P & PTE_U)
				{
					CHECK_FAIL_FORK(duppage(envid, pn));
				}
			}
		}
	}

	CHECK_FAIL_FORK(sys_env_set_status(envid, ENV_RUNNABLE));
	return envid;
}

// Challenge!
int
sfork(void)
{
	panic("sfork not implemented");
	return -E_INVAL;
}
