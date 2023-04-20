#include "ConversionsInstructions.c"

long BinaryToLong(long binary[], long length){
    long sum=0;
    long i=0;
    while(i<length){
        sum+=binary[i]*(Power(2,(length-i-1)));
        i++;
    }

    //Cas des nombres positifs
    if(binary[0]==0) return sum;

    //Cas des nombres negatifs (representes en complement a 2)
    else return (sum - Power(2,length));
}

//On donne par defaut la valeur 0 a tous les registres
void tabRegisterInit(long* r){
    for (long i=0; i<32; i++){
        r[i]=0;
    }
}

long ReturnCodeOp(long **tabBinary, long j){
    return 16*tabBinary[j][0] + 8*tabBinary[j][1] + 4*tabBinary[j][2] + 2*tabBinary[j][3] + tabBinary[j][4];
}

long ReturnDest(long **tabBinary, long j){
    return 16*tabBinary[j][5] + 8*tabBinary[j][6] + 4*tabBinary[j][7] + 2*tabBinary[j][8] + tabBinary[j][9];
}

long ReturnSrc1(long **tabBinary, long j){
    return 16*tabBinary[j][10] + 8*tabBinary[j][11] + 4*tabBinary[j][12] + 2*tabBinary[j][13] + tabBinary[j][14];
}

long ReturnSrc2(long **tabBinary, long j){
    long sum=0;
    long i=16;
    while(i<=31){
        sum+=tabBinary[j][i]*(Power(2,(31-i)));
        i++;
    }
    //Cas des nombres positifs
    if (tabBinary[j][16]==0) return sum;

    //Cas des nombres negatifs
    else return (sum - Power(2,16));
}



//Fonction que l on va appeler a la fin de chaque instruction qui affecte le registre d etat (uniquement Z et N)
void ModifRegistreDEtat(long* r, long d){
    //Modification de Z:
    if (r[d]==0) Z=1;
    else Z=0;
    //Modification de N:
    if(r[d]<0) N=1;
    else N=0;
}

void And(long* r, long d, long n, long S, long ValImmediate){
    if(ValImmediate==1) r[d]=r[n]&S;
    else r[d]=r[n]&r[S];
    C=0;
    ModifRegistreDEtat(r,d);
    PC+=4;
}

void Or(long* r, long d, long n, long S, long ValImmediate){
    if(ValImmediate==1) r[d]=r[n]|S;
    else r[d]=r[n]|r[S];
    C=0;
    ModifRegistreDEtat(r,d);
    PC+=4;
}

void Xor(long* r, long d, long n, long S, long ValImmediate){
    if(ValImmediate==1) r[d]=r[n]^S;
    else r[d]=r[n]^r[S];
    C=0;
    ModifRegistreDEtat(r,d);
    PC+=4;
}

void Mul(long* r, long d, long n, long S, long ValImmediate){
    long rsTmp;
    //Etape 1 : On recupere les 16 bits de poids faibles de RN, ceux de S,
    //Etape 1.1 : On recupere la valeur de S
    long x;
    if(ValImmediate==1) x=S;
    else x=r[S];

    //Etape 1.2 : on recupere les 32 bits de RN, et on cree un tableau temporaire qui va contenir uniquement les 16 bits de poids faible de RN
    long RN[32];
    Conv_Binary(RN,r[n],0,31);
    long RNtmp[16];
    for(long i=31; i>=16; i--){
        RNtmp[i-16]=RN[i];
    }
    //Etape 1.3 : on recupere les 32 bits de S et on cree un tableau temporaire qui va contenir uniquement les 16 bits de poids faible de S
    //On distingue 2 cas: si S est une valeur immediate, elle est deja codee sur 16 bits; si cest un registre, il faut refaire la meme chose que l etape 1.2
    long RStmp[16];
    if(ValImmediate==1){
        //Conv_Binary(RStmp,S,0,15);
        rsTmp=labs(S);
    }
    else{
        long RS[32];
        Conv_Binary(RS,r[S],0,31);
        for(long i=31; i>=16; i--){
            RStmp[i-16]=RS[i];
        }
    }

    //Etape 2: on les converti en decimal (sans prendre compte des signes) et on les multiplie (c est deja fait pour rsTmp dans le cas ou S est une valeur immediate)
    //Etape 2.1 : conversion de RNtmp et de RStmp dans rnTmp et rsTmp
    long rnTmp=0;
    if (ValImmediate==0) rsTmp=0;
    long i=0;
    while(i<16){
        rnTmp+=RNtmp[i]*(Power(2,(16-i-1)));
        if(ValImmediate==0) rsTmp+=RStmp[i]*(Power(2,(16-i-1)));
        i++;
    }

    //Etape 2.2 : multiplication des deux
    long result=rnTmp*rsTmp;

    //Etape 3: on recopie le resultat dans RD en modifiant le bit de signe en fonction des signes de r[n] et S
    if(  (r[n]>=0 && x>=0)  || (r[n]<0 && x<0)  ) {
        r[d]=result;
    }
    else {
        r[d]=(-1)*result;
    }
    ModifRegistreDEtat(r,d);
    PC+=4;
}


