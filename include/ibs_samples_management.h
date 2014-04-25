#ifndef __H_IBS_SAMPLES
#define __H_IBS_SAMPLES

#include "ibs_defs.h"
#include <linux/slab.h>



typedef struct list{
	struct list* next;
	struct list* prev;
	struct ibs_op_sample sample;
} ibs_sample_list;




#endif 
