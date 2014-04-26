#define INCR 4		/* Incrémentation du compteur */
#define	RESULT 2	/* Nbr de proc les plus actifs*/

/** Prototypes fonctions **/
void init(Gestion* chain);
void addSort(Gestion* gest, Process* current);
void increase(struct task_struct* current);
void decrease(struct task_struct* current, int* coef);
void sortResult(Gestion* gest, struct task_struct* result, int* coef);
void purge(Gestion* gest);