void Div(long* r, long d, long n, long S, long ValImmediate){
    //On recupere la valeur de S
    long x;
    if(ValImmediate==1) x=S;
    else x=r[S];

    r[d]=r[n]/x;

    //On met le registre de retenue a 0
    C=0;

    ModifRegistreDEtat(r,d);
    PC+=4;
}


void Add(long* r, long d, long n, long S, long ValImmediate){

    //Etape 1: on met r[n] et S en binaire sur 32 bits
    long RN[32];
    Conv_Binary(RN,r[n],0,31);

    long RS[32];
    //Si S est une valeur immediate, on recopie les bits de poids faible et on repete le bit de signe dans les 16 bits de poids fort
    if(ValImmediate==1){
        long RStmp[16];
        Conv_Binary(RStmp,S,0,15);
        for(long i=31; i>=16; i--){
            RS[i]=RStmp[i-16];
        }
        //On copie le bit de poids fort dans ce qu il reste
        for(long i=0; i<16; i++){
            RS[i]=RStmp[0];
        }


    }
    //Si S est un registre, alors on sait qu il est code sur 32 bits, donc pas de recopie du bit de poids fort
    else Conv_Binary(RS,r[S],0,31);


    //RD = la ou sera stocke le resultat de l addition
    long RD[32];

    //Etape 2 : on effectue l addition bit a bit (avec les retenues)
    long carry=0;
    long i=31;
    long result;
    while(i>=0){
        result=RN[i]+RS[i]+carry;
        if(result==0 || result==1) {
            carry=0;
            RD[i]=result;
        }
        else if(result==2){
            carry=1;
            RD[i]=0;
        }
        //Cas ou il y a 1+1+1
        else{
            carry=1;
            RD[i]=1;
        }
        i--;
    }

    //Etape 3: on recupere les bits de RD et on les met dans r[d]
    r[d]=BinaryToLong(RD,32);

    //S il y a une retenue, on met le registre C a 1; 0 sinon
    if(carry==0) C=0;
    else C=1;
    ModifRegistreDEtat(r,d);
    PC+=4;
}

void Sub(long* r, long d, long n, long S, long ValImmediate){

    //Etape 1: on met r[n] et S en binaire sur 32 bits
    long RN[32];
    Conv_Binary(RN,r[n],0,31);

    long RS[32];
    //Si S est une valeur immediate, on recopie les bits de poids faible et on repete le bit de signe dans les 16 bits de poids fort
    if(ValImmediate==1){
        long RStmp[16];
        Conv_Binary(RStmp,S,0,15);
        for(long i=31; i>=16; i--){
            RS[i]=RStmp[i-16];
        }
        //On copie le bit de poids fort dans ce qu il reste
        for(long i=0; i<16; i++){
            RS[i]=RStmp[0];
        }
    }
    //Si S est un registre, alors on sait qu il est code sur 32 bits, donc pas de recopie du bit de poids fort
    else Conv_Binary(RS,r[S],0,31);


    //RD = la ou sera stocke le resultat de l addition
    long RD[32];

    //Etape 2 : on effectue l addition bit a bit (avec les retenues)
    long carry=0;
    long i=31;
    long result;
    while(i>=0){
        result=RN[i]-RS[i]-carry;
        if(result==0 || result==1) {
            carry=0;
            RD[i]=result;
        }
        //Cas ou il y a 1-1-1 : 0 de resultat et 1 de retenue
        else if(result==-1){
            carry=1;
            RD[i]=1;
        }
        //Cas ou il y a 0-1-1: 0 de resultat et 1 de retenue
        else{
            carry=1;
            RD[i]=0;
        }
        i--;
    }

    //Etape 3: on recupere les bits de RD et on les met dans r[d]
    r[d]=BinaryToLong(RD,32);

    //S il y a une retenue, on met le registre C a 1; 0 sinon
    if(carry==0) C=0;
    else C=1;
    ModifRegistreDEtat(r,d);
    PC+=4;
}



