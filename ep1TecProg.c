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
Coordenada exibeNave(Nave nave, double velX, double velY, long coordXMax, long coordYMax, MASK* msks, PIC* pics, PIC P1, PIC P2, int ordemNave);

/*Cria os vetores e mascaras a serem alocados nas posicoes dos vetores passados como parametro
Utiliza os arquivos do diretorio, abrindo todas as imagens referentes às angulações das naves.*/
void criaListaMaskPic(MASK *msks, PIC *pics, WINDOW* w1);

int detectaColisao(Coordenada nave1, Coordenada nave2, Projetil *listaProjeteis);

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

int detectaColisao(Coordenada nave1, Coordenada nave2, Projetil *listaProjeteis){
    Coordenada centro;
    centro.pos_x = 400; centro.pos_y = 320;
    nave1.pos_x += 14; nave1.pos_y -= 14;
    nave2.pos_x += 14; nave2.pos_y -= 14;
    if( distanciaEntrePontos(nave1, nave2) <= 28 )
        return 0;
    if(distanciaEntrePontos(nave1, centro) <= 50 && distanciaEntrePontos(nave2, centro) <= 50 )
        return 0;
    if(distanciaEntrePontos(nave1, centro) <= 50 )
        return 2;
    if(distanciaEntrePontos(nave2, centro) <= 50 )
        return 1;
    else
        return -1;    
}

Coordenada exibeNaveAngulo(Nave nave, int angulo, long coordXMax, long coordYMax, MASK* msks, PIC* pics, PIC P1, PIC P2, int ordemNave){
    Coordenada naveCoord;
    Forca velocidade;

    angulo += 36*(ordemNave-1);

    /*Posicionamento dos elementos de acordo com a origem do plano (Centro (400, 320) )*/
    if(nave.coordenadas.pos_x >= 0 && nave.coordenadas.pos_y >= 0){
        /*1º QUADRANTE*/
        naveCoord.pos_x = 400 + nave.coordenadas.pos_x / coordXMax;
        naveCoord.pos_y = 320 - nave.coordenadas.pos_y / coordYMax;
        PutPic(P2, P1, naveCoord.pos_x, naveCoord.pos_y, 28, 28, 0, 0);
        printf("Coorde da janela 1: %lf, %lf \n", 400 + nave.coordenadas.pos_x / coordXMax, 320 - nave.coordenadas.pos_y / coordYMax);
    }else if(nave.coordenadas.pos_x < 0 && nave.coordenadas.pos_y > 0){
        /*2º QUADRANTE*/
        naveCoord.pos_x = 400 - fabs(nave.coordenadas.pos_x) / coordXMax;
        naveCoord.pos_y = 320 - nave.coordenadas.pos_y / coordYMax; 
        PutPic(P2, P1, naveCoord.pos_x, naveCoord.pos_y, 28, 28,  0, 0);
        printf("Coorde da janela 2: %lf, %lf \n", 400 - nave.coordenadas.pos_x / coordXMax, 320 - nave.coordenadas.pos_y / coordYMax);
    }else if(nave.coordenadas.pos_x < 0 && nave.coordenadas.pos_y < 0){
        /*3º QUADRANTE*/
        naveCoord.pos_x = 400 - fabs(nave.coordenadas.pos_x) / coordXMax;
        naveCoord.pos_y = 320 + fabs(nave.coordenadas.pos_y) / coordYMax;
        PutPic(P2, P1, naveCoord.pos_x, naveCoord.pos_y, 28, 28, 0, 0);
        printf("Coorde da janela 3: %lf, %lf \n", 400 - nave.coordenadas.pos_x / coordXMax, 320 + nave.coordenadas.pos_y / coordYMax);
    }else{
        /*4º QUADRANTE*/
        naveCoord.pos_x = 400 + nave.coordenadas.pos_x / coordXMax;
        naveCoord.pos_y  = 320 + fabs(nave.coordenadas.pos_y) / coordYMax;
        PutPic(P2, P1, naveCoord.pos_x, naveCoord.pos_y, 28, 28, 0, 0);
        printf("Coorde da janela 4: %lf, %lf \n", 400 + nave.coordenadas.pos_x / coordXMax, 320 + fabs(nave.coordenadas.pos_y) / coordYMax);
    }
    SetMask(P2, msks[angulo]);
    PutPic(P2, pics[angulo], 0, 0, 28, 28, 0, 0);
    PutPic(P1, P2, 0, 0, 28, 28, naveCoord.pos_x, naveCoord.pos_y );
    UnSetMask(P2);

    return naveCoord;
}

