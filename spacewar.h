/* STRUCTS PARA A MANIPULAÇÃO DE DADOS */
#define eps 1e-9

struct coordenada{
    double pos_x, pos_y;
};
typedef struct coordenada Coordenada;

struct forca{
    Coordenada inicio;
    Coordenada fim;
    double intensidade;
};
typedef struct forca Forca;

struct nave{
    char nome[3];
    double massa, vel_x, vel_y;
    Coordenada coordenadas;
    int quantProjeteisRestantes;
    int quantVidasRestantes;
};
typedef struct nave Nave;

struct projetil{
    double massa, vel_x, vel_y;
    Coordenada coordenadas;
    int tempoDeVidaRestante;
    int ativo;
    struct projetil* prox;
};
typedef struct projetil Projetil;

struct planeta{
    double raio, massa;
    Coordenada coordenadas;
};
typedef struct planeta Planeta;

/* PROTÓTIPOS DAS FUNÇÕES PARA MANIPULAR AS FORÇAS */

double calculaAngulo(Forca f);

void insereProjetil(double mas, double posx, double posy, double velx, double vely, Projetil** inicio);

double distanciaEntrePontos(Coordenada c1, Coordenada c2);

double forcaGravitacional(double m1, double m2, double d);

Forca normalizaForca (Coordenada inicio, Coordenada fim, double intensidade);

double forcaResultante(Forca forca1, Forca forca2);

Forca calcCoordForcaRes (Forca forca1, Forca forca2, double intensidade);

Coordenada posicaoToroidal(Nave nave, long width, long height);

void rotacionaNave(Nave* nave, double rad);

void aceleraNave(Nave* nave);