void Shr(long* r, long d, long n, long S, long ValImmediate){
    //On recupere la valeur de S
    long x;
    if(ValImmediate==1) x=S;
    else x=r[S];

    //On recupere la valeur de r[n] en binaire (pour pouvoir determiner sa retenue C)
    long RN[32];
    Conv_Binary(RN,r[n],0,31);


    //Decallage en fonction du signe de S
    if(x>=0){
        r[d]=r[n]>>x;
        C=RN[32-x];
    }
    else{
        r[d]=r[n]<<labs(x);
        C=RN[labs(x)-1];
    }

    ModifRegistreDEtat(r,d);
    PC+=4;
}


void Ldb(long* r, long d, long n, long S, long ValImmediate){
    long x;
    if(ValImmediate==1) x=S;
    else x=r[S];

    //On convertit le registre en un tabBinary code sur 4 octets = 32 bits
    long RD[32];
    Conv_Binary(RD,r[d],0,31);


    //On convertit aussi le S (ou rS) en un tabBinary code sur 1 octet = 8 bits
    long RS[8];
    long valeur=hexa2long(memory[r[n]+x]);

    //On recupere le signe de valeur
    long sign;
    if(valeur>=0) sign=0;
    else sign=1;
    Conv_Binary(RS,valeur,0,7);

    //Maintenant on peut travailler sur les bits
    //Etape 1: on copie le bit de poids fort (sign) sur tous les indices de RD allant de 0 a 24 inclus
    for(long i=0; i<=24; i++){
        RD[i]=sign;
    }

    //Etape 2 : on copie le reste de RS dans ce qu il reste de R[D]
    long j=7                                                                                                                                                                                         ;
    while(j>=0){
        RD[24+j]=RS[j];
        j--;
    }


    //Etape 3: on recupere les bits de RD et on les met dans r[d]
    r[d]=BinaryToLong(RD,32);
    PC+=4;
}


void Ldh(long* r, long d, long n, long S, long ValImmediate){
    long x;
    if(ValImmediate==1) x=S;
    else x=r[S];

    //On convertit le registre en un tabBinary code sur 4 octets = 32 bits
    long RD[32];
    Conv_Binary(RD,r[d],0,31);

    //On convertit aussi le S (ou rS) en tabBinary code sur 2 octets = 16 bits
    long RS[16];
    long valeur=hexa2long(memory[r[n]+x]);
    Conv_Binary(RS,valeur,0,7);
    valeur=hexa2long(memory[r[n]+x+1]);
    //On recupere le signe de valeur
    long sign;
    if(valeur>=0) sign=0;
    else sign=1;
    Conv_Binary(RS,valeur,8,15);

    //Maintenant on peut travailler sur les bits
    //Etape 1: on copie le bit de poids fort (sign pour tous les indices de RD allant de 0 a 15 inclus)
    for(long i=0; i<=15; i++){
        RD[i]=sign;
    }

    //Etape 2 : on copie le reste de RS dans ce qu il reste de R[D]
    long j=15;
    while(j>=0){
        RD[j+16]=RS[j];
        j--;
    }

    //Etape 3 : On recupere les bits de RD et on les met dans r[d]
    r[d]=BinaryToLong(RD,32);
    PC+=4;

}

void Ldw(long* r, long d, long n, long S, long ValImmediate){
    long x;
    if(ValImmediate==1) x=S;
    else x=r[S];

    //On convertit le registre en un tabBinary code sur 4 octets = 32 bits
    long RD[32];
    Conv_Binary(RD,r[d],0,31);

    //On convertit aussi le S (ou rS) en tabBinary code sur 4 octets = 32 bits
    long RS[32];
    long valeur=hexa2long(memory[r[n]+x]);
    Conv_Binary(RS,valeur,0,7);
    valeur=hexa2long(memory[r[n]+x+1]);
    Conv_Binary(RS,valeur,8,15);
    valeur=hexa2long(memory[r[n]+x+2]);
    Conv_Binary(RS,valeur,16,31);


    //Etape 1 : on copie le reste RS dans RD
    long j=0;
    while(j<=31){
        RD[j]=RS[j];
        j++;
    }

    //Etape 3 : On recupere les bits de RD et on les met dans r[d]
    r[d]=BinaryToLong(RD,32);
    PC+=4;
}


