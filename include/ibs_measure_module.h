#ifndef _H_IBS_MEASURE_MODULE_
#define _H_IBS_MEASURE_MODULE_

#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/cpumask.h>

#define MS_TO_NS(x)	(x * 1E6L)

#define IBS_OP_LOW_VALID_BIT           (1ULL<<18)      /* bit 18 */
#define IBS_OP_LOW_ENABLE              (1ULL<<17)      /* bit 17 */
#define APIC_EILVT_LVTOFF_IBS          1
#define APIC_EILVT0                    0x500

void (*cpuset_cpus_allowed)(struct task_struct* p, struct cpumask* mask);

#endif /* _H_IBS_MEASURE_MODULE_ */

