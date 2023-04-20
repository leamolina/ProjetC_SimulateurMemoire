#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include <time.h>

long Z;
long C;
long N;
long PC;
char memory[65536][2];
char* FileName;

#include "Instructions.c"

int main(int ARGV, char* ARGC[]){
    char line[T_MAX];
    char* FileNameText=malloc(256*sizeof(char));
    FileNameText=ARGC[1];
    FILE *f= fopen(FileNameText,"r");
    if (f==NULL){
        printf("Le fichier texte inscrit en ligne de commande est introuvable. Execution impossible. \n");
        return 0;
    }

    // Création de la liste du programe et de la liste des etiquettes
    ListProgram *List_prg = listProgramInit();
    ListLabel *listLabel = listLabelInit();
    long num_line = 0;

    // Remplir la liste par chaque ligne du fichier (1 element de Liste_prg <=> 1 ligne)
    while (fgets(line, sizeof(line), f)) {
        ListLine *List_line = listLineInit(num_line);

        delete_caractere(line, ",");
        //Remplissage de List_line
        char* space=" ";
        char e[T_MAX];
        char* word=strtok(line,space);
            if (IsLabel(word)==1){
                strcpy(e,word);
                delete_caractere(e,":");
                delete_caractere(e,"\n");
                pushLabel(listLabel, e, num_line*4);
            }
            while (word != NULL ) {
                if (IsLabel(word)==1){
                    word = strtok ( NULL,space );
                    continue;
                }
                delete_caractere(word,"\r\n.\r"); //Au cas ou c est le dernier mot de la ligne
                pushElementInListLine(List_line, word);
                word = strtok ( NULL,space );
            }
        pushListInListProgram(List_prg, List_line);
        num_line++;
    }
    rewind(f);
    int fclose(FILE *f);

    //ETAPE 2 : On parcourt la liste, on effectue les conversions necessaires et on remplit hexa.txt

    FILE* fHexa = NULL;
    fHexa = fopen("hexa.txt", "w");
    FileName="hexa.txt";
    if (fHexa==NULL){
        printf("Impossible d ouvrir le fichier. Execution impossible \n");
        return 0;
    }
    long tabBinary[32];
    TabInit(tabBinary,32);

    char tabHexa[8];
    TabHexaInit(tabHexa,8);


    long i=0;
    long nbLines=List_prg->length;
    ListLine* list_line=List_prg->first;
    while(i<nbLines){
        TabInit(tabBinary,32);
        Element * elem=GetElement(list_line,0);
        long ValCodeOp = GetValDecimalCodeOp(elem->val,i);
        Conv_Binary(tabBinary, ValCodeOp, 0, 4);

        //Cas des instructions arithmetiques et logiques
        if (ValCodeOp>=0 && ValCodeOp<=7) ConvArithmetic(elem,tabBinary,i);

        //Cas des instructions de transfert (ld)
        else if(ValCodeOp>=8 && ValCodeOp<=10) ConvTransferLd(elem,tabBinary,i);

        //Cas des instructions de transfert (st)
        else if(ValCodeOp>=11 && ValCodeOp<=13) ConvTransferSt(elem,tabBinary,i);

        //Cas des instructions de saut : on code sur les 16 derniers bits
        else if(ValCodeOp>=14 && ValCodeOp<=20) ConvJump(elem,tabBinary,listLabel,i);

        //Cas des instructions d'entree sortie
        else if(ValCodeOp == 21 || ValCodeOp==22)  ConvES(elem,tabBinary,i);

        //rnd : meme fonctionnement que les instructions arithmetiques
        else if(ValCodeOp == 23) {
            ConvArithmetic(elem,tabBinary,i);
        }
        //hlt
        else{
            //On cherche une erreur eventuelle dans le texte = si il y a un autre mot
            if(elem->next !=NULL){
                printf("Erreur en ligne %ld: l'instruction hlt s'utilise seule. Execution impossible \n",i+1);
                remove(FileName);
                exit(-1);
            }
        }
        Conv_Hexa(tabBinary,tabHexa);
        FillInFile(fHexa,tabHexa);
        i++;
        list_line=list_line->next;

    }
    rewind(fHexa);
    int fclose(FILE* fHexa);


    //PARTIE 2 :

    PC=0;
    N=0;
    Z=0;
    C=0;

    srandom(time(NULL));
    fHexa=fopen("hexa.txt","r");
    if(fHexa==NULL){
        printf("Impossible d'ouvrir le fichier texte. Execution impossible \n");
        return 0;
    }
    nbLines=GetNumLines(fHexa);
    rewind(fHexa);

    //etape 2: on initialise un tableau de nbLines tableaux on reouvre le fichier

    //Allocation mémoire
    long **tabBin=malloc(sizeof(tabBin)*nbLines);
    for(long i=0; i<nbLines; i++){
        tabBin[i]=malloc(sizeof(**tabBin)*32);
    }

    long* r=malloc(32*sizeof(long)+1);
    //si je veux acceder a r1 (langague assembleur)  ---> r[1] (C)
    tabRegisterInit(r);

    long cpt=0;
    while (fgets(line, sizeof(line), fHexa)){
        if(cpt>nbLines) break;
        FillInMemory(line,cpt);
        delete_caractere(line,"\r\n.\r");
        FillInTabBinary(line, tabBin[cpt],cpt,8);
        cpt++;
    }

    int fclose(FILE* fHexa);

    while(PC!=-1 && PC<4*nbLines+1){

        Execution(tabBin,PC/4,r);
        //r0 doit toujours valoir 0 (donc au cas ou on l a modifie, on lui redonne sa valeur initiale a la fin de l execution de l instruction)
        r[0]=0;
    }

    free(r);
    for(long i=0; i<nbLines; i++) free(tabBin[i]);
    free(tabBin);
    FreeListProgram(List_prg);
    FreeLabel(listLabel);
    printf("Fin du programme \n");

    return 0;
}
