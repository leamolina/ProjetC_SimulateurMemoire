#include "Conversions.c"
#define T_MAX 256


//Conversion d une ligne contenant une instruction arithmetique
void ConvArithmetic(Element* elem,long tabBinary[], long i){
    elem=elem->next; //Recupere rd
    long reg=-1;
    if(IsRegister(elem->val)){
        long reg=GetValRegister(elem->val);
        Conv_Binary(tabBinary,reg,5,9);
    }
    else{
        printf("Erreur en ligne %ld. Le premier 'registre' n'en est pas un. Execution impossible \n",i+1);
        remove(FileName);
        exit(-1);
    }

    elem=elem->next; //Recupere rn
    if(IsRegister(elem->val)){
        reg=GetValRegister(elem->val);
        Conv_Binary(tabBinary,reg,10,14);
    }
    else{
        printf("Erreur en ligne %ld. Le second 'registre' n'en est pas un. Execution impossible \n",i+1);
        remove(FileName);
        exit(-1);
    }


    elem=elem->next; //Recupere Src2 : soit #n soit rn soit #hn
    FillInTabSrc2(elem->val,tabBinary,i);

    if(elem->next!=NULL){
        printf("Erreur en ligne %ld. Il a y a au moins un mot en trop a la fin. Execution impossible \n",i+1);
        remove(FileName);
        exit(-1);
    }
}
//Conversion d une ligne contenant une instruction de saut
void ConvJump(Element* elem, long tabBinary[],ListLabel* listLabel,long i){
    //On recupere le deuxieme mot qui est le nom d'une etiquette
    elem=elem->next;
    if(IsRegister(elem->val)){
        tabBinary[15]=0;
        long reg=GetValRegister(elem->val);
        Conv_Binary(tabBinary,reg,16,31);
        return;
    }
    char *NameLabel=elem->val;
    long Address=AddressLabel(listLabel, NameLabel);

    if(Address==-1) {
        printf("Erreur en ligne %ld : la valeur '%s' n'est ni un registre, ni une etiquette presente dans le programme. Execution impossible \n",i+1, elem->val);
        remove(FileName);
        exit(-1);
    }
    else{
        tabBinary[15]=1;
        Conv_Binary(tabBinary, Address, 16, 31);
    }
    if(elem->next!=NULL){
        printf("Erreur en ligne %ld: Il a y a au moins un mot en trop a la fin. Execution impossible \n",i+1);
        remove(FileName);
        exit(-1);
    }
}
//Conversion d une ligne contenant une instruction d'entree/sortie
void ConvES(Element* elem, long tabBinary[], long i){
    elem=elem->next; //Recupere rd
    long nb=GetValRegister(elem->val);
    Conv_Binary(tabBinary,nb,5,9);
    if(elem->next!=NULL){
        printf("Erreur en ligne %ld: Il a y a au moins un mot en trop a la fin. Execution impossible \n",i+1);
        remove(FileName);
        exit(-1);
    }
}
//Conversion d une ligne contenant une instruction de transfert (load)
void ConvTransferLd(Element* elem, long tabBinary[], long i){
    //rd
    elem=elem->next;
    long reg=GetValRegister(elem->val);
    Conv_Binary(tabBinary,reg,5,9);

    //(rn)S
    elem=elem->next;
    char* parenthesis="()";
    char* new=malloc(10*sizeof(char));
    strcpy(new,elem->val);
    if(strstr(new,"(")==NULL || strstr(new,")")==NULL){
       printf("Erreur en ligne %ld: Oubli des parentheses. Execution impossible \n",i+1);
       remove(FileName);
       exit(-1);
    }
    char* s=strtok(new,parenthesis);
    //rn
    long reg_rn=GetValRegister(s);
    Conv_Binary(tabBinary,reg_rn,10,14);

    //S
    s=strtok(new,parenthesis);
    if (IsRegister(s)){
        tabBinary[15]=0;
        long reg=GetValRegister(s);
        Conv_Binary(tabBinary,reg,16,31);
    }
    else if(s[0]=='#'){
        tabBinary[15]=1;
        //Valeur hexadecimale
        if(s[1]=='h'){
            delete_caractere(s,"#h");
            long n=hexa2long(s);
            Conv_Binary(tabBinary,n,16,31);
        }

        //Valeur decimale
        else{
            long n=GetValImediate(s);
            Conv_Binary(tabBinary,n, 16, 31);
        }
    }

    else{
        printf("Erreur en ligne %ld: S n est ni un registre, ni une valeur imédiate. Execution impossible \n",i+1);
        remove(FileName);
        exit(-1);
    }

    if(elem->next!=NULL){
        printf("Erreur en ligne %ld: Il a y a au moins un mot en trop a la fin. Execution impossible \n",i+1);
        remove(FileName);
        exit(-1);
    }
    free(new);

}
//Conversion d une ligne contenant une instruction de transfert (store)
void ConvTransferSt(Element* elem, long tabBinary[], long i){
    long reg_rd=0;
    //(rd)S
    elem=elem->next;
    char* parenthesis="()";
    char* new=malloc(10*sizeof(char));
    strcpy(new,elem->val);
    if(strstr(new,"(")==NULL || strstr(new,")")==NULL){
       printf("Erreur en ligne %ld: Oubli des parentheses. Execution impossible \n",i+1);
       remove(FileName);
       exit(-1);
    }
    char* s=strtok(new,parenthesis);
    //rd
    if(IsRegister(s)){
        long reg_rd=GetValRegister(s);
        Conv_Binary(tabBinary,reg_rd,5,9);
    }
    else{
        printf("Erreur en ligne %ld : le deuxieme mot est cense etre un registre. Execution impossible \n",i+1);
        remove(FileName);
        exit(-1);
    }

    //S
    s=strtok(new,parenthesis);
    if(s==NULL){
        printf("Erreur en ligne %ld: Execution impossible \n",i+1);
        remove(FileName);
        exit(-1);
    }
    if (IsRegister(s)){
        tabBinary[15]=0;
        long reg=GetValRegister(s);
        Conv_Binary(tabBinary,reg,16,31);
    }

    else if(s[0]=='#'){
        tabBinary[15]=1;
        //Valeur hexadecimale
        if(s[1]=='h'){
            delete_caractere(s,"#h");
            long n=hexa2long(s);
            Conv_Binary(tabBinary,n+reg_rd,16,31);
        }

        //Valeur decimale
        else{
            long n=GetValImediate(s);
            Conv_Binary(tabBinary,n+reg_rd, 16, 31);
        }
    }

    else{
        printf("Erreur en ligne %ld: S n est ni un registre, ni une valeur imédiate. Execution impossible \n",i+1);
        remove(FileName);
        exit(-1);
    }
    //rn
    elem=elem->next;
    long reg=GetValRegister(elem->val);
    Conv_Binary(tabBinary,reg,10,14);

    if(elem->next!=NULL){
        printf("Erreur en ligne %ld: Il a y a au moins un mot en trop a la fin. Execution impossible \n",i+1);
        remove(FileName);
        exit(-1);
    }
    free(new);
}
