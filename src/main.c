#include <stdio.h>
#include <stdlib.h>
#include "list_proc.h"
#include "order.h"

int main(int argc, char** argv)
{
  int proc[5][10]=	{{1,2,5,7,9,10,11,13,18,20},
			 {2,3,5,5,6,11,12,13,16,18},
			 /*{1,4,5,6,9,10,12,15,17,20},*/
			 /*{4,5,7,8,10,12,13,14,15,19},*/
			 {5,5,5,5,5,5,5,5,5,5},
			 {5,5,5,5,5,5,5,5,5,5},
			 {2,3,5,8,9,11,13,15,16,17}};
						 
  int proc2[5][10]=	{{1,2,3,4,5,1,2,3,4,5},
			 {1,2,3,4,5,1,2,3,4,5},
			 {1,2,3,4,5,1,2,3,4,5},
			 {1,2,3,4,5,1,2,3,4,5},
			 {1,2,3,4,5,1,2,3,4,5}};
  int proc3[5][10]=	{{1,2,3,4,5,1,2,3,4,5},
			 {1,2,3,4,5,1,2,3,4,5},
			 {1,2,3,4,5,1,2,3,4,5},
			 {1,2,3,4,5,1,2,3,4,5},
			 {1,2,3,4,5,1,2,3,4,5}};
  int proc4[5][10]=	{{1,2,5,7,9,10,11,13,18,20},
			 {2,3,5,5,6,11,12,13,16,18},
			 {1,4,5,6,9,10,12,15,17,20},
			 {4,5,7,8,10,12,13,14,15,19},
			 {2,3,5,8,9,11,13,15,16,17}};
  /** Variables **/
  Gestion processus;			/* Liste des procs */
  Gestion heatTab[LIST];		/* Table de chaleur */
  Process resultTab[RESULT];	/* Table des procs les plus actif */
  int coefDec=1;

  /* Tampons */
  int i,j;
  Gestion *it;
  Process *elem;

  printf("Start\n");

  /** Initialisation **/
  init(&processus);
	
  for(i=0;i<LIST;i++)
    init(&(heatTab[i]));

  /** Début **/
  printf("Premier\n");
  for(i=0 ; i<5 ; i++)
    {
      purge(heatTab);
		
      for(elem=processus.racine ; elem!=NULL ; elem=elem->next)
	decrease(elem,&coefDec);
		
      for(j=0 ; j<10 ; j++)
	search(&processus, proc[i][j]);
		
      for(elem=processus.racine ; elem!=NULL ; elem=elem->next){
	addSort(&(heatTab[elem->heat]),elem);			
      }
    }
	
  sortResult(heatTab, resultTab, &coefDec);
  printf("\tcoef %d\n",coefDec);
	
  /**********************************************************************/
  printf("Second\n");
  for(i=0 ; i<5 ; i++)
    {
      purge(heatTab);
		
      for(elem=processus.racine ; elem!=NULL ; elem=elem->next)
	decrease(elem,&coefDec);
		
      for(j=0 ; j<10 ; j++)
	search(&processus, proc2[i][j]);
		
      for(elem=processus.racine ; elem!=NULL ; elem=elem->next)
	addSort(&(heatTab[elem->heat]),elem);
    }
  sortResult(heatTab, resultTab, &coefDec);
  printf("\tcoef %d\n",coefDec);
	
  /**********************************************************************/
  printf("Troisieme\n");
  for(i=0 ; i<5 ; i++)
    {
      purge(heatTab);
		
      for(elem=processus.racine ; elem!=NULL ; elem=elem->next)
	decrease(elem,&coefDec);
		
      for(j=0 ; j<10 ; j++)
	search(&processus, proc3[i][j]);
		
      for(elem=processus.racine ; elem!=NULL ; elem=elem->next)
	addSort(&(heatTab[elem->heat]),elem);
    }
  sortResult(heatTab, resultTab, &coefDec);
  printf("\tcoef %d\n",coefDec);
	
  /**********************************************************************/
  printf("Quatrieme\n");
  for(i=0 ; i<5 ; i++)
    {
      purge(heatTab);
		
      for(elem=processus.racine ; elem!=NULL ; elem=elem->next)
	decrease(elem,&coefDec);
		
      for(j=0 ; j<10 ; j++)
	search(&processus, proc4[i][j]);
		
      for(elem=processus.racine ; elem!=NULL ; elem=elem->next)
	addSort(&(heatTab[elem->heat]),elem);
    }
  sortResult(heatTab, resultTab, &coefDec);
  printf("\tcoef %d\n",coefDec);
	
  /**********************************************************************/
	
	
	
  /** Affichage **/
  for(i=0 ; i<LIST ; i++)
    {
      printf("\nTemperature : %d\n", i);
		
      for(it=heatTab[i].next ; it!=&(heatTab[i]) ; it=it->next)
	printf("\tID:%d Heat:%d\n",it->racine->id,it->racine->heat);
    }
	
  printf("\nCOUCOU\n\n");
	
  for(i=0 ; i<RESULT ; i++)
    printf("Id : %d\n",resultTab[i].id);
	
	
  printf("END\n");
  return 0;
}
