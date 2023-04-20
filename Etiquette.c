typedef struct Label Label;
struct Label{
    char *name;
    long address;
    Label *next;
};

typedef struct ListLabel ListLabel;
struct ListLabel{
    Label *first;
    long length;
};
//Initialise de la liste d etiquette
ListLabel *listLabelInit(){

    ListLabel *listLabel = malloc(sizeof(listLabel));

    if (listLabel == NULL) exit(-1);
    listLabel->first = NULL;
    listLabel->length = 0;

    return listLabel;
}
//Recupere l etiquette a un certain indice
Label *GetLabel(ListLabel *list, long index){
    long i;
    Label *label = list->first;

    if (list == NULL) exit(-1);

    //par convention, on donnera le premier element
    if (index >= list->length || index < 0) return list->first;

    for (i = 0; i < index; i++) label = label->next;

    return label;
}
//Rajoute une etiquette dans la liste chainee d etiquette
void pushLabel(ListLabel *list, char *name, long address){
    Label *new_label = malloc(sizeof(Label));
    if (list == NULL || new_label == NULL) exit(-1);

    // Allocation pour la chaine de caractere
    new_label->name = malloc((strlen(name) + 1) * sizeof(char));
    if (new_label->name == NULL) exit(-1);
    strcpy(new_label->name, name);
    new_label->address = address;
    new_label->next = NULL;
    if (list->length > 0){
        Label *last;
        last = GetLabel(list, list->length - 1);
        last->next = new_label;
    }
    else list->first = new_label;
    list->length++;
}

void printLabelName(char *name){
    printf("%s", name);
    printf(" : ");
}

void printLabelAddress(long address){
    printf("%ld", address);
    printf(" => ");
}

 void printListEtiquette(ListLabel *list){
    if (list == NULL){
        exit(-1);
    }
    Label *label = list->first;
    while (label != NULL){
        printLabelName(label->name);
        printLabelAddress(label->address);
        label = label->next;
    }
    printf("NULL\n");
    printf("%ld\n", list->length);
}

//Renvoie 1 si le mot est une etiquette (fini par :), 0 sinon
long IsLabel(char *word){
    long lengthword=strlen(word);
    if(word[lengthword-1]==':') return 1;
    else return 0;
}

//Renvoie l adresse associee au nom d'une etiquette (cherche ce nom dans ListLabel); renvoie -1 si elle trouve rien
long AddressLabel(ListLabel* list, char* str){
    for(long i=0; i<list->length; i++){
        Label* e=GetLabel(list,i);
        long nb=strcmp(e->name,str);
        if (nb==0){
            return e->address;
        }
    }
    return -1;
}
//Libere la memoire
void FreeLabel(ListLabel* listLabel){
    if (listLabel==NULL){
        exit(-1);
    }
    Label* label = listLabel->first;
    Label* tmp=NULL;
    while(label !=NULL){
        tmp=label->next;
        free(label->name);
        free(label);
        label=tmp;
    }
    free(listLabel);
}
