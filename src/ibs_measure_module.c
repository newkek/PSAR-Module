#include "ibs_measure_module.h"

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/kallsyms.h>
#include <linux/cpumask.h>
#include <asm/nmi.h>
#include <asm/apic.h>
#include <linux/cpu.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kévin Gallardo - Pierre-Yves Péneau");
MODULE_DESCRIPTION("TODO");

static struct hrtimer hr_timer;
static struct task_struct* thread1;
static struct cpumask dst;

static ktime_t kt_period;

int keep_running;

enum hrtimer_restart
my_hrtimer_callback(struct hrtimer *timer)
{
  struct task_struct* current_thread_mine;
  struct thread_info* ti;

  printk("Callback Timer \n");
  current_thread_mine =  get_current();
  printk("current cpu p : %ld\n", current->stime);

  ti = current_thread_info();
  printk("current cpu : %d\n", ti->cpu);
 
  hrtimer_forward_now(& hr_timer, kt_period);

  return HRTIMER_RESTART;
}


static int
ibs_cpu_notifier(struct notifier_block* b, unsigned long action, void* data)
{
  int cpu;

  cpu = (unsigned long)data;
  printk("ibs_cpu_notifier\n");
	
  /*switch (action) {
    case CPU_DOWN_FAILED:
    case CPU_ONLINE:
    smp_call_function_single(cpu, set_ibs_rate, NULL, 0);
    break;
    case CPU_DOWN_PREPARE:
    smp_call_function_single(cpu, ibs_stop, NULL, 1);
    break;
    }*/
  return NOTIFY_DONE;
}

static struct notifier_block ibs_cpu_nb = { .notifier_call = ibs_cpu_notifier };

static inline void
apic_init_ibs_nmi_per_cpu (void* args)
{
  unsigned long reg;
  unsigned int  v;

  reg = (APIC_EILVT_LVTOFF_IBS << 4) + APIC_EILVT0;
  v = (0 << 16) | (APIC_EILVT_MSG_NMI << 8) | 0;
  apic_write(reg, v);
}

void
set_ibs_rate(void *args)
{
  unsigned int low;
  unsigned int high;
  uint32_t rand;

  rand = high = 0;

  low = (((0x1FFF0 + rand) >> 4) & 0xFFFF) \
    + ((1 & 0x1) << 19) /* bit 19 */
    + IBS_OP_LOW_ENABLE;

  wrmsr(MSR_AMD64_IBSOPCTL, low, high);
}


static void
ibs_stop(void)
{
  unsigned int low;
  unsigned int high;

  low = high = 0;
  /* clear max count and enable */
  wrmsr(MSR_AMD64_IBSOPCTL, low, high);
}


static int
handle_ibs_nmi(struct pt_regs* const regs)
{
  int cpu;
  unsigned int low;
  unsigned int high;
  /* struct ibs_op_sample ibs_op_stack; */
  /* struct ibs_op_sample *ibs_op = &ibs_op_stack; */

/* #if DUMP_OVERHEAD */
  uint64_t time_start,time_stop;

  cpu = smp_processor_id();
  rdtscll(time_start);
/* #endif */

  /* per_cpu(stats, cpu).total_interrupts++; */
  rdmsr(MSR_AMD64_IBSOPCTL, low, high);

  if (low & IBS_OP_LOW_VALID_BIT)
    {
      rdmsr(MSR_AMD64_IBSOPDATA2, low, high);
      /* ibs_op->ibs_op_data2_low = low; */
      /* ibs_op->ibs_op_data2_high = high; */

      /* If the sample does not touch DRAM, stop. */
      /* if((ibs_op->ibs_op_data2_low & 7) != 3) { */
      /* 	      goto end; */
      /* } */
      /* if((!consider_L1L2) && ((ibs_op->ibs_op_data2_low & 7) == 0)) { */
      /* 	     goto end; */
      /* } */
      /* if((ibs_op->ibs_op_data2_low & 7) == 3)  */
      /* 	     per_cpu(stats, cpu).total_samples_L3DRAM++; */
      printk("MSR_AMD64_IBSOPDATA2 %d %d", low, high);
      rdmsr(MSR_AMD64_IBSOPRIP, low, high);
      /* ibs_op->ibs_op_rip_low = low; */
      /* ibs_op->ibs_op_rip_high = high; */
      printk("MSR_AMD64_IBSOPRIP %d %d", low, high);
      rdmsr(MSR_AMD64_IBSOPDATA, low, high);
      /* ibs_op->ibs_op_data1_low = low; */
      /* ibs_op->ibs_op_data1_high = high; */
      printk("MSR_AMD64_IBSOPDATA %d %d", low, high);
      rdmsr(MSR_AMD64_IBSOPDATA3, low, high);
      /* ibs_op->ibs_op_data3_low = low; */
      /* ibs_op->ibs_op_data3_high = high; */
      printk("MSR_AMD64_IBSOPDATA3 %d %d", low, high);
      rdmsr(MSR_AMD64_IBSDCLINAD, low, high);
      /* ibs_op->ibs_dc_linear_low = low; */
      /* ibs_op->ibs_dc_linear_high = high; */
      printk("MSR_AMD64_IBSOPCLINAD %d %d", low, high);
      rdmsr(MSR_AMD64_IBSDCPHYSAD, low, high);
      /* ibs_op->ibs_dc_phys_low = low; */
      /* ibs_op->ibs_dc_phys_high = high; */
      
      /* if(ibs_op->phys_addr == 0) */
      /*      goto end; */

      /* if(ibs_op->lin_addr < min_lin_address || ibs_op->lin_addr > max_lin_address) { */
      /* goto end; */
    }

  /* per_cpu(stats, cpu).total_samples++; */
  /* rbtree_add_sample(!user_mode(regs), ibs_op, smp_processor_id(), current->pid, current->tgid); */
  /* end: __attribute__((unused)); */

  printk("MSR_AMD64_IBSDCPHYSAD %d %d", low, high);
  rdmsr(MSR_AMD64_IBSOPCTL, low, high);
  printk("MSR_AMD64_IBSOPCTL %d %d", low, high);
  high = 0;
  low &= ~IBS_OP_LOW_VALID_BIT;
  low |= IBS_OP_LOW_ENABLE;
/* #if DUMP_OVERHEAD */
  rdtscll(time_stop);
  /* per_cpu(stats, cpu).time_spent_in_NMI += time_stop - time_start; */
/* #endif */
  wrmsr(MSR_AMD64_IBSOPCTL, low, high);

  /* exit: __attribute__((unused)); */
  return 1;
}

