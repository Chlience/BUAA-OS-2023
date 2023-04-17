#include <env.h>
#include <pmap.h>
#include <printk.h>

/* Overview:
 *   Implement a round-robin scheduling to select a runnable env and schedule it using 'env_run'.
 *
 * Post-Condition:
 *   If 'yield' is set (non-zero), 'curenv' should not be scheduled again unless it is the only
 *   runnable env.
 *
 * Hints:
 *   1. The variable 'count' used for counting slices should be defined as 'static'.
 *   2. Use variable 'env_sched_list', which contains and only contains all runnable envs.
 *   3. You shouldn't use any 'return' statement because this function is 'noreturn'.
 */
extern struct Env_sched_list env_sched_list;
void schedule(int yield) {
	/* 这个 static 的作用需要注意 */
	static int count = 0; // remaining time slices of current env
	struct Env *e = curenv;
	
	static int user_time[5];
	static int user_able[5];

	struct Env *ee = NULL;
	static int user = - 1;
	for (int i = 0; i <= 4; ++ i) {
		user_able[i] = 0;
	}
	TAILQ_FOREACH(ee, &env_sched_list, env_sched_link) {
		if (ee != NULL) {
			user_able[ee->env_user] = 1;
		}
	}

	count = count - 1;
	if (yield || (count == 0) || (e == NULL) || (e->env_status != ENV_RUNNABLE)) {
		if ((e != NULL) && (e->env_status == ENV_RUNNABLE)) {
			TAILQ_REMOVE(&env_sched_list, e, env_sched_link);
			TAILQ_INSERT_TAIL(&env_sched_list, e, env_sched_link);
			user_time[e->env_user] += e->env_pri;
		}
		if(TAILQ_EMPTY(&env_sched_list)) {
			panic("schedule: no runnable envs");
		}
		int min_user = -1;
		for (int i = 0; i <= 4; ++ i) {
			if (user_able[i] && (min_user == - 1 || user_time[i] < user_time[min_user])) {
				min_user = i;
			}
		}

		TAILQ_FOREACH(ee, &env_sched_list, env_sched_link) {
			if (ee != NULL && ee->env_user == min_user) {
				e = ee;
				break;
			}
		}
		
		// e = TAILQ_FIRST(&env_sched_list);
		count = e->env_pri;
		env_run(e);
	} else {
		env_run(e);
	}
}
