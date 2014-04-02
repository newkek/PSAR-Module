#include "ibs_measure_module.h"

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/kallsyms.h>
#include <linux/cpumask.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kévin Gallardo - Pierre-Yves Péneau");
MODULE_DESCRIPTION("TODO");

#define MS_TO_NS(x)	(x * 1E6L)

static struct hrtimer hr_timer;
static ktime_t kt_period;


static struct task_struct* thread1;
static struct cpumask dst;


void (*cpuset_cpus_allowed)(struct task_struct *p, struct cpumask *mask);


enum hrtimer_restart my_hrtimer_callback( struct hrtimer *timer )
{
  printk("Callback Timer \n");

  struct task_struct* current_thread_mine;
  current_thread_mine =  get_current();

  printk("current cpu p : %ld\n", current->stime);

  struct thread_info* ti;
  ti = current_thread_info();

  printk("current cpu : %d\n", ti->cpu);
 
  hrtimer_forward_now(& hr_timer, kt_period);

  return HRTIMER_RESTART;
}
int keep_running;

int thread_fn(void * args){
	u64 i=0;
	cpumask_clear(&dst);
	cpumask_set_cpu(24, &dst);
	struct thread_info* ti;
	struct task_struct *tak;
	ti = current_thread_info();
	//sched_setaffinity(0, &dst);
	cpuset_cpus_allowed(current, &dst);
	/*while(i<5){
		printk("In thread1, cpu : %d\n", ti->cpu);
		printk("nb online cpus : %d\n", num_online_cpus());
		i++;
		schedule();
	}*/
	
	while (keep_running){
		printk("round %d : running tasks : ");
		for_each_process(tak){
			if(tak->state == TASK_RUNNING)
				printk("%d - ", tak-> pid);	
		}
		printk("\n");
		i++;
		schedule();

	}
	return 0;
}


static int __init ibs_measure_monitor_init( void )
{
//  ktime_t ktime;
  unsigned long delay_in_ms = 1000L;

  char name[8]="thread1";

  char *sym_name = "cpuset_cpus_allowed";


  unsigned long sym_addr = kallsyms_lookup_name(sym_name);
  printk(KERN_INFO "[%s] %s (0x%lx)\n", __this_module.name, sym_name, sym_addr);
  cpuset_cpus_allowed = sym_addr;
  if (!cpuset_cpus_allowed)
	return -1;




  printk("HR Timer module installing\n");


  kt_period = ktime_set( 0, MS_TO_NS(delay_in_ms) );

  hrtimer_init( &hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL );
  
  hr_timer.function = &my_hrtimer_callback;

  thread1 = kthread_create(thread_fn, NULL, name);

  if((thread1)){
	printk("In if\n");
	wake_up_process(thread1);
  }

  keep_running=1;

  //printk( "Starting timer to fire in %ldms (%ld)\n", delay_in_ms, jiffies );

  //hrtimer_start( &hr_timer, kt_period, HRTIMER_MODE_REL );

  return 0;
}

static void __exit ibs_measure_monitor_cleanup( void )
{
  int ret;
  keep_running=0;
  ret = hrtimer_cancel( &hr_timer );

  if (ret) printk("The timer was still in use...\n");

  printk("HR Timer module uninstalling\n");

  return;
}

module_init(ibs_measure_monitor_init);
module_exit(ibs_measure_monitor_cleanup);