int
thread_fn(void * args)
{
  u64 i;
  struct thread_info *ti;
  struct task_struct *tak __attribute__((unused));

  i = 0;
  cpumask_clear(&dst);
  cpumask_set_cpu(24, &dst);
  ti = current_thread_info();
  /* sched_setaffinity(0, &dst); */
  cpuset_cpus_allowed(current, &dst);

  /* while ( i < 5 ) */
  /*   { */
  /*     printk("In thread1, cpu : %d\n", ti->cpu); */
  /*     printk("nb online cpus : %d\n", num_online_cpus()); */
  /*     i++; */
  /*     schedule(); */
  /*   } */
	
  /* while (keep_running) */
  /*   { */
  /*     printk("round %d : running tasks : "); */
  /*     for_each_process(tak) */
  /*     { */
  /* 	if(tak->state == TASK_RUNNING) */
  /* 	  printk("%d - ", tak-> pid);	 */
  /*     } */
  /*     printk("\n"); */
  /*     i++; */
  /*     schedule(); */

  /*   } */

  /* per CPU INIT */
  on_each_cpu(apic_init_ibs_nmi_per_cpu, NULL, 1);
  apic_write(APIC_LVTPC, APIC_DM_NMI);
  register_cpu_notifier(&ibs_cpu_nb);
  register_nmi_handler(NMI_LOCAL, handle_ibs_nmi, 0, "psar");
  on_each_cpu(set_ibs_rate, NULL, 1);

  return 0;
}


static int
__init ibs_measure_monitor_init( void )
{
  /* ktime_t ktime; */
  unsigned long delay_in_ms;
  unsigned long sym_addr;
  char* sym_name = "cpuset_cpus_allowed";
  char name[8] = "thread1";

  delay_in_ms = 1000L;
  sym_addr = kallsyms_lookup_name(sym_name);

  printk(KERN_INFO "[%s] %s (0x%lx)\n", __this_module.name, sym_name, sym_addr);
  cpuset_cpus_allowed = sym_addr;

  if ( !cpuset_cpus_allowed )
    return -1;

  printk("HR Timer module installing\n");
  kt_period = ktime_set( 0, MS_TO_NS(delay_in_ms) );
  hrtimer_init( &hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL );
  hr_timer.function = &my_hrtimer_callback;

  thread1 = kthread_create(thread_fn, NULL, name);

  if ( thread1 )
    {
      printk("In if\n");
      wake_up_process(thread1);
    }

  keep_running = 1;

  /* printk( "Starting timer to fire in %ldms (%ld)\n", delay_in_ms, jiffies ); */
  /* hrtimer_start( &hr_timer, kt_period, HRTIMER_MODE_REL ); */

  return 0;
}

static void
__exit ibs_measure_monitor_cleanup( void )
{
  int ret;

  ret = hrtimer_cancel( &hr_timer );
  keep_running = 0;

  if ( ret )
    printk("The timer was still in use...\n");

  ibs_stop();
  printk("HR Timer module uninstalling\n");

  return;
}

module_init(ibs_measure_monitor_init);
module_exit(ibs_measure_monitor_cleanup);
