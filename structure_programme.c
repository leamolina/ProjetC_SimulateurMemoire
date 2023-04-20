#include "Etiquette.c"

typedef struct Element Element;
struct Element{
    char *val;
    Element *next;
};

typedef struct ListLine ListLine;
struct ListLine{
    Element *word;
    int length;
    int num_line;
    ListLine *next;
};

typedef struct ListProgram ListProgram;
struct ListProgram{
    ListLine *first;
    int length;
};

ListProgram *listProgramInit(){

    ListProgram *listProgram = malloc(sizeof(listProgram));

    if (listProgram == NULL){
        exit(-1);
    }

    listProgram->first = NULL;
    listProgram->length = 0;
    return listProgram;
}

ListLine *listLineInit(int num_line){

    ListLine *listLine = malloc(sizeof(listLine));

    if (listLine == NULL){
        exit(-1);
    }

    listLine->word = NULL;
    listLine->length = 0;
    listLine->num_line = num_line;
    return listLine;
}

ListLine *GetListLine(ListProgram *listProgram, int index){
    int i;
    ListLine *listLine = listProgram->first;

    if (listProgram == NULL){
        exit(-1);
    }

    if (index >= listProgram->length || index < 0){
        return listProgram->first;
    }

    for (i = 0; i < index; i++){
        listLine = listLine->next;
    }

    return listLine;
}

void pushListInListProgram(ListProgram *listProgram, ListLine *listLine){

    if (listProgram == NULL || listLine == NULL){
        exit(-1);
    }

    if (listProgram->length > 0){
        ListLine *last;
        last = GetListLine(listProgram, listProgram->length - 1);
        last->next = listLine;
    }
    else{
        listProgram->first = listLine;
    }

    listProgram->length++;
}

Element *GetElement(ListLine *list, int index){
    int i;
    Element *element = list->word;

    if (list == NULL){
        exit(-1);
    }

    if (index >= list->length || index < 0){
        return list->word;
    }

    for (i = 0; i < index; i++){
        element = element->next;
    }

    return element;
}


void pushElementInListLine(ListLine *list, char *val){  //Creer une nouvelle case de la liste chainee qui contient char* val

    Element *new = malloc(sizeof(Element));

    if (list == NULL || new == NULL){
        exit(-1);
    }

    new->val = malloc((strlen(val) + 1) * sizeof(char));

    if (new->val == NULL){
        exit(-1);
    }

    strcpy(new->val, val); // Pq on met pas direct new->val=val
    new->next = NULL;

    if (list->length > 0){
        Element *last;
        last = GetElement(list, list->length - 1);
        last->next = new;
    }

    else{
        list->word = new;
    }

    list->length++;
}

void printElement(char *content){
    printf("%s", content);
    printf(" -> ");
}


 void printListLine(ListLine *list){
    if (list == NULL){
        exit(-1);
    }

    Element *elem = list->word;

    while (elem != NULL){
        printElement(elem->val);
        elem = elem->next;
    }

    printf("NULL\n");
    printf("Taille de la liste de mots: %d\n", list->length);
    printf("Numéro de la ligne: %d\n", list->num_line);
    printf("Numéro de la l'adresse: %d\n", (list->num_line) * 4);
    printf("-------\n");

}

 void printListProgram(ListProgram *list){
    if (list == NULL){
        exit(-1);
    }
    ListLine *listLine = list->first;

    while (listLine != NULL){
        printListLine(listLine);
        listLine = listLine->next;
    }

    printf("NULL\n");
    printf("Taille de la ListeProgramme: %d\n", list->length);
}
//Liberation memoire
void FreeListLine(ListLine *list){
    if (list == NULL){
        exit(-1);
    }

    Element *elem = list->word;
    Element *tmp = NULL;
    while (elem != NULL){
        tmp = elem->next;
        free(elem->val);
        free(elem);
        elem = tmp;
    }
    free(list);
}
//Liberation memoire
void FreeListProgram(ListProgram* list_prg){
    if(list_prg==NULL){
        exit(-1);
    }
    ListLine * list= list_prg->first;
    ListLine *tmp = NULL;
    while(list != NULL){
        tmp=list->next;
        FreeListLine(list);
        list=tmp;
    }
    free(list_prg);
}
