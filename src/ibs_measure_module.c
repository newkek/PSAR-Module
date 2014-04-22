#include "ibs_measure_module.h"
//#include "ibs_managing.h"

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cpu.h>
#include <asm/nmi.h>
#include <asm/apic.h>
#include <linux/kthread.h>
#include <linux/nmi.h>
#include <linux/pci.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kévin Gallardo - Pierre-Yves Péneau");
MODULE_DESCRIPTION("TODO");



static DEFINE_PER_CPU(unsigned long, saved_lvtpc);



static struct task_struct *kt, *kts;




static int handle_ibs_nmi(unsigned int cmd, struct pt_regs *regs)
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
	printk("entered into IBS_OP_LOW_VALID_BIT");
      /* rdmsr(MSR_AMD64_IBSOPDATA2, low, high); */
      /* ibs_op->ibs_op_data2_low = low; */
      /* ibs_op->ibs_op_data2_high = high; */
      /* printk("MSR_AMD64_IBSOPDATA2 %d %d", low, high); */

  /*     if ( (!consider_L1L2) && ((ibs_op->ibs_op_data2_low & 7) == 0) ) */
  /*    goto end; */

  /*     if ( (ibs_op->ibs_op_data2_low & 7) == 3) */
  /*    /\* per_cpu(stats, cpu).total_samples_L3DRAM++; *\/ */
  /*    ; */

  /*     rdmsr(MSR_AMD64_IBSOPRIP, low, high); */
  /*     ibs_op->ibs_op_rip_low = low; */
  /*     ibs_op->ibs_op_rip_high = high; */
  /*     printk("MSR_AMD64_IBSOPRIP %d %d", low, high); */

      /* rdmsr(MSR_AMD64_IBSOPDATA, low, high); */
  /*     ibs_op->ibs_op_data1_low = low; */
  /*     ibs_op->ibs_op_data1_high = high; */
      /* printk("MSR_AMD64_IBSOPDATA %d %d\n", low, high); */

      /* rdmsr(MSR_AMD64_IBSOPDATA3, low, high); */
  /*     ibs_op->ibs_op_data3_low = low; */
  /*     ibs_op->ibs_op_data3_high = high; */
      /* printk("MSR_AMD64_IBSOPDATA3 %d %d\n", low, high); */

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
  /*    /\* goto end; *\/ */
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



void my_shutdown_apic(void* args)
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




int pfm_amd64_setup_eilvt(void)
{
#  define IBSCTL_LVTOFFSETVAL		(1 << 8)
#  define IBSCTL				0x1cc
   struct pci_dev *cpu_cfg;
   int nodes;
   u32 value = 0;

   /* per CPU setup */
//   on_each_cpu(apic_init_ibs_nmi_per_cpu, NULL, 1);

   nodes = 0;
   cpu_cfg = NULL;
   do {
      cpu_cfg = pci_get_device(PCI_VENDOR_ID_AMD,
            PCI_DEVICE_ID_AMD_10H_NB_MISC,
            cpu_cfg);
      if (!cpu_cfg)
         break;
      ++nodes;
      pci_write_config_dword(cpu_cfg, IBSCTL, IBSCTL_LVTOFFSETVAL
            | IBSCTL_LVTOFFSETVAL);
      pci_read_config_dword(cpu_cfg, IBSCTL, &value);
      if (value !=  IBSCTL_LVTOFFSETVAL) {
         printk(KERN_DEBUG "Failed to setup IBS LVT offset, "
               "IBSCTL = 0x%08x\n", value);
         return 1;
      }
   } while (1);

   if (!nodes) {
      printk(KERN_DEBUG "No CPU node configured for IBS\n");
      return 1;
   }

#ifdef CONFIG_NUMA
   /* Sanity check */
   /* Works only for 64bit with proper numa implementation. */
   if (nodes != num_possible_nodes()) {
      printk(KERN_DEBUG "Failed to setup CPU node(s) for IBS, "
            "found: %d, expected %d\n",
            nodes, num_possible_nodes());
      return 1;
   }
#endif
   return 0;
}