void Stb(long* r, int d, int n, int S, int ValImmediate){

    long x;
    if(ValImmediate==1) x=S;
    else x=r[S];

    //On convertit ce qui est stocke a l adresse (rd+S) sur 4 octets = 32 bits
    long MEM[8];
    long valeur=hexa2long(memory[r[d]+x]);
    Conv_Binary(MEM,valeur,0,7);

    //On convertit maintenant rn sur 1 octet = 8 bits
    long RNtmp[32];
    Conv_Binary(RNtmp,r[n],0,31);
    //On recupere les 8 premiers bits de RNtmp dans RN
    long RN[8];
    for(long i=0; i<=7; i++){
        RN[i]=RNtmp[24+i];
    }

    //Mainenant on peut travailler sur les bits
    //Etape 1 : on recopie bit a bit la valeur de RN dans MEM
    for (long i=0; i<=7; i++){
        MEM[i]=RN[i];
    }
    //Etape3: on recupere les bits de MEM et on les met dans memoire[r[d]+x]
    long nb=BinaryToLong(MEM,8);
    Hexa(nb,memory[r[d]+x]);
    PC+=4;

}



void Sth(long* r, int d, int n, int S, int ValImmediate){

    long x;
    if(ValImmediate==1) x=S;
    else x=r[S];

    //On convertit ce qui est stocke a l adresse (rd+S) sur 4 octets = 32 bits
    long MEM[8];
    long valeur=hexa2long(memory[r[d]+x]);
    Conv_Binary(MEM,valeur,0,7);

    //On convertit maintenant rn sur 2 octet = 16 bits
    long RNtmp[32];
    Conv_Binary(RNtmp,r[n],0,31);
    //on recupere les 16 premiers bits de RNtmp dans RN
    long RN[16];
    for(long i=0; i<=15; i++){
        RN[i]=RNtmp[16+i];
    }

    //Mainenant on peut travailler sur les bits
    //Etape 1 : on recopie bit a bit la valeur de RN (indices de 0 a 7) dans la memoire
    for (long i=0; i<=7; i++){
        MEM[i]=RN[i];
    }
    long nb=BinaryToLong(MEM,8);
    Hexa(nb,memory[r[d]+x]);

    //Etape 2 : on recopie bit a bit la valeur de RN (indices de 8 a 16) dans la case suivante de la memoire
    for (long i=8; i<=15; i++){
        MEM[i-8]=RN[i];
    }
    nb=BinaryToLong(MEM,8);
    Hexa(nb,memory[r[d]+x+1]);
    PC+=4;

}


void Stw(long* r, int d, int n, int S, int ValImmediate){

    long x;
    if(ValImmediate==1) x=S;
    else x=r[S];

    //On convertit ce qui est stocke a l adresse (rd+S) sur 4 octets = 32 bits
    long MEM[8];
    long valeur=hexa2long(memory[r[d]+x]);
    Conv_Binary(MEM,valeur,0,7);

    //On convertit maintenant rn sur 4 octet = 32 bits
    long RN[32];
    Conv_Binary(RN,r[n],0,31);

    //Mainenant on peut travailler sur les bits
    //Etape 1 : on recopie bit a bit la valeur de RN (indices de 0 a 7) dans la memoire
    for (long i=0; i<=7; i++){
        MEM[i]=RN[i];
    }
    long nb=BinaryToLong(MEM,8);
    Hexa(nb,memory[r[d]+x]);

    //Etape 2 : on recopie bit a bit la valeur de RN (indices de 8 a 16) dans la case suivante de la memoire
    for (long i=8; i<=15; i++){
        MEM[i-8]=RN[i];
    }
    nb=BinaryToLong(MEM,8);
    Hexa(nb,memory[r[d]+x+1]);

    //Etape 3: on recopie bit a bit la valeur de RN (indices de 16 a 23) dans la case memoire suivante
    for (long i=16; i<=23; i++){
        MEM[i-16]=RN[i];
    }
    nb=BinaryToLong(MEM,8);
    Hexa(nb,memory[r[d]+x+2]);

    //Etape 4 : on recopie bit a bit la valeur de RN (indices de 24 a 31) dans la case memoire suivante
    for (long i=24; i<=31; i++){
        MEM[i-24]=RN[i];
    }
    nb=BinaryToLong(MEM,8);
    Hexa(nb,memory[r[d]+x+3]);
    PC+=4;

}


