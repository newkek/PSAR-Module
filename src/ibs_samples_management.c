#include "ibs_samples_management.h"





ibs_sample_list* create_list (void){
        ibs_sample_list* root;
        root = kmalloc(sizeof(ibs_sample_list), GFP_KERNEL);
        if(root != NULL){
                root->next = root;
                root->prev = root;
        } 
        return root;
}




void add_before(ibs_sample_list* elt, struct ibs_op_sample param){
        ibs_sample_list* new = kmalloc(sizeof(ibs_sample_list), GFP_KERNEL);
        if(new != NULL){
                memcpy(&(new->sample), &param, sizeof(struct ibs_op_sample));
                new->prev = elt->prev;
                new->next = elt;
                elt->prev->next = new;
                elt->prev = new;
        }
}




void add_to_queue(ibs_sample_list* root, struct ibs_op_sample param){
        add_before(root, param);
}