Coordenada exibeNave(Nave nave, double dirX, double dirY, long coordXMax, long coordYMax, MASK* msks, PIC* pics, PIC P1, PIC P2, int ordemNave){
    Coordenada naveCoord;
    Forca velocidade;
    int angulo;
    
    velocidade.inicio.pos_x = 0; velocidade.inicio.pos_y = 0;
    velocidade.fim.pos_x = dirX; velocidade.fim.pos_y = dirY;

    angulo = ( (int) round(calculaAngulo(velocidade) ) )/ 10;
    angulo += 36*(ordemNave-1);

    /*Posicionamento dos elementos de acordo com a origem do plano (Centro (400, 320) )*/
    if(nave.coordenadas.pos_x >= 0 && nave.coordenadas.pos_y >= 0){
        /*1º QUADRANTE*/
        naveCoord.pos_x = 400 + nave.coordenadas.pos_x / coordXMax;
        naveCoord.pos_y = 320 - nave.coordenadas.pos_y / coordYMax;
        PutPic(P2, P1, naveCoord.pos_x, naveCoord.pos_y, 28, 28, 0, 0);
        printf("Coorde da janela 1: %lf, %lf \n", 400 + nave.coordenadas.pos_x / coordXMax, 320 - nave.coordenadas.pos_y / coordYMax);
    }else if(nave.coordenadas.pos_x < 0 && nave.coordenadas.pos_y > 0){
        /*2º QUADRANTE*/
        naveCoord.pos_x = 400 - fabs(nave.coordenadas.pos_x) / coordXMax;
        naveCoord.pos_y = 320 - nave.coordenadas.pos_y / coordYMax; 
        PutPic(P2, P1, naveCoord.pos_x, naveCoord.pos_y, 28, 28,  0, 0);
        printf("Coorde da janela 2: %lf, %lf \n", 400 - nave.coordenadas.pos_x / coordXMax, 320 - nave.coordenadas.pos_y / coordYMax);
    }else if(nave.coordenadas.pos_x < 0 && nave.coordenadas.pos_y < 0){
        /*3º QUADRANTE*/
        naveCoord.pos_x = 400 - fabs(nave.coordenadas.pos_x) / coordXMax;
        naveCoord.pos_y = 320 + fabs(nave.coordenadas.pos_y) / coordYMax;
        PutPic(P2, P1, naveCoord.pos_x, naveCoord.pos_y, 28, 28, 0, 0);
        printf("Coorde da janela 3: %lf, %lf \n", 400 - nave.coordenadas.pos_x / coordXMax, 320 + nave.coordenadas.pos_y / coordYMax);
    }else{
        /*4º QUADRANTE*/
        naveCoord.pos_x = 400 + nave.coordenadas.pos_x / coordXMax;
        naveCoord.pos_y  = 320 + fabs(nave.coordenadas.pos_y) / coordYMax;
        PutPic(P2, P1, naveCoord.pos_x, naveCoord.pos_y, 28, 28, 0, 0);
        printf("Coorde da janela 4: %lf, %lf \n", 400 + nave.coordenadas.pos_x / coordXMax, 320 + fabs(nave.coordenadas.pos_y) / coordYMax);
    }

    printf("entrei na agulo angulo\n");
    SetMask(P2, msks[angulo]);
    PutPic(P2, pics[angulo], 0, 0, 28, 28, 0, 0);
    PutPic(P1, P2, 0, 0, 28, 28, naveCoord.pos_x, naveCoord.pos_y );
    UnSetMask(P2);

    return naveCoord;
}

