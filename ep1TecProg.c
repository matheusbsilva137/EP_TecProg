#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "spacewar.h"

#define G 6.67e-11
#define Rt 6.4e+6
#define Mt 6.02e+24
#define delta_t 0.01

int main(int argc, char *argv[]){
    FILE* arquivo;
    Planeta  planetaJogo;
    Nave nave1, nave2;
    int  totalProjeteis, k = 0;
    float i;
    double masP, posxP, posyP, velxP, velyP, tmpTotalSimul, tempoDeVida;
    double d, fr, f1, f2, fproj = 0;
    double aceleracao, deslocamentoN1, deslocamentoN2, deslocamentoP;
    Coordenada *coordProjeteis;
    Projetil* listaProjeteis = NULL;
    Projetil* aux, *proj;
    Forca forca1, forca2, forcaFinalN1, forcaFinalN2, forcaFinalP, forcaProj, velNave;

    arquivo = fopen(argv[1], "r");
    
    fscanf(arquivo, "%lf %lf %lf", &(planetaJogo.raio), &(planetaJogo.massa), &(tmpTotalSimul) );
    fscanf(arquivo, "%s %lf %lf %lf %lf %lf", nave1.nome, &(nave1.massa), &(nave1.coordenadas.pos_x), &(nave1.coordenadas.pos_y), &(nave1.vel_x), &(nave1.vel_y) );

    fscanf(arquivo, "%s %lf %lf %lf %lf %lf", nave2.nome, &(nave2.massa), &(nave2.coordenadas.pos_x), &(nave2.coordenadas.pos_y), &(nave2.vel_x), &(nave2.vel_y) );
    fscanf(arquivo, "%d %lf", &(totalProjeteis), &(tempoDeVida));

    nave1.massa = nave1.massa / Mt;
    nave2.massa = nave2.massa / Mt;                 /*Conversão da massa para a unidade Mt */
    planetaJogo.massa = planetaJogo.massa / Mt;

    nave1.coordenadas.pos_x = nave1.coordenadas.pos_x / Rt;
    nave1.coordenadas.pos_y = nave1.coordenadas.pos_y / Rt;
    nave2.coordenadas.pos_x = nave2.coordenadas.pos_x / Rt; /*Conversão da massa para a unidade Mt */
    nave2.coordenadas.pos_y = nave2.coordenadas.pos_y / Rt;

    planetaJogo.coordenadas.pos_x = 0;
    planetaJogo.coordenadas.pos_y = 0;

    for (i = 1; i <= totalProjeteis; i++){
        fscanf(arquivo, "%lf %lf %lf %lf %lf", &(masP), &(posxP), &(posyP), &(velxP), &(velyP) );
        insereProjetil(masP/Mt, posxP/Rt, posyP/Rt, velxP, velyP, &listaProjeteis);
    }
    aux = listaProjeteis;
    proj = listaProjeteis;
    fclose(arquivo);
    
    coordProjeteis = malloc(totalProjeteis*sizeof(Coordenada));

    for(i = 0; i < tmpTotalSimul; i = i+delta_t){
        printf("******Tempo %f\n", i);
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

        velNave.inicio.pos_x = nave1.coordenadas.pos_x; velNave.inicio.pos_y = nave1.coordenadas.pos_y;
        velNave.fim.pos_x    = nave1.vel_x; velNave.fim.pos_y = nave1.vel_y;
        velNave.intensidade  = sqrt( pow(nave1.vel_x,2) + pow(nave1.vel_y,2));
        velNave              = normalizaForca(nave1.coordenadas, velNave.fim , velNave.intensidade);

        fr = forcaResultante(velNave, forcaFinalN1);
        forcaFinalN1 = calcCoordForcaRes(forcaFinalN1, velNave, fr);

        if(nave1.massa > eps){
            aceleracao = forcaFinalN1.intensidade / nave1.massa;
        }else{
            if(forcaFinalN1.intensidade <= eps){
                aceleracao = 0;
            }
            /*Se a massa é muito pequena e a força não é 0, 
            a aceleração pode tender a infinito, não representável*/
        }
        
        deslocamentoN1 = sqrt(pow(nave1.vel_x,2) + pow(nave1.vel_y, 2))*i + (aceleracao*i*i/2);
        forcaFinalN1 = normalizaForca(nave1.coordenadas, forcaFinalN1.fim, deslocamentoN1);

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

        velNave.inicio.pos_x = nave2.coordenadas.pos_x; velNave.inicio.pos_y = nave2.coordenadas.pos_y;
        velNave.fim.pos_x    = nave2.vel_x; velNave.fim.pos_y = nave2.vel_y;
        velNave.intensidade  = sqrt( pow(nave2.vel_x,2) + pow(nave2.vel_y,2));
        velNave              = normalizaForca(nave2.coordenadas, velNave.fim , velNave.intensidade);

        fr = forcaResultante(velNave, forcaFinalN2);
        forcaFinalN2 = calcCoordForcaRes(forcaFinalN2, velNave, fr);

        if(nave2.massa > eps){
            aceleracao = forcaFinalN2.intensidade / nave2.massa;
        }else{
            if(forcaFinalN2.intensidade <= eps){
                aceleracao = 0;
            }
            /*Se a massa é muito pequena e a força não é 0, 
            a aceleração pode tender a infinito, não representável*/
        }
        
        deslocamentoN2 = sqrt(pow(nave2.vel_x,2) + pow(nave2.vel_y, 2))*i + (aceleracao*i*i/2);
        forcaFinalN2 = normalizaForca(nave2.coordenadas, forcaFinalN2.fim, deslocamentoN2);

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
        
        /*ATUALIZACOES DAS POSICOES DAS NAVES*/
        nave1.coordenadas.pos_x = forcaFinalN1.fim.pos_x;
        nave1.coordenadas.pos_y = forcaFinalN1.fim.pos_y;
        printf("NAVE 1: (%lf , %lf)\n", nave1.coordenadas.pos_x, nave1.coordenadas.pos_y);

        nave2.coordenadas.pos_x = forcaFinalN2.fim.pos_x;
        nave2.coordenadas.pos_y = forcaFinalN2.fim.pos_y;
        printf("NAVE 2: (%lf , %lf)\n", nave2.coordenadas.pos_x, nave2.coordenadas.pos_y);

        aux = listaProjeteis;
        for(k = 0; k < totalProjeteis && aux != NULL; k++, aux = aux->prox){
            aux->coordenadas.pos_x = coordProjeteis[k].pos_x;
            aux->coordenadas.pos_y = coordProjeteis[k].pos_y;
            printf("PROJÉTIL %d: (%lf , %lf)\n", k+1, aux->coordenadas.pos_x, aux->coordenadas.pos_y);
        }

        printf("\n\n");
    }

    return 0;
}