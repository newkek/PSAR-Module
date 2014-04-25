#define INCR 4		/* Incrémentation du compteur */
#define	RESULT 2	/* Nbr de proc les plus actifs*/

/** Prototypes fonctions **/
void init(Gestion* chain);
void addSort(Gestion* gest, Process* current);
void increase(Process* current);
void decrease(Process* current, int* coef);
void sortResult(Gestion* gest, Process* result, int* coef);
void purge(Gestion* gest);
