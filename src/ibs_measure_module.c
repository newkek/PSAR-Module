#include "ibs_measure_module.h"


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kévin Gallardo - Pierre-Yves Péneau");
MODULE_DESCRIPTION("TODO");


static struct hrtimer hr_timer;


static struct task_struct* thread1, *thread2;
static struct completion on_exit;

static struct cpumask dst;


static ktime_t kt_period;


int keep_running;


static char* options = "init";
module_param(options, charp, 0000);
MODULE_PARM_DESC(options, "Options for the numa monitoring");



ibs_sample_list* samples;




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
handle_ibs_nmi(unsigned int cmd, struct pt_regs* const regs)
{
	int cpu;
	unsigned int low;
	unsigned int high;
	struct ibs_op_sample ibs_op;
	/* struct ibs_op_sample *ibs_op = &ibs_op_stack; */
	
	/* #if DUMP_OVERHEAD */
	uint64_t time_start,time_stop;
	
	cpu = smp_processor_id();
	rdtscll(time_start);
	/* #endif */
	
	/* per_cpu(stats, cpu).total_interrupts++; */
	rdmsr(MSR_AMD64_IBSOPCTL, low, high);
	printk(KERN_INFO "this is a test\n");
	
	if (low & IBS_OP_LOW_VALID_BIT)
	{
		rdmsr(MSR_AMD64_IBSOPDATA2, low, high);
		/* If the sample does not touch DRAM, stop. */
		/* if((ibs_op->ibs_op_data2_low & 7) != 3) { */
		/* 	      goto end; */
		/* } */
		/* if((!consider_L1L2) && ((ibs_op->ibs_op_data2_low & 7) == 0)) { */
		/* 	     goto end; */
		/* } */
		/* if((ibs_op->ibs_op_data2_low & 7) == 3)  */
		/* 	     per_cpu(stats, cpu).total_samples_L3DRAM++; */
		ibs_op.ibs_op_data2_low = low; 
		ibs_op.ibs_op_data2_high = high;
		printk("MSR_AMD64_IBSOPDATA2 %d %d\n", low, high);

		rdmsr(MSR_AMD64_IBSOPRIP, low, high);
		ibs_op.ibs_op_rip_low = low; 
		ibs_op.ibs_op_rip_high = high;
		printk("MSR_AMD64_IBSOPRIP %d %d\n", low, high);

		rdmsr(MSR_AMD64_IBSOPDATA, low, high);
		ibs_op.ibs_op_data1_low = low;
		ibs_op.ibs_op_data1_high = high;
		printk("MSR_AMD64_IBSOPDATA %d %d\n", low, high);

		rdmsr(MSR_AMD64_IBSOPDATA3, low, high);
		ibs_op.ibs_op_data3_low = low;
		ibs_op.ibs_op_data3_high = high;
		printk("MSR_AMD64_IBSOPDATA3 %d %d\n", low, high);

		rdmsr(MSR_AMD64_IBSDCLINAD, low, high);
		ibs_op.ibs_dc_linear_low = low;
		ibs_op.ibs_dc_linear_high = high;
		printk("MSR_AMD64_IBSOPCLINAD %d %d\n", low, high);
	
	
		/* if(ibs_op->lin_addr < min_lin_address || ibs_op->lin_addr > max_lin_address) { */
		/* goto end; */
	}
	
	
	
	rdmsr(MSR_AMD64_IBSDCPHYSAD, low, high);
	ibs_op.ibs_dc_phys_low = low; 
	ibs_op.ibs_dc_phys_high = high;
	printk("MSR_AMD64_IBSDCPHYSAD %d %d\n", low, high);

	rdmsr(MSR_AMD64_IBSDCLINAD, low, high);
	ibs_op.ibs_dc_linear_low = low; 
	ibs_op.ibs_dc_linear_high = high;
	printk("MSR_AMD64_IBSDCLINAD %d %d\n", low, high);
	
	rdmsr(MSR_AMD64_IBSBRTARGET, low, high);
	ibs_op.ibs_br_trg_low = low; 
	ibs_op.ibs_br_trg_high = high;
	printk("MSR_AMD64_IBSBRTARGET %d %d\n", low, high);

	ibs_op.cpu=cpu;

	
	add_to_queue(samples, ibs_op);


	rdmsr(MSR_AMD64_IBSOPCTL, low, high);
	high = 0;
	low &= ~IBS_OP_LOW_VALID_BIT;
	low |= IBS_OP_LOW_ENABLE;
	/* #if DUMP_OVERHEAD */
	/* rdtscll(time_stop); */
	/* per_cpu(stats, cpu).time_spent_in_NMI += time_stop - time_start; */
	/* #endif */
	wrmsr(MSR_AMD64_IBSOPCTL, low, high);
	
	return 1;
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


int init_samples(void){
	samples = create_list();
	if(samples != NULL)
		return 1; //successfull
	return 0;


}


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





static void 
ibs_init(void){
	on_each_cpu(apic_init_ibs_nmi_per_cpu, NULL, 1);
	register_cpu_notifier(&ibs_cpu_nb);
	register_nmi_handler(NMI_LOCAL, handle_ibs_nmi, 0, "psar");
}




static void
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
ibs_start_cpu(int cpu){
	smp_call_function_single(cpu, set_ibs_rate, NULL, 1);
}




static void 
ibs_stop_cpu(void* args){

	unsigned int low;
	unsigned int high;
	
	low = 0;
	high = 0;
	/* clear max count and enable */
	wrmsr(MSR_AMD64_IBSOPCTL, low, high);
	return;

}


static void
ibs_stop(unsigned int cpu){
	smp_call_function_single(cpu, ibs_stop_cpu, NULL, 1);
}




static void
test(void* args){
	printk("[test] CPU : %d\n", smp_processor_id());
}




static int 
thread_fn(void* args){
	
	int count = 0;
	struct ibs_op_sample op;
	struct task_struct it;


	printk("[%s] thread launcher pid : %d, cpu : %d\n", __this_module.name, current->pid, smp_processor_id());

	/* SECURITIES FOR KERNEL THREAD */
        set_freezable();
        allow_signal(SIGKILL);
	set_current_state(TASK_INTERRUPTIBLE);
        schedule_timeout(5);


	/* INIT FOR THE EXIT THREAD */
	init_completion(&on_exit);
	
	/* SAMPLES INITIALIZATION */	
	if(!init_samples()){
		printk(KERN_INFO "[%s] LIST INITIALIZATION FAILED !\n", __this_module.name);
		do_exit(0);
		return 1;
	}
	else{
		printk(KERN_INFO "[%s] LIST INITIALIZATION SUCCESSFULL\n", __this_module.name);
	}



	/* INIT MEASURES */
	
/*	ibs_init();
*/	


	do{

		for_each_process(it){
			if(it->state == TASK_RUNNING){
				/* code eric : ajouter dans le tableau, mettre à jour sa chaleur */
			}
			else{
				/*code eric : diminuer sa chaleur */
			}
			
		}
		count++;
		if (count > measures_rate){
			/* nouvelle phase de  mesures */


			/* arrêter les mesures précédentes grace au tableau précédemment enregistré : */
			/* pour chaque thread dans le tableau precedemment enregistré (t){
				ibs_stop(t->cpu);
			}
			*/

			/* code eric : récupérer le tableau des threads les plus chauds */

			count = 0;

			/* lancement de nouvelles mesures : */
			/* pour chaque thread dans le tableau des threads les plus chauds  (t){
				ibs_start(t->cpu);
				
			}
			mémoriser le tableau pour arreter les mesures plus tard
			*/
		}
		/* code eric : flush tableau */


	} while(!kthread_should_stop());

	
	printk("Exit\n");
	return 0;
}





static int
thread_fn2(void *args)
{

	/* THREAD SECURITIES */
	set_freezable();
	allow_signal(SIGKILL);
        set_current_state(TASK_INTERRUPTIBLE);
        schedule_timeout(10);


	printk(KERN_INFO "in thread_fn2()\n");
/*	on_each_cpu(ibs_stop, NULL, 1); */

	kthread_stop(thread1);

	/* stopper les dernieres mesures lancees : */
	/* pour chaque thread dans le tableau (t){
		ibs_stop(t->cpu);
	}
	*/


	/* retourner les data dans un fichier */	
	/* free les data */


	/* Allow module_exit to exit */
	complete(&on_exit);

	do_exit(0);
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
	int error;

	delay_in_ms = 1000L;
	sym_addr = kallsyms_lookup_name(sym_name);
	
	printk(KERN_INFO "[%s] %s (0x%lx)\n", __this_module.name, sym_name, sym_addr);
	
	printk("HR Timer module installing\n");

	
		
	thread1 = kthread_run(thread_fn, NULL, "ibs_monitor1");
	if(IS_ERR(thread1))
	{
	        error = PTR_ERR(thread1);
	        return error;
	}


	printk(KERN_INFO "test  : %s\n", options);	
	return 0;
}



static void
__exit ibs_measure_monitor_cleanup( void )
{
  	int error;
	
  	printk(KERN_INFO "HR Timer module uninstalling\n");

	thread2 = kthread_run(thread_fn2, NULL, "ibs_monitor2");

	if(IS_ERR(thread2))
	{
	        error = PTR_ERR(thread2);
	        return;
	}
	wait_for_completion(&on_exit);
	if(samples != NULL)
		kfree(samples);
	return;
	
}

module_init(ibs_measure_monitor_init);
module_exit(ibs_measure_monitor_cleanup);
