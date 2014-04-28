#include "order.h"


/** Initialisation struct Gestion 				**/
void init(Gestion* chain)				/* entrée: list des chaines */
{
		chain->racine=NULL;
		chain->next=chain;
}

/** Ajout maillon dans tab chaleur 				**/
/** Param : ptr tab chaleur, ptr task_struct 	**/
void add_in_table(Gestion* gest, struct task_struct* proc)
{
	Gestion* new = kmalloc(sizeof(Gestion), GFP_KERNEL);
    if ( new != NULL )
    {
		new->racine=proc;
		new->next=gest->next;
		gest->next=new;
	}
}

/** Incrementation compteur chaleur **/
/** Param : ptr task_struct			**/
void increase(struct task_struct* proc)
{
/*	proc->heat += INCR;
	if(proc->heat > LIST-1)
		proc->heat = LIST-1;
*/	if(proc->heat < LIST-1){
		proc->heat += INCR;
	}
}

/** Decrementation compteur chaleur **/
/** Param : ptr task_struct			**/
void decrease(struct task_struct* proc, int* coef)
{	
	proc->heat-=*coef;
	if(proc->heat <0)
		proc->heat = 0;
}

/** Vide tab chaleur et libère les lises chainées 	**/
/** Param : ptr tab chaleur							**/
void purge(Gestion* gest)
{
	int i;
	Gestion *it, *next;
	
	for(i=0 ; i<LIST ; i++)
	{
		for(it=gest[i].next ; it!=&(gest[i]) ; it=next)
		{
			next=it->next;
			kfree(it);
		}
		gest[i].next=&(gest[i]);
	}
}

/** Stocker les elems les plus actifs 								**/
/** Param : ptr tab chaleur, ptr tab result, ptr coef decremente	**/
void create_table_result(Gestion* gest, Process* result, int* coef)
{
	int i, j=LIST-1;
	int nbProc=0;
	Gestion* proc = kmalloc (sizeof(Gestion), GFP_KERNEL);
	
	proc=NULL;
	
	for(i=0 ; i<RESULT ; i++)
	{
		do
		{
			if(gest[j].next==gest+j)
				j--;
				
			else
			{
				if(proc==NULL)
				{
					proc=gest[j].next;
//					result[i]=*(proc->racine);
					result[i].id = proc->racine->pid;
					result[i].heat = proc->racine->heat;
					result[i].cpu = proc->racine->cpu;
				}
				else
				{
					result[i].id = proc->next->racine->pid;
					result[i].heat = proc->next->racine->heat;
					result[i].cpu = proc->next->racine->cpu;
//					result[i]=*(proc->next->racine);
					proc=proc->next;
				}
					
				if(proc->next==gest+j)
				{
					proc=NULL;
					j--;
				}
				
				nbProc++;
			}
		}while(gest[j].next==gest+j);
	}
	
	if(j==LIST-1)
	{
		while(proc->next!=gest+j)
		{
			nbProc++;
			proc=proc->next;
		}
		
		if((nbProc>(2*RESULT)) && (INCR<=(*coef+=1)))
			*coef=INCR-1;
	}
		
	else
	{
		if(*coef-=1<1)
			*coef=1;
	}
}

