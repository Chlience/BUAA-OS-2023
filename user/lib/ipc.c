// User-level IPC library routines

#include <env.h>
#include <lib.h>
#include <mmu.h>

// Send val to whom.  This function keeps trying until
// it succeeds.  It should panic() on any error other than
// -E_IPC_NOT_RECV.
//
// Hint: use syscall_yield() to be CPU-friendly.
void ipc_send(u_int whom, u_int val, const void *srcva, u_int perm) {
	int r;
	while ((r = syscall_ipc_try_send(whom, val, srcva, perm)) == -E_IPC_NOT_RECV) {
		syscall_yield();
	}
	user_assert(r == 0);
}

// Receive a value.  Return the value and store the caller's envid
// in *whom.
//
// Hint: use env to discover the value and who sent it.
u_int ipc_recv(u_int *whom, void *dstva, u_int *perm) {
	int r = syscall_ipc_recv(dstva);
	if (r != 0) {
		user_panic("syscall_ipc_recv err: %d", r);
	}

	if (whom) {
		*whom = env->env_ipc_from;
	}

	if (perm) {
		*perm = env->env_ipc_perm;
	}

	return env->env_ipc_value;
}

u_int get_time(u_int *us) {
	int temp = 0;
	panic_on(msyscall(SYS_write_dev, &temp, 0x15000000, 4));
	u_int total = 0;
	panic_on(msyscall(SYS_read_dev, &total, 0x15000010, 4));
	panic_on(msyscall(SYS_read_dev, us, 0x15000020, 4))
	return total;
}

void usleep(u_int us) {
	u_int time_us;
	u_int time_s = get_time(&time_us);
	u_int new_us;
	u_int new_s;
	u_long gap_us;
	while (1) {
		new_s = get_time(&new_us);
		gap_us = (new_s - time_s) * 1000000 + new_us - time_us;
		if (gap_us >= us) {
			return;
		} else {
			yield();
		}
	}
}
