/* SPDX-License-Identifier: GPL-2.0 */
#include <linux/module.h>

#include <linux/sched.h>
#include <trace/events/sched.h>

#define CREATE_TRACE_POINTS
#include "sched_events.h"

static inline struct cfs_rq *get_group_cfs_rq(struct sched_entity *se)
{
#ifdef CONFIG_FAIR_GROUP_SCHED
	return se->my_q;
#else
	return NULL;
#endif
}

static inline struct cfs_rq *get_se_cfs_rq(struct sched_entity *se)
{
#ifdef CONFIG_FAIR_GROUP_SCHED
	return se->cfs_rq;
#else
	return NULL;
#endif
}

static void sched_pelt_cfs(void *data, struct cfs_rq *cfs_rq)
{
	if (trace_sched_pelt_cfs_enabled()) {
		const struct sched_avg *avg;
		char path[PATH_SIZE];
		int cpu;

		avg = sched_trace_cfs_rq_avg(cfs_rq);
		sched_trace_cfs_rq_path(cfs_rq, path, PATH_SIZE);
		cpu = sched_trace_cfs_rq_cpu(cfs_rq);

		trace_sched_pelt_cfs(cpu, path, avg);
	}
}

static void sched_pelt_rt(void *data, struct rq *rq)
{
	if (trace_sched_pelt_rt_enabled()) {
		const struct sched_avg *avg = sched_trace_rq_avg_rt(rq);
		int cpu = sched_trace_rq_cpu(rq);

		if (!avg)
			return;

		trace_sched_pelt_rt(cpu, avg);
	}
}

static void sched_pelt_dl(void *data, struct rq *rq)
{
	if (trace_sched_pelt_dl_enabled()) {
		const struct sched_avg *avg = sched_trace_rq_avg_dl(rq);
		int cpu = sched_trace_rq_cpu(rq);

		if (!avg)
			return;

		trace_sched_pelt_dl(cpu, avg);
	}
}

static void sched_pelt_irq(void *data, struct rq *rq)
{
	if (trace_sched_pelt_irq_enabled()){
		const struct sched_avg *avg = sched_trace_rq_avg_irq(rq);
		int cpu = sched_trace_rq_cpu(rq);

		if (!avg)
			return;

		trace_sched_pelt_irq(cpu, avg);
	}
}

static void sched_pelt_se(void *data, struct sched_entity *se)
{
	if (trace_sched_pelt_se_enabled()) {
		void *gcfs_rq = get_group_cfs_rq(se);
		void *cfs_rq = get_se_cfs_rq(se);
		struct task_struct *p;
		char path[PATH_SIZE];
		char *comm;
		pid_t pid;
		int cpu;

		sched_trace_cfs_rq_path(gcfs_rq, path, PATH_SIZE);
		cpu = sched_trace_cfs_rq_cpu(cfs_rq);

		p = gcfs_rq ? NULL : container_of(se, struct task_struct, se);
		comm = p ? p->comm : "(null)";
		pid = p ? p->pid : -1;

		trace_sched_pelt_se(cpu, path, comm, pid, &se->avg);
	}
}

static void sched_overutilized(void *data, struct root_domain *rd, bool overutilized)
{
	if (trace_sched_overutilized_enabled()) {
		char span[SPAN_SIZE];

		cpumap_print_to_pagebuf(false, span, sched_trace_rd_span(rd));

		trace_sched_overutilized(overutilized, span);
	}
}

static void sched_update_nr_running(void *data, struct rq *rq, int change)
{
          if (trace_sched_update_nr_running_enabled()) {
		  int cpu = sched_trace_rq_cpu(rq);
		  int nr_running = sched_trace_rq_nr_running(rq);

	          trace_sched_update_nr_running(cpu, change, nr_running);
	  }
}

static int sched_tp_init(void)
{
	register_trace_pelt_cfs_tp(sched_pelt_cfs, NULL);
	register_trace_pelt_rt_tp(sched_pelt_rt, NULL);
	register_trace_pelt_dl_tp(sched_pelt_dl, NULL);
	register_trace_pelt_irq_tp(sched_pelt_irq, NULL);
	register_trace_pelt_se_tp(sched_pelt_se, NULL);
	register_trace_sched_overutilized_tp(sched_overutilized, NULL);
	register_trace_sched_update_nr_running_tp(sched_update_nr_running, NULL);

	return 0;
}

static void sched_tp_finish(void)
{
	unregister_trace_pelt_cfs_tp(sched_pelt_cfs, NULL);
	unregister_trace_pelt_rt_tp(sched_pelt_rt, NULL);
	unregister_trace_pelt_dl_tp(sched_pelt_dl, NULL);
	unregister_trace_pelt_irq_tp(sched_pelt_irq, NULL);
	unregister_trace_pelt_se_tp(sched_pelt_se, NULL);
	unregister_trace_sched_overutilized_tp(sched_overutilized, NULL);
	unregister_trace_sched_update_nr_running_tp(sched_update_nr_running, NULL);
}


module_init(sched_tp_init);
module_exit(sched_tp_finish);

MODULE_LICENSE("GPL");
