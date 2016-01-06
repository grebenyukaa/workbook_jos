#include <inc/assert.h>

#include <kern/env.h>
#include <kern/pmap.h>
#include <kern/monitor.h>

static envid_t last_env_id = 1;

// Choose a user environment to run and run it.
void
sched_yield(void)
{
	// Implement simple round-robin scheduling.
	// Search through 'envs' for a runnable environment,
	// in circular fashion starting after the previously running env,
	// and switch to the first such environment found.
	// It's OK to choose the previously running env if no other env
	// is runnable.
	// But never choose envs[0], the idle environment,
	// unless NOTHING else is runnable.

	for (int i = 0; i < 2; ++i)
	{
		envid_t from, to;
		if (!i)
		{
			from = last_env_id;
			to = NENV;
		}
		else
		{
			from = 1;
			to = last_env_id;
		}

		for (envid_t env_id = from; env_id < to; ++env_id)
		{
			if (envs[env_id].env_status == ENV_RUNNABLE)
			{
				last_env_id = env_id + 1;
				if (last_env_id >= NENV)
					last_env_id = 1;
				env_run(&envs[env_id]);
			}
		}
	}

	// Run the special idle environment when nothing else is runnable.
	if (envs[0].env_status == ENV_RUNNABLE)
		env_run(&envs[0]);
	else {
		cprintf("Destroyed all environments - nothing more to do!\n");
		while (1)
			monitor(NULL);
	}
}
