#include <stdio.h>
#include <stdlib.h>
#include "list_proc.h"
#include "order.h"

/** Ajout a la liste des procs **/
int addNew(Gestion* gest, int val)		/*entrée identifiant, + elem gestion */
{
    Process* new = malloc ( sizeof *new );
    if ( new != NULL )
    {
        new->id   = val;
        new->next = gest->racine;
        new->heat = 0;
        
        increase(new);
        
        gest->racine = new;
		return 0;
    }
    return -1;
}

/** Recherche un elem a la liste des procs **/
int search(Gestion* gest,int val)
{
	Process* it;
	
	if(gest->racine == NULL)
	{
		addNew(gest,val);
		return 0;
	}
	
	for(it=gest->racine ; it!=NULL ; it=it->next)
	{
		if(it->id == val)
		{
			increase(it);
			return 0;
		}
	}
	
	addNew(gest,val);
    return 0;
}    
    
   /* entrée pointeur structure, pointeur du debut de la list */
/*int rm(Gestion* gest, Element* current)
{
	if(gest->racine == current)
	{
		if(current->next == current)
		{
			gest->racine = NULL;
			free(current);
			gest->nbElem --;
			return 0;
		}
		else
			gest->racine = current->next;
	}
	
	current->next->prev = current->prev;
	current->prev->next = current->next;
	
	free(current);
	gest->nbElem --;
	return 0;
}*/
