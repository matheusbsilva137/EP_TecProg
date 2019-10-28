#include <stdio.h>
double calculaAngulo(double xIni,double xFin, double yIni, double yFim){
    /*Determina o angulo entre o vetor (força) direcional da nave e uma reta paralela ao eixo x
    O angulo retornado está entre 0 e 2Pi*/
    double angulo, y;

    y = (yFim - yIni);

    angulo = atan(x/y) + M_PI_2;
    if( y > 0 )
        return (angulo + M_PI_2)%2*M_PI;
    else
        return (angulo + M_PI)%2*M_PI + M_PI_2;
    
}

int main(){
    double xi, xf, yi, yf;
    xi = -2;
    xf = 0;
    yi = -2
    yf = 0;
    angulo = calculaAngulo() 
}