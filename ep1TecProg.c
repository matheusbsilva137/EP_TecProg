#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include "xwc.h"
#include "spacewar.h"

#define G 6.67e-11
#define Rt 6.4e+6
#define Mt 6.02e+24
#define delta_t 0.01

/*Imprime graficamente a nave na Pic P1 usando P2 como base para aplicação da máscara. Ajusta a angulação da nave*/
void exibeNave(Nave nave, Forca forcaFinalN, long coordXMax, long coordYMax, MASK* msks, PIC* pics, PIC P1, PIC P2, int ordemNave);

/*Cria os vetores e mascaras a serem alocados nas posicoes dos vetores passados como parametro
Utiliza os arquivos do diretorio, abrindo todas as imagens referentes às angulações das naves.*/
void criaListaMaskPic(MASK *msks, PIC *pics, WINDOW* w1);

void criaListaMaskPic(MASK *msks, PIC *pics, WINDOW* w1){
    char *str1 = "nave1-";
    char *str2 = "g.xpm";
    char arq[15];
    int i;

    for( i = 0; i <= 710; i += 10 ){
        if( i <= 350 )
            snprintf (arq, 15, "%s%03d%s", str1, i, str2 );
        else
            snprintf (arq, 15, "%s%03d%s", str1, i-360, str2 );
        printf("ABRINDO: %s\n",arq);

        msks[i/10] = NewMask(w1, 28, 28);
        pics[i/10] = ReadPic(w1, arq, msks[i/10]);

        if(i == 350 ) str1 = "nave2-";
    }
}

void exibeNave(Nave nave, Forca forcaFinalN, long coordXMax, long coordYMax, MASK* msks, PIC* pics, PIC P1, PIC P2, int ordemNave){
    int angulo;
    
    angulo = ( (int) round(calculaAngulo(forcaFinalN) ) )/ 10;
    angulo += 35*(ordemNave-1);

    /*Posicionamento dos elementos de acordo com a origem do plano (Centro (400, 320) )*/
    if(nave.coordenadas.pos_x >= 0 && nave.coordenadas.pos_y >= 0){
        /*1º QUADRANTE*/
        PutPic(P2, P1, 400 + nave.coordenadas.pos_x / coordXMax, 320 - nave.coordenadas.pos_y / coordYMax, 28, 28, 0, 0);
        SetMask(P2, msks[angulo]);
        PutPic(P2, pics[angulo], 0, 0, 28, 28, 0, 0);
        PutPic(P1, P2, 0, 0, 28, 28, 400 + nave.coordenadas.pos_x / coordXMax, 320 - nave.coordenadas.pos_y / coordYMax);
        UnSetMask(P2);
        printf("Coorde da janela 1: %lf, %lf \n", 400 + nave.coordenadas.pos_x / coordXMax, 320 - nave.coordenadas.pos_y / coordYMax);
    }else if(nave.coordenadas.pos_x < 0 && nave.coordenadas.pos_y > 0){
        /*2º QUADRANTE*/
        PutPic(P2, P1, 400 - fabs(nave.coordenadas.pos_x) / coordXMax, 320 - nave.coordenadas.pos_y / coordYMax, 28, 28,  0, 0);
        SetMask(P2, msks[angulo]);
        PutPic(P2, pics[angulo], 0, 0, 28, 28, 0, 0);
        PutPic(P1, P2, 0, 0, 28, 28, 400 - fabs(nave.coordenadas.pos_x) / coordXMax, 320 - nave.coordenadas.pos_y / coordYMax);
        UnSetMask(P2);
        printf("Coorde da janela 2: %lf, %lf \n", 400 - nave.coordenadas.pos_x / coordXMax, 320 - nave.coordenadas.pos_y / coordYMax);
    }else if(nave.coordenadas.pos_x < 0 && nave.coordenadas.pos_y < 0){
        /*3º QUADRANTE*/
        PutPic(P2, P1, 400 - fabs(nave.coordenadas.pos_x) / coordXMax, 320 + fabs(nave.coordenadas.pos_y) / coordYMax, 28, 28, 0, 0);
        SetMask(P2, msks[angulo]);
        PutPic(P2, pics[angulo], 0, 0, 28, 28, 0, 0);
        PutPic(P1, P2, 0, 0, 28, 28, 400 - fabs(nave.coordenadas.pos_x) / coordXMax, 320 + fabs(nave.coordenadas.pos_y) / coordYMax );
        UnSetMask(P2);
        printf("Coorde da janela 3: %lf, %lf \n", 400 - nave.coordenadas.pos_x / coordXMax, 320 + nave.coordenadas.pos_y / coordYMax);
    }else{
        /*4º QUADRANTE*/
        PutPic(P2, P1, 400 + nave.coordenadas.pos_x / coordXMax, 320 + fabs(nave.coordenadas.pos_y) / coordYMax, 28, 28, 0, 0);
        SetMask(P2, msks[angulo]);
        PutPic(P2, pics[angulo], 0, 0, 28, 28, 0, 0);
        PutPic(P1, P2, 0, 0, 28, 28, 400 + nave.coordenadas.pos_x / coordXMax, 320 + fabs(nave.coordenadas.pos_y) / coordYMax);
        UnSetMask(P2);
        printf("Coorde da janela 4: %lf, %lf \n", 400 + nave.coordenadas.pos_x / coordXMax, 320 + fabs(nave.coordenadas.pos_y) / coordYMax);
    }
}