Coordenada exibeProjetil(Projetil* p, Forca forcaFinalP, long coordXMax, long coordYMax, PIC P1){
    Coordenada pCoord;

    /*Posicionamento dos elementos de acordo com a origem do plano (Centro (400, 320) )*/
    if(p->coordenadas.pos_x >= 0 && p->coordenadas.pos_y >= 0){
        /*1º QUADRANTE*/
        pCoord.pos_x = 400 + p->coordenadas.pos_x / coordXMax;
        pCoord.pos_y = 320 - p->coordenadas.pos_y / coordYMax;
    }else if(p->coordenadas.pos_x < 0 && p->coordenadas.pos_y > 0){
        /*2º QUADRANTE*/
        pCoord.pos_x = 400 - fabs(p->coordenadas.pos_x) / coordXMax;
        pCoord.pos_y = 320 - p->coordenadas.pos_y / coordYMax;
    }else if(p->coordenadas.pos_x < 0 && p->coordenadas.pos_y < 0){
        /*3º QUADRANTE*/
        pCoord.pos_x = 400 - fabs(p->coordenadas.pos_x) / coordXMax;
        pCoord.pos_y = 320 + fabs(p->coordenadas.pos_y) / coordYMax;
    }else{
        /*4º QUADRANTE*/
        pCoord.pos_x = 400 + p->coordenadas.pos_x / coordXMax;
        pCoord.pos_y  = 320 + fabs(p->coordenadas.pos_y) / coordYMax;
    }
    WFillArc(P1, pCoord.pos_x, pCoord.pos_y, 0, 360*64, 5, 5, WNamedColor("WHITE"));

    return pCoord;
}

