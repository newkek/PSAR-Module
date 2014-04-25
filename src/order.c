#include <stdio.h>
#include <stdlib.h>
#include "list_proc.h"
#include "order.h"


/** Initialisation struct Gestion 				**/
void init(Gestion* chain)				/* entrée: list des chaines */
{
		chain->racine=NULL;
		chain->next=chain;
}

/** Ajout maillon dans tab chaleur 				**/
/** Param : ptr tab chaleur, ptr task_struct 	**/
void addSort(Gestion* gest, Process* current)
{
	Gestion* new = malloc ( sizeof *new );
    if ( new != NULL )
    {
		new->racine=current;
		new->next=gest->next;
		gest->next=new;
	}
}

/** Incrementation compteur chaleur **/
/** Param : ptr task_struct			**/
void increase(Process* current)
{
	current->heat += INCR;
	if(current->heat > LIST-1)
		current->heat = LIST-1;
}

/** Decrementation compteur chaleur **/
/** Param : ptr task_struct			**/
void decrease(Process* current, int* coef)
{	
	current->heat-=*coef;
	if(current->heat <0)
		current->heat = 0;
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
			free(it);
		}
		gest[i].next=&(gest[i]);
	}
}

/** Stocker les elems les plus actifs 								**/
/** Param : ptr tab chaleur, ptr tab result, ptr coef decremente	**/
void sortResult(Gestion* gest, Process* result, int* coef)
{
	int i, j=LIST-1;
	int nbProc=0;
	Gestion* current = malloc ( sizeof *current );
	
	current=NULL;
	
	for(i=0 ; i<RESULT ; i++)
	{
		do
		{
			if(gest[j].next==gest+j)
				j--;
				
			else
			{
				if(current==NULL)
				{
					current=gest[j].next;
					result[i]=*(current->racine);
				}
				else
				{
					result[i]=*(current->next->racine);
					current=current->next;
				}
					
				if(current->next==gest+j)
				{
					current=NULL;
					j--;
				}
				
				nbProc++;
			}
		}while(gest[j].next==gest+j);
	}
	
	if(j==LIST-1)
	{
		while(current->next!=gest+j)
		{
			nbProc++;
			current=current->next;
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

