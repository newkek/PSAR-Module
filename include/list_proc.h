#define MAX 1000
#define LIST 64		/* Taille du tab de chaleur */

/** Definition proc (task_struct) **/
typedef struct ProcessList{
	int id;
	int heat;
	struct ProcessList* next;
}Process;

/** Pointeur sur task_struct et sur le maillon suivant **/
typedef struct GestionList{
	struct task_struct* racine;
	struct GestionList* next;
}Gestion;

/** Prototypes fonctions **/
int addNew(Gestion* gest, int val);
int search(Gestion* chain,int val);
/*int rm(Gestion* gest, Process* current);*/