int main(int argc, char *argv[]){
    FILE* arquivo;
    Planeta  planetaJogo;
    Nave nave1, nave2;
    Coordenada *coordProjeteis, coordJanelaN1, coordJanelaN2;
    Projetil* listaProjeteis = NULL, *aux, *proj;
    Forca forca1, forca2, forcaFinalN1, forcaFinalN2, forcaFinalP, forcaProj, velNave;
    KeyCode kDir1, kEsq1, kAcel1, kAtir1;
    KeyCode kDir2, kEsq2, kAcel2, kAtir2, key1, key2;
    int  totalProjeteis, k = 0, direcaoX, direcaoY, vencedor = -1;
    float i;
    double masP, posxP, posyP, velxP, velyP, tmpTotalSimul, tempoDeVida;
    double d, fr, f1, f2, fproj = 0;
    double aceleracao, deslocamentoN1, deslocamentoN2, deslocamentoP;
    int anguloInicial1, anguloInicial2, exibiuNave1, exibiuNave2, jogar = 1;
    int totRot1 = 0, totRot2 = 0;
    char textoProj[28];

    WINDOW *w1;
    PIC P1, P2, P3, pics[72];
    MASK msks[72];
    long width = 33e6, height = 528e5;
    long coordX, coordY, coordXMax = width/400, coordYMax = height/320;
    
    w1 = InitGraph(800,670, "Space War");
    InitKBD(w1);
    WPrint(w1, 10, 20, "Escolha uma tecla para girar a nave 1 para a direita" );
    kDir1 = WGetKey(w1);
    WPrint(w1, 10, 50, "Escolha uma tecla para girar a nave 1 para a esquerda" );
    kEsq1 = WGetKey(w1);
    WPrint(w1, 10, 80, "Escolha uma tecla para acelerar a nave 1" );
    kAcel1 = WGetKey(w1);
    WPrint(w1, 10, 110, "Escolha uma tecla para disparar os projeteis na nave 1" );
    kAtir1 = WGetKey(w1);

    WPrint(w1, 410, 20, "Escolha uma tecla para girar a nave 2 para a direita" );
    kDir2 = WGetKey(w1);
    WPrint(w1, 410, 50, "Escolha uma tecla para girar a nave 2 para a esquerda" );
    kEsq2 = WGetKey(w1);
    WPrint(w1, 410, 80, "Escolha uma tecla para acelerar a nave 2" );
    kAcel2 = WGetKey(w1);
    WPrint(w1, 410, 110, "Escolha uma tecla para disparar os projeteis na nave 2" );
    kAtir2 = WGetKey(w1);

    P1 = ReadPic(w1, "fundo.xpm", NULL);
    P2 = NewPic(w1, 28, 28);
    P3 = ReadPic(w1, "fundo.xpm", NULL);

    criaListaMaskPic(msks, pics, w1);

    nave1.quantVidasRestantes = 3;
    nave2.quantVidasRestantes = 3;

    while (jogar && nave1.quantVidasRestantes > 0 && nave2.quantVidasRestantes > 0){
        vencedor = -1;
        arquivo = fopen(argv[1], "r");
        
        fscanf(arquivo, "%lf %lf %lf", &(planetaJogo.raio), &(planetaJogo.massa), &(tmpTotalSimul) );
        fscanf(arquivo, "%s %lf %lf %lf %lf %lf", nave1.nome, &(nave1.massa), &(nave1.coordenadas.pos_x), &(nave1.coordenadas.pos_y), &(nave1.vel_x), &(nave1.vel_y) );

        fscanf(arquivo, "%s %lf %lf %lf %lf %lf", nave2.nome, &(nave2.massa), &(nave2.coordenadas.pos_x), &(nave2.coordenadas.pos_y), &(nave2.vel_x), &(nave2.vel_y) );
        fscanf(arquivo, "%d %lf", &(totalProjeteis), &(tempoDeVida));
        nave1.quantProjeteisRestantes = totalProjeteis/2;
        nave2.quantProjeteisRestantes = totalProjeteis/2;

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

        velNave.inicio.pos_x = 0; velNave.inicio.pos_y = 0;
        velNave.fim.pos_x = nave1.vel_x; velNave.fim.pos_y = nave1.vel_y;
        anguloInicial1 = ( (int) round(calculaAngulo(velNave) ) );

        velNave.inicio.pos_x = 0; velNave.inicio.pos_y = 0;
        velNave.fim.pos_x = nave2.vel_x; velNave.fim.pos_y = nave2.vel_y;
        anguloInicial2 = ( (int) round(calculaAngulo(velNave) ) );

        PutPic(P1, P3, 0, 0, 800, 670, 0, 0);

        for(i = 0; i < tmpTotalSimul && vencedor < 0; i = i+delta_t){   
            exibiuNave1 = 0; exibiuNave2 = 0;     
            while(WCheckKBD(w1) == 1){
                key2 = WKeySymToKeyCode(WLastKeySym());
                key1 = WGetKey(w1);

                /*if( key1 != key2 &&
                    key2 != kAcel1 &&
                    key2 != kDir1 &&
                    key2 != kEsq1 &&
                    key2 != kAtir1 ){
                        if ( key2 == kDir2){
                            WPrint(P1, 560, 658, "(DIR)");
                            totRot2 += 10;
                        } else if ( key2 == kEsq2){
                            WPrint(P1, 560, 658, "(ESQ)");
                            totRot2 -= 10;
                        } else if ( key2 == kAcel2){
                            if( (nave2.vel_x < 4e6 && nave2.vel_y < 4e6) && (nave2.vel_x > -4e6 && nave2.vel_y > -4e6) ){
                                WPrint(P1, 560, 658, "(ACEL)");
                                rotacionaNave(&nave2, totRot2);
                                totRot2 = 0;
                                aceleraNave(&nave2);
                            }else{
                                WPrint(P1, 490, 658, "(LIM. DE VEL.)");
                            }
                        } else if ( key2 == kAtir2){
                            
                        }
                }*/

                if ( key1 == kDir1){
                    WPrint(P1, 210, 658, "(DIR)");
                    totRot1 += 10;
                    anguloInicial1 = (anguloInicial1+10)%360;

                    printf("Angulo em graus da nave 1: %d\n", anguloInicial1/10);
                    coordJanelaN1 = exibeNaveAngulo(nave1, anguloInicial1/10, coordXMax, coordYMax, msks, pics, P1, P2, 1);
                    exibiuNave1 = 1;
                } else if ( key1 == kEsq1){
                    WPrint(P1, 210, 658, "(ESQ)");
                    totRot1 += 350;
                    anguloInicial1 = (anguloInicial1+350)%360;
                    printf("Angulo em graus da nave 1: %d\n", anguloInicial1/10);
                    coordJanelaN1 = exibeNaveAngulo(nave1, anguloInicial1/10, coordXMax, coordYMax, msks, pics, P1, P2, 1);
                    exibiuNave1 = 1;
                } else if ( key1 == kAcel1){
                    if( (nave1.vel_x < 5e6 && nave1.vel_y < 5e6) && (nave1.vel_x > -5e6 && nave1.vel_y > -5e6)){
                        WPrint(P1, 210, 658, "(ACEL)");
                        rotacionaNave(&nave1, (totRot1+180)%360);
                        totRot1 = 0;
                        aceleraNave(&nave1);
                    }else{
                        WPrint(P1, 210, 658, "(LIM. DE VEL.)");
                        rotacionaNave(&nave1, (totRot1+180)%360);
                        totRot1 = 0;
                    }
                } else if ( key1 == kAtir1){

                }

                /*TECLAS PARA A NAVE 2*/
                /*if( key1 != key2 &&
                    key2 != kAcel2 &&
                    key2 != kDir2 &&
                    key2 != kEsq2 &&
                    key2 != kAtir2 ){
                        if ( key2 == kDir1){
                            WPrint(P1, 560, 658, "(DIR)");
                            totRot1 += 10;
                        } else if ( key2 == kEsq1){
                            WPrint(P1, 560, 658, "(ESQ)");
                            totRot1 -= 10;
                        } else if ( key2 == kAcel1){
                            if( (nave1.vel_x < 4e6 && nave1.vel_y < 4e6) && (nave1.vel_x > -4e6 && nave1.vel_y > -4e6) ){
                                WPrint(P1, 560, 658, "(ACEL)");
                                rotacionaNave(&nave1, totRot1);
                                totRot1 = 0;
                                aceleraNave(&nave1);
                            }else{
                                WPrint(P1, 490, 658, "(LIM. DE VEL.)");
                            }
                        } else if ( key2 == kAtir2){
                            
                        }
                }*/

                if ( key1 == kDir2){
                    WPrint(P1, 560, 658, "(DIR)");
                    totRot2 += 10;
                    anguloInicial2 = (anguloInicial2+10)%360;
                    coordJanelaN2 = exibeNaveAngulo(nave2, anguloInicial2/10, coordXMax, coordYMax, msks, pics, P1, P2, 2);
                    exibiuNave2 = 1;
                } else if ( key1 == kEsq2){
                    WPrint(P1, 560, 658, "(ESQ)");
                    totRot2 += 350;
                    anguloInicial2 = (anguloInicial2+350)%360;
                    coordJanelaN2 = exibeNaveAngulo(nave2, anguloInicial2/10, coordXMax, coordYMax, msks, pics, P1, P2, 2);
                    exibiuNave2 = 1;
                } else if ( key1 == kAcel2){
                    if( (nave2.vel_x < 4e6 && nave2.vel_y < 5e6) && (nave2.vel_x > -5e6 && nave2.vel_y > -5e6) ){
                        WPrint(P1, 560, 658, "(ACEL)");
                        rotacionaNave(&nave2, totRot2%360);
                        totRot2 = 0;
                        aceleraNave(&nave2);
                    }else{
                        WPrint(P1, 490, 658, "(LIM. DE VEL.)");
                        rotacionaNave(&nave2, totRot2%360);
                        totRot2 = 0;
                    }
                } else if ( key1 == kAtir2){
                    
                }
            }

            PutPic(P1, pics[9], 0, 0, 28, 28, 0, 640);
            if(nave1.quantProjeteisRestantes == 1){
                snprintf(textoProj, 26, "%s -> %d Projetil Restante", nave1.nome, nave1.quantProjeteisRestantes);
            }else{
                snprintf(textoProj, 29, "%s -> %d Projeteis Restantes", nave1.nome, nave1.quantProjeteisRestantes);
            }
            WPrint(P1, 32, 658, textoProj );

            PutPic(P1, pics[45], 0, 0, 28, 28, 772, 640);
            if(nave2.quantProjeteisRestantes == 1){
                snprintf(textoProj, 26, "%d Projetil Restante <- %s", nave2.quantProjeteisRestantes, nave2.nome );
            }else{
                snprintf(textoProj, 29, "%d Projeteis Restantes <- %s", nave2.quantProjeteisRestantes, nave2.nome);
            }
            WPrint(P1, 600, 658, textoProj );

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
                if(aux->ativo == 1){
                    d = distanciaEntrePontos(nave1.coordenadas, aux->coordenadas);
                    fproj = forcaGravitacional(nave1.massa, aux->massa, d);
                    forcaProj  = normalizaForca(nave1.coordenadas, aux->coordenadas, fproj);

                    fr = forcaResultante(forcaProj, forcaFinalN1);
                    forcaFinalN1 = calcCoordForcaRes(forcaFinalN1, forcaProj, fr);
                }

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
            
            nave1.vel_x += aceleracao * delta_t;
            nave1.vel_y += aceleracao * delta_t;

            nave1.coordenadas.pos_x += direcaoX * nave1.vel_x * delta_t;
            nave1.coordenadas.pos_y += direcaoY * nave1.vel_y * delta_t;       

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
                if(aux->ativo == 1){
                    d = distanciaEntrePontos(nave2.coordenadas, aux->coordenadas);
                    fproj = forcaGravitacional(nave2.massa, aux->massa, d);
                    forcaProj  = normalizaForca(nave2.coordenadas, aux->coordenadas, fproj);

                    fr = forcaResultante(forcaProj, forcaFinalN2);
                    forcaFinalN2 = calcCoordForcaRes(forcaFinalN2, forcaProj, fr);
                }
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
            
            nave2.vel_x += aceleracao * delta_t;
            nave2.vel_y += aceleracao * delta_t;

            nave2.coordenadas.pos_x += direcaoX * nave2.vel_x * delta_t;
            nave2.coordenadas.pos_y += direcaoY * nave2.vel_y * delta_t;

            /*PROJETEIS*/
            while(proj != NULL && totalProjeteis > 0){
                if(proj->ativo == 1){
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

                    if ( forcaFinalP.fim.pos_x - forcaFinalP.inicio.pos_x < 0) {
                        direcaoX =  -1; 
                    }else{
                        direcaoX = 1;
                    }
                
                    if(forcaFinalP.fim.pos_y - forcaFinalP.inicio.pos_y < 0){
                        direcaoY = -1;
                    }else{
                        direcaoY = 1;
                    }

                    coordProjeteis[k].pos_x += direcaoX * nave2.vel_x * delta_t;
                    coordProjeteis[k].pos_y += direcaoY * nave2.vel_y * delta_t;

                    proj->vel_x += aceleracao * delta_t;
                    proj->vel_y += aceleracao * delta_t;

                    exibeProjetil(proj, forcaFinalP, coordXMax, coordYMax, P1);
                    k++;
                }
                proj = proj->prox;
            }

            /***
             *IMPRESSÃO DAS COORDENADAS
            */       
            printf("******Tempo %f\n", i);
            printf("NAVE 1: (%lf , %lf)\n", nave1.coordenadas.pos_x, nave1.coordenadas.pos_y);
            nave1.coordenadas = posicaoToroidal(nave1, width, height);
            if( !exibiuNave1 )
                coordJanelaN1 = exibeNaveAngulo(nave1, (anguloInicial1)/10, coordXMax, coordYMax, msks, pics, P1, P2, 1);

            printf("NAVE 2: (%lf , %lf)\n", nave2.coordenadas.pos_x, nave2.coordenadas.pos_y);
            nave2.coordenadas = posicaoToroidal(nave2, width, height);
            if( !exibiuNave2 )
                coordJanelaN2 = exibeNaveAngulo(nave2, (anguloInicial2)/10, coordXMax, coordYMax, msks, pics, P1, P2, 2);

            vencedor = detectaColisao(coordJanelaN1, coordJanelaN2, listaProjeteis);

            PutPic(w1, P1, 0, 0, 800, 670, 0, 0);
            aux = listaProjeteis;
            WFlush();
            usleep(10000);
            WClear(w1);
            PutPic(P1, P3, 0, 0, 800, 670, 0, 0);
            printf("\n\n");
        }
        if(vencedor < 0)
            snprintf(textoProj, 29, "O tempo acabou. Empate!");
        else if(vencedor == 0){
            snprintf(textoProj, 29, "As naves colidiram. Empate!");
            nave2.quantVidasRestantes--;
            nave1.quantVidasRestantes--;
        }
        else if(vencedor == 1){
            snprintf(textoProj, 29, "Jogador 1 venceu!");
            nave2.quantVidasRestantes--;
        }
        else{
            snprintf(textoProj, 29, "Jogador 2 venceu!");
            nave1.quantVidasRestantes--;
        }
        
        WPrint(P1, 340, 638, textoProj );
        snprintf(textoProj, 50, "Aperte alguma tecla para iniciar o proximo round");
        WPrint(P1, 260, 658, textoProj );
        PutPic(w1, P1, 0, 0, 800, 670, 0, 0);

        while(!WCheckKBD(w1));
    }
    return 0;
}