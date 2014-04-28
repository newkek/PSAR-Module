#define INCR 4		/* Incrémentation du compteur */
#define	RESULT 2	/* Nbr de proc les plus actifs*/
#define MAX 1000
#define LIST 64         /* Taille du tab de chaleur */

#include <linux/slab.h>

/** Definition proc (task_struct) **/
typedef struct ProcessList{
        int id;
        int heat;
        int cpu;
}Process;

/** Pointeur sur task_struct et sur le maillon suivant **/
typedef struct GestionList{
        struct task_struct* racine;
        struct GestionList* next;
}Gestion;

/** Prototypes fonctions **/
void init(Gestion* chain);
void add_in_table(Gestion* gest, struct task_struct* proc);
void increase(struct task_struct* proc);
void decrease(struct task_struct* proc, int* coef);
void create_table_result(Gestion* gest, Process* result, int* coef);
void purge(Gestion* gest);
