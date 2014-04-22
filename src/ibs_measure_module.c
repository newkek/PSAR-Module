#include "ibs_measure_module.h"
#include "ibs_managing.h"

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cpu.h>
#include <asm/nmi.h>
#include <asm/apic.h>
#include <linux/kthread.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kévin Gallardo - Pierre-Yves Péneau");
MODULE_DESCRIPTION("TODO");

static struct task_struct *kt;

int thread_fn_start(void* args)
{
  /* Start ibs sampling */
  ibs_start();

  printk(KERN_INFO "thread_fn() : measures launched\n");
  return 0;
}



int thread_fn_stop(void* args){
	/* stop measures */
	ibs_stop();
 	/* printk for information */
	printk(KERN_INFO "thread_fn() : measures stopped");
	return 0;
}
          
static int __init ibs_measure_monitor_init(void)
{
  /* printk for information */
  printk(KERN_INFO "START [%s]\n", __this_module.name);

  /* launching the kernel thread which engage monitoring */
  kt = kthread_create(thread_fn_start, NULL, "kt");
  if (kt){
	printk("thread successfully started");
	wake_up_process(kt);
  }

  return 0;
}

static void __exit ibs_measure_monitor_cleanup(void)
{
  /* printk for information */
  printk(KERN_INFO "STOP [%s]\n", __this_module.name);

  kt = kthread_create(thread_fn_stop, NULL, "kt");
  if (kt){
	printk("thread successfully started");
	wake_up_process(kt);
  }

  return;
}

module_init(ibs_measure_monitor_init);
module_exit(ibs_measure_monitor_cleanup);