void Jmp(long* r, long S, long ValImmediate){
    if(ValImmediate==1) PC=S;
    else PC=r[S];
    C=0;
}

void Jzs(long* r, long S, long ValImmediate){
    if(Z==1){
        if(ValImmediate==1){
            PC=S;
        }
        else PC=r[S];
    }
    else PC+=4;
    C=0;
}

void Jzc(long* r, long S, long ValImmediate){
    if(Z==0){
        if(ValImmediate==1) PC=S;
        else PC=r[S];
    }
    else PC+=4;
    C=0;
}

void Jcs(long* r, long S, long ValImmediate){
    if(C==1){
        if(ValImmediate==1) PC=S;
        else PC=r[S];
    }
    else PC+=4;
    C=0;
}

void Jcc(long* r, long S, long ValImmediate){
    if(C==0){
        if(ValImmediate==1) PC=S;
        else PC=r[S];
    }
    else PC+=4;
    C=0;
}

void Jns(long* r, long S, long ValImmediate){
    if(N==1){
        if(ValImmediate==1) PC=S;
        else PC=r[S];
    }
    else PC+=4;
    C=0;
}

void Jnc(long* r, long S, long ValImmediate){
    if(N==0){
        if(ValImmediate==1) PC=S;
        else PC=r[S];
    }
    else PC+=4;
    C=0;
}

void In(long* r,long d){
    long nb;
    printf("Entrer la valeur de r%ld : ",d);
    scanf("%ld",&nb);
    r[d]=nb;
    ModifRegistreDEtat(r,d);
    PC+=4;
}


void Out(long* r, long d){
    printf("r%ld = %ld\n",d,r[d]);
    ModifRegistreDEtat(r,d);
    PC+=4;
}

void Rnd(long* r, long d, long n, long S, long ValImmediate){
    long x, min, alea;

    if(ValImmediate==1) x=S-1;
    else x=r[S]-1;
    //Etape 1 : on recupere le plus petit des deux nombres
    if(x>r[n]) min=r[n];
    else min=x;
    //Etape 2: on recupere la difference (en valeur absolue) des deux nombres
    long diff=labs(x-r[n]);

    //Etape 3 : on affecte a r[d] la valeur aleatoire comprise entre r[n] et S-1 inclus
    alea=random()%(diff+1);
    r[d]= alea+min;
    PC+=4;
}

