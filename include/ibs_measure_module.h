#ifndef _H_IBS_MEASURE_MODULE_
#define _H_IBS_MEASURE_MODULE_

#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/cpumask.h>
#include <linux/module.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/kallsyms.h>
#include <asm/nmi.h>
#include <asm/apic.h>
#include <linux/cpu.h>
#include <linux/freezer.h>

#include "ibs_samples_management.h"

#define MS_TO_NS(x)	(x * 1E6L)

#define IBS_OP_LOW_VALID_BIT           (1ULL<<18)      /* bit 18 */
#define IBS_OP_LOW_ENABLE              (1ULL<<17)      /* bit 17 */
#define APIC_EILVT_LVTOFF_IBS          1
#define APIC_EILVT0                    0x500

unsigned int measures_rate;

static struct hrtimer hr_timer;


static struct task_struct* thread1, *thread2;
static struct completion on_exit;

static struct cpumask dst;


static ktime_t kt_period;


static char* options = "init";

ibs_sample_list* samples;



void (*cpuset_cpus_allowed)(struct task_struct* p, struct cpumask* mask);





#endif /* _H_IBS_MEASURE_MODULE_ */

