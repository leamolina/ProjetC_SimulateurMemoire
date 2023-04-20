#include "Divers.c"

void TabInit(long tab[], long n){
    for(long i=0; i<n; i++) tab[i]=0;
}

void DisplayTabInt(long tab[], long n){
    printf("Affichage tab binaire : \n");
    for(long i=0; i<n; i++){
        printf("%lu  ",tab[i]);
    }
    printf("\n");
}

void TabHexaInit(char tab[], long n){
    for(long i=0; i<n; i++) tab[i]='0';
}

void DisplayTabHexa(char tab[], long n){
    printf("Affichage tab hexa : \n");
    for(long i=0; i<n; i++){
        printf("%c \t", tab[i]);
    }
}
//Converti une chaine de caractere en long
long Integer(char *s){
    char** end = NULL;
    long base=10;
    return strtol(s,end,base);
}


//Renvoie la valeur hexadecimale d un entier (compris entre 0 et 15)
char HexaChar(long n){
    char tmp;
    if(n>=0 && n<=9) tmp=n+48;
    else tmp=n+55;
    return(tmp);
}

//Recupere la valeur decimale du code operatoire (et verifie s il y a une erreur)
long GetValDecimalCodeOp(char* word, long j){
    char * TabCodeOp[]={"and", "or", "xor", "mul", "div", "add", "sub", "shr", "ldb", "ldh", "ldw", "stb", "sth", "stw", "jmp", "jzs", "jzc", "jcs", "jcc", "jns", "jnc", "in", "out", "rnd", "hlt"};
    long i;
    for (i=0; i<=24; i++){
        if ((strcmp(word,TabCodeOp[i]))==0){
            if(i==24) return 31;
            else return i;
        }
    }
    printf("Erreur en ligne %ld : %s n'est pas un code operatoire valide. Execution impossible \n",j+1,word);
    remove(FileName);
    exit(-1);
    return -1;
}

//Renvoie 1 si le caractere est bien un chiffre, 0 sinon
long IsDigit(char c){
    //long n=atoi(c);
    if(c>='0' && c<='9') return 1;
    return 0;
}


long hexa2long(char *str) {
    long val = 0;
    long index = 0;
    char c;
    while ((c = str[index++])) {
        if (IsDigit(c))
            val = 16*val + (c - '0');
        else
            val = 16*val + (c - 'A' + 10);
    }
    return val;
}

//traduit une valeur decimale en binaire
void Conv_Binary(long tabBinary[], long nb, long indexStart, long indexEnd){
	long i;
    if(nb<0){
        long length=indexEnd-indexStart+1;
        nb=Power(2,length)-labs(nb);
    }
	for (i=indexEnd; i>=indexStart; i--){
		tabBinary[i]= nb%2;
		nb= nb/2;
    }
}

//Renvoie 1 si le nombre est un registre, 0 sinon
long IsRegister(char *c){
    if(strlen(c)==2 || strlen(c)==3){
        if(c[0]=='r'){
            //Recuperation de la valeur (sans le 'r')
            //char* p=strchr(c,'r');
            //p++;
            c++;
            char** end=NULL;
            long n=strtol(c,end,10);
            if(n>=0 && n<=31){
                return 1;
            }
        }
    }
    return 0;

}

//traduit une valeur binaire a hexadecimale
void Conv_Hexa(long tabBinary[], char tabHexa[]){

    long tab[4];
    long i=31;
    long k=7;

    while(i>=0){
        long cpt=3;

        //On rempli un sous tableau
        while(cpt>=0){
            tab[cpt]=tabBinary[i];
            i--;
            cpt--;
        }
        //On recupere le nombre associe
        long nb=(1*tab[3])+(2*tab[2])+(4*tab[1])+(8*tab[0]);

        tabHexa[k]=HexaChar(nb);
        k--;
    }

}
//Renvoie l indice du registre
long GetValRegister(char* rn){
    char*tmp=strtok(rn,"r");
    return Integer(tmp);
}
//Recupere une valeur immediate (decimale)
long GetValImediate(char* rn){
    char*tmp=strtok(rn,"#");
    return Integer(tmp);
}

//Prend un champs Src2 et verifie si c est une valeur Imediate ou non; rempli tabBinary
void FillInTabSrc2(char* src2,long tabBinary[],long i){
    long n;
    //Verifie le type de Src2

    if(src2[0]=='#'){
        tabBinary[15]=1;

        //Valeur hexadecimale
        if(src2[1]=='h'){
            delete_caractere(src2,"#h");
            n=hexa2long(src2);
            Conv_Binary(tabBinary,n,16,31);
        }

        //Valeur decimale : pour le faire en complement a 2, il
        else{
            n=GetValImediate(src2);
            if(n>=0){
                Conv_Binary(tabBinary, n, 17, 31);
                tabBinary[16]=0;
            }
            else{
                //long complement2=labs(n);
                //Conv_Binary(tabBinaire,complement2,17,31);
                //tabBinaire[16]=1;
                long complement2=Power(2,16)+n;
                Conv_Binary(tabBinary,complement2,16,31);
            }
        }
    }

    //Registre
    else if(src2[0]=='r'){
        if(IsRegister(src2)){
            tabBinary[15]=0;
            n=GetValRegister(src2);
            Conv_Binary(tabBinary,n,16,31);
        }
        else{
        printf("Erreur en ligne %ld. Le dernier 'registre' n'en est pas un. Execution impossible \n",i+1);
        remove(FileName);
        exit(-1);
        }
    }

    //Ni l'un ni l'autre : erreur
    else{
        printf("Erreur en ligne %ld : le dernier mot n'est ni une valeur immediate, ni un registre. Execution impossible \n",i+1);

    }
}

//on aura au prealable initialise ch (c est lui qui va contenir la traduction hexa)
//ATTENTION : cette instruction ne s utilise QUE pour traduire 1 octet en hexadecimal
void Hexa(long n, char* ch){
    long i=1;
    long nb=n;
    while(nb!=0){
        ch[i]=HexaChar(nb%16);
        nb=nb/16;
        i--;
    }

}

//Prend une ligne contenant un code hexa et rempli dans le sous tableau correspondant a la ligne
void FillInTabBinary(char* line, long tab[], long index, long length){
    for(long i=0; i<length; i++){
        char c=line[i];
        long nb=-1;
        //Recuperation du long associe
        if(c>='0' && c<='9') nb=c-'0';
        else if(c>='A' && c<='F') nb=c-'A'+10;
        else{
            printf("Erreur, %c n est pas une valeur hexadecimale. Execution impossible. \n",c);
            exit(-1);
        }
        Conv_Binary(tab,nb,4*i,4*(i+1)-1);
    }
}
