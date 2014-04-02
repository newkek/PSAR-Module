#ifndef _H_IBS_MEASURE_MODULE_
#define _H_IBS_MEASURE_MODULE_

#define MS_TO_NS(x)	(x * 1E6L)

void (*cpuset_cpus_allowed)(struct task_struct *p, struct cpumask *mask);

module_init (ibs_measure_monitor_init);
module_exit (ibs_measure_monitor_cleanup);

#endif /* _H_IBS_MEASURE_MODULE_ */

