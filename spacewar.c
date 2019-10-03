#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "spacewar.h"
#define G 6.674e-11

void insereProjetil(double mas, double posx, double posy, double velx, double vely, Projetil** inicio);

double distanciaEntrePontos(Coordenada c1, Coordenada c2);

double forcaGravitacional(double m1, double m2, double d);

Forca normalizaForca (Coordenada inicio, Coordenada fim, double intensidade);

double forcaResultante(Forca forca1, Forca forca2);

Forca calcCoordForcaRes (Forca forca1, Forca forca2, double intensidade);

void insereProjetil(double mas, double posx, double posy, double velx, double vely, Projetil** inicio){
    Projetil* nova;
    nova = malloc(sizeof(Projetil));
    nova -> massa = mas;
    nova -> coordenadas.pos_x = posx;
    nova -> coordenadas.pos_y = posy;
    nova -> vel_x = velx;
    nova -> vel_y = vely;
    nova -> prox = *inicio;
    *inicio = nova;
}

double distanciaEntrePontos(Coordenada c1, Coordenada c2){
    double distancia = sqrt(pow(c2.pos_x - c1.pos_x ,2) + pow(c2.pos_y - c1.pos_y ,2));

    return distancia;
}

double forcaGravitacional(double m1, double m2, double d){
    double forca;
    forca = G*(m1 * m2) / (pow(d,2));

    return forca;
}

Forca normalizaForca (Coordenada inicio, Coordenada fim, double intensidade){
    Forca f;
    Coordenada c;
    double norma;

    f.intensidade = intensidade;
    f.inicio.pos_x = inicio.pos_x;
    f.inicio.pos_y = inicio.pos_y;
    norma = distanciaEntrePontos(inicio, fim);

    if(intensidade <= eps || norma <=eps){
        f.fim.pos_x = inicio.pos_x;
        f.fim.pos_y = inicio.pos_y;
        f.intensidade = 0;
    }else{
        c.pos_x = (fim.pos_x - inicio.pos_x)/norma;
        c.pos_y = (fim.pos_y - inicio.pos_y)/norma;

        f.fim.pos_x = c.pos_x * intensidade;
        f.fim.pos_y = c.pos_y * intensidade;
    }    

    return f;
}

double forcaResultante(Forca forca1, Forca forca2){
    double produtoEscalar, produtoNormas, cosForca;
    Coordenada v1, v2;
    double forcaResultante;

    v1.pos_x = (forca1.fim.pos_x - forca1.inicio.pos_x);
    v1.pos_y = (forca1.fim.pos_y - forca1.inicio.pos_y);

    v2.pos_x = (forca2.fim.pos_x - forca2.inicio.pos_x);
    v2.pos_y = (forca2.fim.pos_y - forca2.inicio.pos_y);

    produtoEscalar = ((v1.pos_x) * (v2.pos_x)) + ((v1.pos_y) * (v2.pos_y));
    produtoNormas = forca1.intensidade * forca2.intensidade;
    if (produtoNormas != 0) {
        cosForca = produtoEscalar / produtoNormas;
        forcaResultante = sqrt(pow(forca1.intensidade,2)+pow(forca2.intensidade,2)+2*forca1.intensidade*forca2.intensidade*cosForca);
    }else{
        if(forca1.intensidade == 0 && forca2.intensidade != 0){
            forcaResultante = forca2.intensidade;
        }else if(forca2.intensidade == 0 && forca1.intensidade != 0){
            forcaResultante = forca1.intensidade;
        }else{
            forcaResultante = 0;
        }
    }

    return forcaResultante;
}


Forca calcCoordForcaRes (Forca forca1, Forca forca2, double intensidade){
    Coordenada fr;
    Forca forcaResultante;
    if( fabs(forca1.fim.pos_x - forca2.fim.pos_x) <= eps){
        printf("Alinhados em x\n");
        /*os pontos estão alinhados em algum x*/
        fr.pos_x = forca1.fim.pos_x;

        if ( forca1.intensidade > forca2.intensidade ) {
            fr.pos_y = forca1.fim.pos_y - forca2.fim.pos_y;
        }else{
            fr.pos_y = forca2.fim.pos_y - forca1.fim.pos_y;
        }
    }else if( fabs(forca1.fim.pos_y - forca2.fim.pos_y) <= eps ){
        printf("Alinhados em y");
        /*os pontos estão alinhados em algum y*/
        fr.pos_y = forca1.fim.pos_y;

        if ( forca1.intensidade > forca2.intensidade ) {
            fr.pos_x = forca1.fim.pos_x - forca2.fim.pos_x;
        }else{
            fr.pos_x = forca2.fim.pos_x - forca1.fim.pos_x;
        }
    }else{
        /*os pontos estão desalinhados, aplico a regra do palalelogramo
        somando os vetores. Assim, consigo a coordenada do ponto final
        da forca resultante*/
        printf("Desalinhadso\n");
        fr.pos_x = forca1.fim.pos_x - forca2.fim.pos_x;
        fr.pos_y = forca1.fim.pos_y - forca2.fim.pos_y;
    }

    forcaResultante.inicio.pos_x = forca1.inicio.pos_x;
    forcaResultante.inicio.pos_y = forca1.inicio.pos_y;
    forcaResultante.fim.pos_x = fr.pos_x;
    forcaResultante.fim.pos_y = fr.pos_y;
    forcaResultante.intensidade = intensidade;
    forcaResultante = normalizaForca(forcaResultante.inicio, forcaResultante.fim, intensidade);

    return forcaResultante;
}