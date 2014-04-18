#include "ibs_measure_module.h"

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cpu.h>
#include <asm/nmi.h>
#include <asm/apic.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kévin Gallardo - Pierre-Yves Péneau");
MODULE_DESCRIPTION("TODO");

static DEFINE_PER_CPU(unsigned long, saved_lvtpc);

void
my_shutdown_apic_wesh(void* args)
{
  u8 lvt_off = APIC_EILVT_LVTOFF_IBS;
  u8 mask = 1;
  u8 msg_type = APIC_EILVT_MSG_FIX;
  u8 vector = 0;
  unsigned long reg = (lvt_off << 4) + APIC_EILVT0;
  unsigned int  v   = (mask << 16) | (msg_type << 8) | vector;

  apic_write(reg, v);
  return;
}

/* This function initialize IBS by writing values on MSR registers */
void set_ibs_rate(void *args)
{
  unsigned int low, high;
  uint32_t rand = 0;

  /* value for IbsOpCntCtl bit */
  low = (((0x1FFF0 + rand) >> 4) & 0xFFFF) + ((1 & 0x1) << 19) \
    + IBS_OP_LOW_ENABLE;
  high = 0;

  /* Write on MSR CTL register */
  wrmsr(MSR_AMD64_IBSOPCTL, low, high);
}

static inline void apic_init_ibs_nmi_per_cpu (void * args)
{
  unsigned long reg;
  unsigned int  v;
  /* TODO: understand this two lines */
  /* Really.. ? */
  reg = (APIC_EILVT_LVTOFF_IBS << 4) + APIC_EILVT0;
  v = (0 << 16) | (APIC_EILVT_MSG_NMI << 8) | 0;
  /* Write v on apic register */
  apic_write(reg, v);
}

/* Is this function  must return NOTIFY_DONE ? */
static int
ibs_cpu_notifier(struct notifier_block *b, unsigned long action, void *data)
{
  printk("ibs_cpu_notifier called\n");
  return NOTIFY_DONE;
}

/* 
 * Init function for notifier_call 
 * This structure is defined in the kernel 
 */
static struct notifier_block ibs_cpu_nb = {
  .notifier_call = ibs_cpu_notifier
};

void
ibs_start(void)
{
  int cpu;

  cpu = smp_processor_id();

  /* Init apic on all CPU */
  on_each_cpu(apic_init_ibs_nmi_per_cpu, NULL, 1);

  per_cpu(saved_lvtpc, cpu) = apic_read(APIC_LVTPC);

  /* Enable NMI by writing on APIC_LVTPC register */
  apic_write(APIC_LVTPC, APIC_DM_NMI);
  /* Init notifier. TODO: check if it's useless */
  register_cpu_notifier(&ibs_cpu_nb);
  /* Register handler */
  register_nmi_handler(NMI_LOCAL, handle_ibs_nmi, 0, __this_module.name);
  /* Start measures */
  on_each_cpu(set_ibs_rate, NULL, 1);  

  return;
}

/* This function stop IBS sampling by writing on MSR registers */
void
ibs_stop(void)
{
  unsigned int low, high;

  /* Les APIC */
  unsigned int v;
  int cpu ;

  /* Disable all bits */
  low = high = 0;
  /* Write on MSR CTL register */
  wrmsr(MSR_AMD64_IBSOPCTL, low, high);

  cpu = smp_processor_id();

  /* restoring APIC_LVTPC can trigger an apic error because the delivery
   * mode and vector nr combination can be illegal. That's by design: on
   * power on apic lvt contain a zero vector nr which are legal only for
   * NMI delivery mode. So inhibit apic err before restoring lvtpc
   */
  v = apic_read(APIC_LVTERR);
  apic_write(APIC_LVTERR, v | APIC_LVT_MASKED);
  apic_write(APIC_LVTPC, per_cpu(saved_lvtpc, cpu));
  apic_write(APIC_LVTERR, v);

  /* Unregister NMI_LOCAL */
  unregister_nmi_handler(NMI_LOCAL, __this_module.name);

  /* Shutdown APIC on each CPU */
  on_each_cpu(my_shutdown_apic_wesh, NULL, 1);

  /* Unregister CPU notifier */
  unregister_cpu_notifier(&ibs_cpu_nb);

}