int main(int argc, char *argv[]){
    FILE* arquivo;
    Planeta  planetaJogo;
    Nave nave1, nave2;
    Coordenada *coordProjeteis;
    Projetil* listaProjeteis = NULL, *aux, *proj;
    Forca forca1, forca2, forcaFinalN1, forcaFinalN2, forcaFinalP, forcaProj, velNave;
    int  totalProjeteis, k = 0, direcaoX, direcaoY;
    float i;
    double masP, posxP, posyP, velxP, velyP, tmpTotalSimul, tempoDeVida;
    double d, fr, f1, f2, fproj = 0;
    double aceleracao, deslocamentoN1, deslocamentoN2, deslocamentoP;

    WINDOW *w1;
    PIC P1, P2, P3, pics[72];
    MASK msks[72];
    long width = 33e6, height = 528e5;
    long coordX, coordY, coordXMax = width/400, coordYMax = height/320;
    
    w1 = InitGraph(800,640, "Space War");

    P1 = ReadPic(w1, "fundo.xpm", NULL);
    P2 = NewPic(w1, 28, 28);
    P3 = ReadPic(w1, "fundo.xpm", NULL);

    criaListaMaskPic(msks, pics, w1);

    arquivo = fopen(argv[1], "r");
    
    fscanf(arquivo, "%lf %lf %lf", &(planetaJogo.raio), &(planetaJogo.massa), &(tmpTotalSimul) );
    fscanf(arquivo, "%s %lf %lf %lf %lf %lf", nave1.nome, &(nave1.massa), &(nave1.coordenadas.pos_x), &(nave1.coordenadas.pos_y), &(nave1.vel_x), &(nave1.vel_y) );

    fscanf(arquivo, "%s %lf %lf %lf %lf %lf", nave2.nome, &(nave2.massa), &(nave2.coordenadas.pos_x), &(nave2.coordenadas.pos_y), &(nave2.vel_x), &(nave2.vel_y) );
    fscanf(arquivo, "%d %lf", &(totalProjeteis), &(tempoDeVida));

    planetaJogo.coordenadas.pos_x = 0;
    planetaJogo.coordenadas.pos_y = 0;

    for (i = 1; i <= totalProjeteis; i++){
        fscanf(arquivo, "%lf %lf %lf %lf %lf", &(masP), &(posxP), &(posyP), &(velxP), &(velyP) );
        insereProjetil(masP, posxP, posyP, velxP, velyP, &listaProjeteis);
    }
    aux = listaProjeteis;
    proj = listaProjeteis;
    fclose(arquivo);
    
    coordProjeteis = malloc(totalProjeteis*sizeof(Coordenada));
    
    for(i = 0; i < tmpTotalSimul; i = i+delta_t){
        for(k = 0; k < totalProjeteis && aux != NULL; k++, aux = aux->prox){
            aux->coordenadas.pos_x = coordProjeteis[k].pos_x;
            aux->coordenadas.pos_y = coordProjeteis[k].pos_y;
            printf("PROJÉTIL %d: (%lf , %lf)\n", k+1, aux->coordenadas.pos_x, aux->coordenadas.pos_y);
        }

        proj = listaProjeteis;

        /*NAVE 1*/
        d = distanciaEntrePontos(nave1.coordenadas, planetaJogo.coordenadas);
        f1 = forcaGravitacional(nave1.massa, planetaJogo.massa, d);
        
        forca1 = normalizaForca(nave1.coordenadas, planetaJogo.coordenadas, f1);

        f2 = forcaGravitacional(nave1.massa, nave2.massa, d);
        forca2 = normalizaForca(nave1.coordenadas, nave2.coordenadas, f2);

        fr = forcaResultante(forca1, forca2);
        forcaFinalN1 = calcCoordForcaRes(forca1, forca2, fr);

        /*Percorro a lista ligada de projéteis criando uma nova força resultante
        em cada iteração do while */
        while (aux != NULL && totalProjeteis > 0) {
            d = distanciaEntrePontos(nave1.coordenadas, aux->coordenadas);
            fproj = forcaGravitacional(nave1.massa, aux->massa, d);
            forcaProj  = normalizaForca(nave1.coordenadas, aux->coordenadas, fproj);

            fr = forcaResultante(forcaProj, forcaFinalN1);
            forcaFinalN1 = calcCoordForcaRes(forcaFinalN1, forcaProj, fr);

            aux = aux -> prox;
        }

        if(nave1.massa > eps){
            aceleracao = forcaFinalN1.intensidade / nave1.massa;
        }else{
            if(forcaFinalN1.intensidade <= eps){
                aceleracao = 0;
            }
            /*Se a massa é muito pequena e a força não é 0, 
            a aceleração pode tender a infinito, não representável*/
        }

        if ( forcaFinalN1.fim.pos_x - forcaFinalN1.inicio.pos_x < 0) {
           direcaoX =  -1; 
        }else{
            direcaoX = 1;
        }
        
        if(forcaFinalN1.fim.pos_y - forcaFinalN1.inicio.pos_y < 0){
            direcaoY = -1;
        }else{
            direcaoY = 1;
        }
        
        nave1.coordenadas.pos_x += direcaoX * nave1.vel_x * delta_t;
        nave1.coordenadas.pos_y += direcaoY * nave1.vel_y * delta_t;

        nave1.vel_x += aceleracao * delta_t;
        nave1.vel_y += aceleracao * delta_t;        

        /*NAVE 2*/
        aux = listaProjeteis;
        d = distanciaEntrePontos(nave2.coordenadas, planetaJogo.coordenadas);
        f1 = forcaGravitacional(nave2.massa, planetaJogo.massa, d);
        
        forca1 = normalizaForca(nave2.coordenadas, planetaJogo.coordenadas, f1);

        f2 = forcaGravitacional(nave2.massa, nave1.massa, d);
        forca2 = normalizaForca(nave2.coordenadas, nave1.coordenadas, f2);

        fr = forcaResultante(forca1, forca2);
        forcaFinalN2 = calcCoordForcaRes(forca1, forca2, fr);

        /*Percorro a lista ligada de projéteis criando uma nova força resultante
        em cada iteração do while */
        while (aux != NULL && totalProjeteis > 0) {
            d = distanciaEntrePontos(nave2.coordenadas, aux->coordenadas);
            fproj = forcaGravitacional(nave2.massa, aux->massa, d);
            forcaProj  = normalizaForca(nave2.coordenadas, aux->coordenadas, fproj);

            fr = forcaResultante(forcaProj, forcaFinalN2);
            forcaFinalN2 = calcCoordForcaRes(forcaFinalN2, forcaProj, fr);

            aux = aux -> prox;
        }

        if(nave2.massa > eps){
            aceleracao = forcaFinalN2.intensidade / nave2.massa;
        }else{
            if(forcaFinalN2.intensidade <= eps){
                aceleracao = 0;
            }
            /*Se a massa é muito pequena e a força não é 0, 
            a aceleração pode tender a infinito, não representável*/
        }

        if ( forcaFinalN2.fim.pos_x - forcaFinalN2.inicio.pos_x < 0) {
           direcaoX =  -1; 
        }else{
            direcaoX = 1;
        }
        
        if(forcaFinalN2.fim.pos_y - forcaFinalN2.inicio.pos_y < 0){
            direcaoY = -1;
        }else{
            direcaoY = 1;
        }

        nave2.coordenadas.pos_x += direcaoX * nave2.vel_x * delta_t;
        nave2.coordenadas.pos_y += direcaoY * nave2.vel_y * delta_t;

        nave2.vel_x += aceleracao * delta_t;
        nave2.vel_y += aceleracao * delta_t;

        /*PROJETEIS*/
        while(proj != NULL && totalProjeteis > 0){
            aux = listaProjeteis;
            d = distanciaEntrePontos(proj->coordenadas, planetaJogo.coordenadas);
            f1 = forcaGravitacional(proj->massa, planetaJogo.massa, d);
            
            forca1 = normalizaForca(proj->coordenadas, planetaJogo.coordenadas, f1);

            f2 = forcaGravitacional(proj->massa, nave1.massa, d);
            forca2 = normalizaForca(proj->coordenadas, nave1.coordenadas, f2);

            fr = forcaResultante(forca1, forca2);
            forcaFinalP = calcCoordForcaRes(forca1, forca2, fr);

            f2 = forcaGravitacional(proj->massa, nave2.massa, d);
            forca2 = normalizaForca(proj->coordenadas, nave2.coordenadas, f2);
            fr = forcaResultante(forcaFinalP, forca2);

            forcaFinalP = calcCoordForcaRes(forcaFinalP, forca2, fr);

            /*Percorro a lista ligada de projéteis criando uma nova força resultante
            em cada iteração do while */
            while (aux != NULL && totalProjeteis > 0) {
                d = distanciaEntrePontos(proj->coordenadas, aux->coordenadas);
                fproj = forcaGravitacional(proj->massa, aux->massa, d);
                forcaProj  = normalizaForca(proj->coordenadas, aux->coordenadas, fproj);

                fr = forcaResultante(forcaProj, forcaFinalP);
                forcaFinalP = calcCoordForcaRes(forcaFinalP, forcaProj, fr);

                aux = aux -> prox;
            }

            velNave.inicio.pos_x = proj->coordenadas.pos_x; velNave.inicio.pos_y = proj->coordenadas.pos_y;
            velNave.fim.pos_x    = proj->vel_x; velNave.fim.pos_y = proj->vel_y;
            velNave.intensidade  = sqrt( pow(proj->vel_x,2) + pow(proj->vel_y,2));
            velNave              = normalizaForca(proj->coordenadas, velNave.fim , velNave.intensidade);

            fr = forcaResultante(velNave, forcaFinalP);
            forcaFinalP = calcCoordForcaRes(forcaFinalP, velNave, fr);

            if(proj->massa > eps){
                aceleracao = forcaFinalP.intensidade / proj->massa;
            }else{
                if(forcaFinalP.intensidade <= eps){
                    aceleracao = 0;
                }
                /*Se a massa é muito pequena e a força não é 0, 
                a aceleração pode tender a infinito, não representável*/
            }
            
            deslocamentoP = sqrt(pow(proj->vel_x,2) + pow(proj->vel_y, 2))*i + (aceleracao*i*i/2);
            forcaFinalP = normalizaForca(proj->coordenadas, forcaFinalP.fim, deslocamentoP);
            coordProjeteis[k].pos_x = forcaFinalP.fim.pos_x;
            coordProjeteis[k].pos_y = forcaFinalP.fim.pos_y;

            k++;
            proj = proj->prox;
        }

        /***
         *IMPRESSÃO DAS COORDENADAS
         */       
        printf("******Tempo %f\n", i);
        printf("NAVE 1: (%lf , %lf)\n", nave1.coordenadas.pos_x, nave1.coordenadas.pos_y);
        nave1.coordenadas = posicaoToroidal(nave1, width, height);
        exibeNave(nave1, forcaFinalN1, coordXMax, coordYMax, msks, pics, P1, P2, 1);
        
        printf("NAVE 2: (%lf , %lf)\n", nave2.coordenadas.pos_x, nave2.coordenadas.pos_y);
        nave2.coordenadas = posicaoToroidal(nave2, width, height);
        exibeNave(nave2, forcaFinalN2, coordXMax, coordYMax, msks, pics, P1, P2, 2);
        
        PutPic(w1, P1, 0, 0, 800, 640, 0, 0);

        aux = listaProjeteis;
        WFlush();
        usleep(10000);
        WClear(w1);
        PutPic(P1, P3, 0, 0, 800, 640, 0, 0);
        printf("\n\n");
    }
    return 0;
}