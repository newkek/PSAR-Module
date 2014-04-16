#include "ibs_measure_module.h"

#include <linux/kernel.h>
#include <linux/module.h>
#include <asm/nmi.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kévin Gallardo - Pierre-Yves Péneau");
MODULE_DESCRIPTION("TODO");

/*
 * This function initialize IBS by writing values on MSR registers 
 * if ibs_method equals 0: cycles-based sampling
 * if ibs_method equals 1: dispatched op-based sampling
 */
void
set_ibs_rate(int sampling_rate, int ibs_method)
{
  unsigned int low;
  unsigned int high;
  uint32_t rand;

  rand = high = 0;

  /* TODO: find how to calculate low */
  low = (((sampling_rate + rand) >> 4) & 0xFFFF) \
    + ((ibs_method & 0x1) << 19)                  /* bit 19 */
    + IBS_OP_LOW_ENABLE;

  /* Write on MSR */
  wrmsr(MSR_AMD64_IBSOPCTL, low, high);
}


/* This function stop IBS sampling by writing on MSR registers */
void
ibs_stop(void)
{
  unsigned int low;
  unsigned int high;

  low = high = 0;
  /* Write on MSR */
  wrmsr(MSR_AMD64_IBSOPCTL, low, high);
}

/* This function will be called on every interruption */
static int
handle_ibs_nmi(struct pt_regs* const regs)
/* struct pt_reg us used with rb_tree */
/* handle_ibs_nmi(struct pt_regs* const regs) */
{
  int cpu;
  int consider_L1L2;
  unsigned int low;
  unsigned int high;

  /* Structure qui va contenir tout ce qui aura été mis dans les MSR */
  /* TODO: trouver la doc sur la récup des données */
  struct ibs_op_sample  ibs_op_stack;
  struct ibs_op_sample* ibs_op;

  consider_L1L2 = 0;

  /* On récupère le numéro du CPU */
  cpu = smp_processor_id();
  ibs_op = &ibs_op_stack;
  
  /* per_cpu(stats, cpu).total_interrupts++; */

  rdmsr(MSR_AMD64_IBSOPCTL, low, high);

  if (low & IBS_OP_LOW_VALID_BIT)
    {
      rdmsr(MSR_AMD64_IBSOPDATA2, low, high);
      ibs_op->ibs_op_data2_low = low;
      ibs_op->ibs_op_data2_high = high;
      printk("MSR_AMD64_IBSOPDATA2 %d %d", low, high);

      if ( (!consider_L1L2) && ((ibs_op->ibs_op_data2_low & 7) == 0) )
	goto end;

      if ( (ibs_op->ibs_op_data2_low & 7) == 3)
	/* per_cpu(stats, cpu).total_samples_L3DRAM++; */
	;
      
      rdmsr(MSR_AMD64_IBSOPRIP, low, high);
      ibs_op->ibs_op_rip_low = low;
      ibs_op->ibs_op_rip_high = high;
      printk("MSR_AMD64_IBSOPRIP %d %d", low, high);

      rdmsr(MSR_AMD64_IBSOPDATA, low, high);
      ibs_op->ibs_op_data1_low = low;
      ibs_op->ibs_op_data1_high = high;
      printk("MSR_AMD64_IBSOPDATA %d %d", low, high);

      rdmsr(MSR_AMD64_IBSOPDATA3, low, high);
      ibs_op->ibs_op_data3_low = low;
      ibs_op->ibs_op_data3_high = high;
      printk("MSR_AMD64_IBSOPDATA3 %d %d", low, high);

      rdmsr(MSR_AMD64_IBSDCLINAD, low, high);
      ibs_op->ibs_dc_linear_low = low;
      ibs_op->ibs_dc_linear_high = high;
      printk("MSR_AMD64_IBSOPCLINAD %d %d", low, high);

      rdmsr(MSR_AMD64_IBSDCPHYSAD, low, high);
      ibs_op->ibs_dc_phys_low = low;
      ibs_op->ibs_dc_phys_high = high;
      printk("MSR_AMD64_IBSDCPHYSAD %d %d", low, high);      
      
      if(ibs_op->phys_addr == 0)
           goto end;

      /* if(ibs_op->lin_addr < min_lin_address || ibs_op->lin_addr > max_lin_address) { */
	/* goto end; */
    }

  /* per_cpu(stats, cpu).total_samples++; */

  rdmsr(MSR_AMD64_IBSOPCTL, low, high);
  printk("MSR_AMD64_IBSOPCTL %d %d", low, high);

 end:  
  /* Reset low & high */
  high = 0;
  low &= ~IBS_OP_LOW_VALID_BIT;
  low |= IBS_OP_LOW_ENABLE;

  /* Write on MSR */
  wrmsr(MSR_AMD64_IBSOPCTL, low, high);

  return 1;
}

/* This function will be execute by a thread initialized in __init function */
int
thread_fn(void* args)
{
  printk(KERN_INFO "thread_fn() test\n");
  return 0;
}

static int
__init ibs_measure_monitor_init(void)
{
  int err;
  int ibs_method;
  unsigned int sampling_rate;

  /* printk for information */
  printk(KERN_INFO "START [%s]\n", __this_module.name);

  /* dispatched op-based sampling */
  sampling_rate = 0x1FFF0;
  ibs_method = 1;
  set_ibs_rate(sampling_rate, ibs_method);

  err = register_nmi_handler(NMI_LOCAL, handle_ibs_nmi, 0, __this_module.name);

  return 0;
}

static void
__exit ibs_measure_monitor_cleanup(void)
{
  /* stop ibs sampling */
  ibs_stop();

  /* printk for information */
  printk(KERN_INFO "STOP [%s]\n", __this_module.name);
  return;
}

module_init(ibs_measure_monitor_init);
module_exit(ibs_measure_monitor_cleanup);