/* Does this function have to return NOTIFY_DONE ? */
static int ibs_cpu_notifier(struct notifier_block *b, unsigned long action, void *data)
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




static int thecpu;

static inline void apic_write_NMI (void *args){
  apic_write(APIC_LVTPC, APIC_DM_NMI);
}  


/* the big work */
void ibs_start(void)
{
  int cpu;

  cpu = smp_processor_id();
  thecpu = cpu;
  /* Init apic on all CPU */
  //on_each_cpu(apic_init_ibs_nmi_per_cpu, NULL, 1);
  

  per_cpu(saved_lvtpc, cpu) = apic_read(APIC_LVTPC);

  register_cpu_notifier(&ibs_cpu_nb);
printk("notifier registered \n");
  
  register_nmi_handler(NMI_LOCAL, handle_ibs_nmi, 0, __this_module.name);
printk("Handler registered\n");

  on_each_cpu(apic_write_NMI, NULL, 1);
printk("apic NMI wrote\n");

  on_each_cpu(apic_init_ibs_nmi_per_cpu, NULL, 1);
printk("APIC_init_ibs_nmi_per_cpu done\n");

  pfm_amd64_setup_eilvt();
printk("pfm done\n");

  on_each_cpu(set_ibs_rate, NULL, 1);
printk("ibs started\n");

  /* Enable NMI by writing on APIC_LVTPC register */
  /* Init notifier. TODO: check if it's useless */
  /* Register handler */
  /* Start measures */
  //on_each_cpu(set_ibs_rate, NULL, 1);

  return;
}


static void __stop_MSR(void* args){

  unsigned int low, high;

  /* Disable all bits */
  low = 0;
  high = 0;
  /* Write on MSR CTL register */
  wrmsr(MSR_AMD64_IBSOPCTL, low, high);
  return ;
}





static void __shutdown_APIC (void* args){
 /* restoring APIC_LVTPC can trigger an apic error because the delivery
 * mode and vector nr combination can be illegal. That's by design: on
 * power on apic lvt contain a zero vector nr which are legal only for
 * NMI delivery mode. So inhibit apic err before restoring lvtpc
 */
  unsigned int v;
  int cpu ;
  cpu = smp_processor_id();
  v = apic_read(APIC_LVTERR);
  apic_write(APIC_LVTERR, v | APIC_LVT_MASKED);
  apic_write(APIC_LVTPC, per_cpu(saved_lvtpc, cpu));
  apic_write(APIC_LVTERR, v);
  return ;
}
  



/* This function stop IBS sampling by writing on MSR registers */
void ibs_stop(void)
{
  //unsigned int low, high;
  int cpu;
  /* Les APIC */
  //unsigned int v;
  cpu = smp_processor_id();


  smp_call_function_single(thecpu, __stop_MSR, NULL, 0);
printk("__stop done\n");
  smp_call_function_single(thecpu, __shutdown_APIC, NULL, 0);
 
printk("__shutdown done\n");
 
  /* Shutdown APIC on each CPU */
  //on_each_cpu(my_shutdown_apic, NULL, 1);
  smp_call_function_single(thecpu, my_shutdown_apic, NULL, 0);
printk("my shutdown apic done\n");
 
  /* Unregister NMI_LOCAL */
  unregister_nmi_handler(NMI_LOCAL, __this_module.name);
printk("handler unregistered \n");
 

 /* Unregister CPU notifier */
  unregister_cpu_notifier(&ibs_cpu_nb);
printk("Out\n");
}





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

  kts = kthread_create(thread_fn_stop, NULL, "kt");
  if (kts){
	printk("thread successfully started");
	wake_up_process(kts);
  }

  return;
}

module_init(ibs_measure_monitor_init);
module_exit(ibs_measure_monitor_cleanup);
