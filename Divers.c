#include "structure_programme.c"

//Supprime un caractere d une chaine de caractere
void delete_caractere(char *t, char *c){
    while((t = strpbrk(t, c)) != NULL){
        memmove(t, t + 1, strlen(t));
    }
}
//Rempli le fichier hexa
void FillInFile(FILE* fHexa, char tabHexa[]){
    for(long i=0; i<8; i++){
        long n=fputc(tabHexa[i],fHexa);
        if (n==EOF){
            printf("Erreur : impossible de remplir le fichier correctement. Arret de l'execution. \n");
            return;
        }
    }
    long n=fputc(10,fHexa);
    if (n==EOF){
            printf("Erreur : impossible de remplir le fichier correctement. Arret de l'execution. \n");
            return;
    }
}

//calcule x puissance y
long Power(long x, long y){
    if(x==0 && y==0) return 1;
    else if(y==0) return 1;
    else return x*Power(x,y-1);
}

//Rempli le tableau de memoire en fonction de sa position
void FillInMemory(char* line, long i){
    long k=4*i;
    while(k<4*(i+1)){
        long j=0;
        while(j<8){
            memory[k][0]=line[j];
            memory[k][1]=line[j+1];
            j+=2;
            k++;
        }
    }
}

//Affiche un tableau de tableaux binaires (length 32)
void DisplayTabTab(long** tab, long n){
    for(long i=0; i<n; i++){
        printf("Affichage sous tableau : \n");
        for(long j=0; j<32; j++){
            printf("%ld \t",tab[i][j]);
        }
        printf("\n \n");
    }
}
//Recupere le nombre de lignes d un fichier
long GetNumLines(FILE* f){
    long cpt=0;
    char line[10];
    while (fgets(line, sizeof(line), f)) {
        cpt++;
    }
    return cpt;
}
