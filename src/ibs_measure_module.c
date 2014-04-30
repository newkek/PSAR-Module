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
#include <linux/freezer.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kévin Gallardo - Pierre-Yves Péneau");
MODULE_DESCRIPTION("TODO");

static struct hrtimer hr_timer;
static struct task_struct* thread1, *thread2;
static struct cpumask dst;

static ktime_t kt_period;

int keep_running;
int used_cpu;
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
  return NOTIFY_DONE;
}


static struct notifier_block ibs_cpu_nb = { .notifier_call = ibs_cpu_notifier };

static inline void
apic_init_ibs_nmi_per_cpu (void* args)
{
  unsigned long reg;
  unsigned int  v;

  apic_write(APIC_LVTPC, APIC_DM_NMI);

  reg = (APIC_EILVT_LVTOFF_IBS << 4) + APIC_EILVT0;
  v = (0 << 16) | (APIC_EILVT_MSG_NMI << 8) | 0;
  apic_write(reg, v);
}

static inline void
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


static inline void
ibs_stop(void* args)
{
  unsigned int low;
  unsigned int high;

  low = high = 0;
  /* clear max count and enable */
  wrmsr(MSR_AMD64_IBSOPCTL, low, high);
  return;
}


static int
handle_ibs_nmi(unsigned int cmd, struct pt_regs* const regs)
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
      printk("MSR_AMD64_IBSOPDATA2 %d %d\n", low, high);
      rdmsr(MSR_AMD64_IBSOPRIP, low, high);
      /* ibs_op->ibs_op_rip_low = low; */
      /* ibs_op->ibs_op_rip_high = high; */
      printk("MSR_AMD64_IBSOPRIP %d %d\n", low, high);
      rdmsr(MSR_AMD64_IBSOPDATA, low, high);
      /* ibs_op->ibs_op_data1_low = low; */
      /* ibs_op->ibs_op_data1_high = high; */
      printk("MSR_AMD64_IBSOPDATA %d %d\n", low, high);
      rdmsr(MSR_AMD64_IBSOPDATA3, low, high);
      /* ibs_op->ibs_op_data3_low = low; */
      /* ibs_op->ibs_op_data3_high = high; */
      printk("MSR_AMD64_IBSOPDATA3 %d %d\n", low, high);
      rdmsr(MSR_AMD64_IBSDCLINAD, low, high);
      /* ibs_op->ibs_dc_linear_low = low; */
      /* ibs_op->ibs_dc_linear_high = high; */
      printk("MSR_AMD64_IBSOPCLINAD %d %d\n", low, high);
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

  printk("MSR_AMD64_IBSDCPHYSAD %d %d\n", low, high);
  rdmsr(MSR_AMD64_IBSOPCTL, low, high);
  printk("MSR_AMD64_IBSOPCTL %d %d\n", low, high);
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


static int
thread_fnn(void* args)
{
  printk("Init sampling...\n");

  /* Set securities for the thread */
  set_freezable();
  allow_signal(SIGKILL);
  set_current_state(TASK_INTERRUPTIBLE);
  schedule_timeout(2);

  /* Experimental lines */
  used_cpu = smp_processor_id();
  printk("Sampling is start on cpu %d\n", used_cpu);

  /* on_each_cpu(apic_init_ibs_nmi_per_cpu, NULL, 1); */
  apic_init_ibs_nmi_per_cpu(NULL);
  register_cpu_notifier(&ibs_cpu_nb);
  register_nmi_handler(NMI_LOCAL, handle_ibs_nmi, 0, "psar");
  /* start sampling */
  set_ibs_rate(NULL);

  printk("Init sampling: ok\n");
  do_exit(0);

  return 0;
}



static int
thread_fn2(void *args)
{
  printk("Stop sampling...\n");

  /* Set securities for the thread */
  set_freezable();
  allow_signal(SIGKILL);
  set_current_state(TASK_INTERRUPTIBLE);
  schedule_timeout(2);

  /* Write on MSR only on used cpu */
  /* Experimental */
  printk("Trying to stop sampling on cpu %d\n", used_cpu);
  smp_call_function_single(used_cpu, ibs_stop, NULL, 1);
  
  printk("Sampling stopped: ok\n");

  do_exit(0);
  return 0;
}


static int
__init ibs_measure_monitor_init( void )
{
  int error;
  unsigned long delay_in_ms;
  unsigned long sym_addr;
  char* sym_name = "cpuset_cpus_allowed";

  delay_in_ms = 1000L;
  sym_addr = kallsyms_lookup_name(sym_name);
	
  printk(KERN_INFO "[%s] %s (0x%lx)\n", __this_module.name, sym_name, sym_addr);
	
  thread1 = kthread_run(thread_fnn, NULL, "thread_");
  if(IS_ERR(thread1))
    {
      error = PTR_ERR(thread1);
      return error;
    }

  printk("Module init ok\n");
  return 0;
}

static void
__exit ibs_measure_monitor_cleanup( void )
{
  int error;

  thread2 = kthread_run(thread_fn2, NULL, "thread_");
  if(IS_ERR(thread2))
    {
      error = PTR_ERR(thread2);
      return;
    }
  return;
}

module_init(ibs_measure_monitor_init);
module_exit(ibs_measure_monitor_cleanup);