/* This function will be called on every interruption */
static int
handle_ibs_nmi(struct pt_regs* const regs)
{
  int cpu;
  int consider_L1L2;
  unsigned int low;
  unsigned int high;

  /* Structure qui va contenir tout ce qui aura été mis dans les MSR */
  /* TODO: trouver la doc sur la récup des données */
  /* struct ibs_op_sample  ibs_op_stack; */
  /* struct ibs_op_sample* ibs_op; */

  consider_L1L2 = 0;
  printk(KERN_INFO "this is a test\n");
  /* On récupère le numéro du CPU */
  cpu = smp_processor_id();
  /* ibs_op = &ibs_op_stack; */
  
  /* per_cpu(stats, cpu).total_interrupts++; */

  rdmsr(MSR_AMD64_IBSOPCTL, low, high);

  /* VALID_BIT enabled: sampling ended normally */
  if (low & IBS_OP_LOW_VALID_BIT)
    {
      /* rdmsr(MSR_AMD64_IBSOPDATA2, low, high); */
      /* ibs_op->ibs_op_data2_low = low; */
      /* ibs_op->ibs_op_data2_high = high; */
      /* printk("MSR_AMD64_IBSOPDATA2 %d %d", low, high); */

  /*     if ( (!consider_L1L2) && ((ibs_op->ibs_op_data2_low & 7) == 0) ) */
  /* 	goto end; */

  /*     if ( (ibs_op->ibs_op_data2_low & 7) == 3) */
  /* 	/\* per_cpu(stats, cpu).total_samples_L3DRAM++; *\/ */
  /* 	; */
      
  /*     rdmsr(MSR_AMD64_IBSOPRIP, low, high); */
  /*     ibs_op->ibs_op_rip_low = low; */
  /*     ibs_op->ibs_op_rip_high = high; */
  /*     printk("MSR_AMD64_IBSOPRIP %d %d", low, high); */

      rdmsr(MSR_AMD64_IBSOPDATA, low, high);
  /*     ibs_op->ibs_op_data1_low = low; */
  /*     ibs_op->ibs_op_data1_high = high; */
      printk("MSR_AMD64_IBSOPDATA %d %d\n", low, high);

      rdmsr(MSR_AMD64_IBSOPDATA3, low, high);
  /*     ibs_op->ibs_op_data3_low = low; */
  /*     ibs_op->ibs_op_data3_high = high; */
      printk("MSR_AMD64_IBSOPDATA3 %d %d\n", low, high);

  /*     rdmsr(MSR_AMD64_IBSDCLINAD, low, high); */
  /*     ibs_op->ibs_dc_linear_low = low; */
  /*     ibs_op->ibs_dc_linear_high = high; */
  /*     printk("MSR_AMD64_IBSOPCLINAD %d %d", low, high); */

  /*     rdmsr(MSR_AMD64_IBSDCPHYSAD, low, high); */
  /*     ibs_op->ibs_dc_phys_low = low; */
  /*     ibs_op->ibs_dc_phys_high = high; */
  /*     printk("MSR_AMD64_IBSDCPHYSAD %d %d", low, high);       */
      
  /*     if(ibs_op->phys_addr == 0) */
  /*          goto end; */

  /*     /\* if(ibs_op->lin_addr < min_lin_address || ibs_op->lin_addr > max_lin_address) { *\/ */
  /* 	/\* goto end; *\/ */
    }

  /* /\* per_cpu(stats, cpu).total_samples++; *\/ */

  /* rdmsr(MSR_AMD64_IBSOPCTL, low, high); */
  /* printk("MSR_AMD64_IBSOPCTL %d %d", low, high); */

 end: __attribute__((unused))
  /* Reset high */
  high = 0;
  /* Disable VALID_BIT because sampling is done */
  low &= ~IBS_OP_LOW_VALID_BIT;
  /* Enable ENABLE for new sampling */
  low |= IBS_OP_LOW_ENABLE;

  /* Write on MSR CTL register */
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
  /* printk for information */
  printk(KERN_INFO "START [%s]\n", __this_module.name);
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
