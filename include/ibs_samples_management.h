#ifndef __H_IBS_SAMPLES
#define __H_IBS_SAMPLES

#include "ibs_defs.h"
#include <linux/slab.h>



typedef struct list{
	struct list* next;
	struct list* prev;
	struct ibs_op_sample sample;
} ibs_sample_list;

ibs_sample_list* create_list (void);
void add_before(ibs_sample_list*, struct ibs_op_sample);
void add_to_queue(ibs_sample_list*, struct ibs_op_sample);


#endif 