void Execution(long ** tabBinary, long j, long*r){
    long d,n,S,ValImmediate;
    long codeop=ReturnCodeOp(tabBinary,j);
    switch(codeop){
        case 0:
            d=ReturnDest(tabBinary,j);
            n=ReturnSrc1(tabBinary,j);
            S=ReturnSrc2(tabBinary,j);
            ValImmediate=tabBinary[j][15];
            And(r,d,n,S,ValImmediate);
            break;
        case 1:
            d=ReturnDest(tabBinary,j);
            n=ReturnSrc1(tabBinary,j);
            S=ReturnSrc2(tabBinary,j);
            ValImmediate=tabBinary[j][15];
            Or(r,d,n,S,ValImmediate);
            break;
        case 2:
            d=ReturnDest(tabBinary,j);
            n=ReturnSrc1(tabBinary,j);
            S=ReturnSrc2(tabBinary,j);
            ValImmediate=tabBinary[j][15];
            Xor(r,d,n,S,ValImmediate);
            break;
        case 3:
            d=ReturnDest(tabBinary,j);
            n=ReturnSrc1(tabBinary,j);
            S=ReturnSrc2(tabBinary,j);
            ValImmediate=tabBinary[j][15];
            Mul(r,d,n,S,ValImmediate);
            break;

        case 4:
            d=ReturnDest(tabBinary,j);
            n=ReturnSrc1(tabBinary,j);
            S=ReturnSrc2(tabBinary,j);
            ValImmediate=tabBinary[j][15];
            Div(r,d,n,S,ValImmediate);
            break;
        case 5:
            d=ReturnDest(tabBinary,j);
            n=ReturnSrc1(tabBinary,j);
            S=ReturnSrc2(tabBinary,j);
            ValImmediate=tabBinary[j][15];
            Add(r,d,n,S,ValImmediate);
            break;
        case 6:
            d=ReturnDest(tabBinary,j);
            n=ReturnSrc1(tabBinary,j);
            S=ReturnSrc2(tabBinary,j);
            ValImmediate=tabBinary[j][15];
            Sub(r,d,n,S,ValImmediate);
            break;
        case 7:
            d=ReturnDest(tabBinary,j);
            n=ReturnSrc1(tabBinary,j);
            S=ReturnSrc2(tabBinary,j);
            ValImmediate=tabBinary[j][15];
            Shr(r,d,n,S,ValImmediate);
            break;
        case 8:
            d=ReturnDest(tabBinary,j);
            n=ReturnSrc1(tabBinary,j);
            S=ReturnSrc2(tabBinary,j);
            ValImmediate=tabBinary[j][15];
            Ldb(r,d,n,S,ValImmediate);
            break;
        case 9:
            d=ReturnDest(tabBinary,j);
            n=ReturnSrc1(tabBinary,j);
            S=ReturnSrc2(tabBinary,j);
            ValImmediate=tabBinary[j][15];
            Ldh(r,d,n,S,ValImmediate);
            break;
        case 10:
            d=ReturnDest(tabBinary,j);
            n=ReturnSrc1(tabBinary,j);
            S=ReturnSrc2(tabBinary,j);
            ValImmediate=tabBinary[j][15];
            Ldw(r,d,n,S,ValImmediate);
            break;
        case 11:
            d=ReturnDest(tabBinary,j);
            n=ReturnSrc1(tabBinary,j);
            S=ReturnSrc2(tabBinary,j);
            ValImmediate=tabBinary[j][15];
            Stb(r,d,n,S,ValImmediate);
            break;
        case 12:
            d=ReturnDest(tabBinary,j);
            n=ReturnSrc1(tabBinary,j);
            S=ReturnSrc2(tabBinary,j);
            ValImmediate=tabBinary[j][15];
            Sth(r,d,n,S,ValImmediate);
            break;
        case 13:
            d=ReturnDest(tabBinary,j);
            n=ReturnSrc1(tabBinary,j);
            S=ReturnSrc2(tabBinary,j);
            ValImmediate=tabBinary[j][15];
            Stw(r,d,n,S,ValImmediate);
            break;
        case 14:
            S=ReturnSrc2(tabBinary,j);
            ValImmediate=tabBinary[j][15];
            Jmp(r,S,ValImmediate);
            break;
        case 15:
            S=ReturnSrc2(tabBinary,j);
            ValImmediate=tabBinary[j][15];
            Jzs(r,S,ValImmediate);
            break;
        case 16:
            S=ReturnSrc2(tabBinary,j);
            ValImmediate=tabBinary[j][15];
            Jzc(r,S,ValImmediate);
            break;
        case 17:
            S=ReturnSrc2(tabBinary,j);
            ValImmediate=tabBinary[j][15];
            Jcs(r,S,ValImmediate);
            break;
        case 18:
            S=ReturnSrc2(tabBinary,j);
            ValImmediate=tabBinary[j][15];
            Jcc(r,S,ValImmediate);
            break;
        case 19:
            S=ReturnSrc2(tabBinary,j);
            ValImmediate=tabBinary[j][15];
            Jns(r,S,ValImmediate);
            break;
        case 20:
            S=ReturnSrc2(tabBinary,j);
            ValImmediate=tabBinary[j][15];
            Jnc(r,S,ValImmediate);
            break;
        case 21:
            d=ReturnDest(tabBinary,j);
            In(r,d);
            break;
        case 22:
            d=ReturnDest(tabBinary,j);
            Out(r,d);
            break;
        case 23:
            d=ReturnDest(tabBinary,j);
            n=ReturnSrc1(tabBinary,j);
            S=ReturnSrc2(tabBinary,j);
            ValImmediate=tabBinary[j][15];
            Rnd(r,d,n,S,ValImmediate);
            break;
        case 31:
            PC=-1;
            break;
        default :
            //On sait que ce cas ne sera jamais atteint, car on s'est deja debarrasse de toutes les ereurs potentielles dans la partie 1
            printf("Erreur : code operatoire incompatible");
            exit(-1);
    }
}
