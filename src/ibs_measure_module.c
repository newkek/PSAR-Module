#include "ibs_measure_module.h"
#include "ibs_managing.h"

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cpu.h>
#include <asm/nmi.h>
#include <asm/apic.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kévin Gallardo - Pierre-Yves Péneau");
MODULE_DESCRIPTION("TODO");

int
thread_fn(void* args)
{
  printk(KERN_INFO "thread_fn() test\n");
  return 0;
}


static int
__init ibs_measure_monitor_init(void)
{
  /* printk for information */
  printk(KERN_INFO "START [%s]\n", __this_module.name);

  /* Start ibs sampling */
  ibs_start();

  return 0;
}

static void
__exit ibs_measure_monitor_cleanup(void)
{
  /* printk for information */
  printk(KERN_INFO "STOP [%s]\n", __this_module.name);

  /* stop ibs sampling */
  ibs_stop();

  return;
}

module_init(ibs_measure_monitor_init);
module_exit(ibs_measure_monitor_cleanup);
