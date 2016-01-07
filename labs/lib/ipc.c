// User-level IPC library routines

#include <inc/lib.h>

// Receive a value via IPC and return it.
// If 'pg' is nonnull, then any page sent by the sender will be mapped at
//	that address.
// If 'from_env_store' is nonnull, then store the IPC sender's envid in
//	*from_env_store.
// If 'perm_store' is nonnull, then store the IPC sender's page permission
//	in *perm_store (this is nonzero iff a page was successfully
//	transferred to 'pg').
// If the system call fails, then store 0 in *fromenv and *perm (if
//	they're nonnull) and return the error.
//
// Hint:
//   Use 'env' to discover the value and who sent it.
//   If 'pg' is null, pass sys_ipc_recv a value that it will understand
//   as meaning "no page".  (Zero is not the right value.)
int32_t
ipc_recv(envid_t *from_env_store, void *pg, int *perm_store)
{
	int errno = sys_ipc_recv(pg ? pg : (void*)UTOP);
    if (errno)
    {
        *from_env_store = 0;
        *perm_store = 0;
        cprintf("[ipc_recv] fail %e", errno);
        return errno;
    }

    if (from_env_store)
        *from_env_store = env->env_ipc_from;
    if (perm_store)
        *perm_store = env->env_ipc_perm;

	return env->env_ipc_value;
}

// Send 'val' (and 'pg' with 'perm', if 'pg' is nonnull) to 'toenv'.
// This function keeps trying until it succeeds.
// It should panic() on any error other than -E_IPC_NOT_RECV.
//
// Hint:
//   Use sys_yield() to be CPU-friendly.
//   If 'pg' is null, pass sys_ipc_recv a value that it will understand
//   as meaning "no page".  (Zero is not the right value.)
void
ipc_send(envid_t to_env, uint32_t val, void *pg, int perm)
{
    int errno;
    while((errno = sys_ipc_try_send(to_env, val, pg ? pg : (void*)UTOP, perm)) == -E_IPC_NOT_RECV)
        sys_yield();

    if (errno)
        panic("[ipc_send] fail: %e", errno);
}
