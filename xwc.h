#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#ifndef NOXPM
#  include <X11/xpm.h>
#endif

typedef unsigned long Color;

typedef struct {
  GC gc;
  union {
	Window window;
	Pixmap m;
	Pixmap p;
  } ptr;	
} WINDOW;

typedef WINDOW *PIC;

typedef WINDOW *MASK;


/* Em todas as funções de desenho e cópia de região variáveis dos tipos
   WINDOW*, PIC e MASK podem ser utilizadas normalmente */
/* Define a cor para a janela win */
void WCor(WINDOW *win, Color c);

/* Desenha um ponto de cor c, na posicao (x,y) da janela win */
void WPlot(WINDOW *win, int x, int y, Color c);
	
/* Desenha uma linha de cor c, na janela win, entre os pontos 
 *  (x1,y1) e (x2,y2) 
 */
void WLine(WINDOW *win, int x1, int y1, int x2, int y2, Color c);

/* Desenha um retangulo no ponto (x,y) de altura h e largura w */
void WRect(WINDOW *win, int x, int y, int w, int h, Color c);
/* Idem, mas o retangulo e' cheio */
void WFillRect(WINDOW *win, int x, int y, int w, int h, Color c);

/* Desenha um arco de elipse na posicao (x,y), inscrito em um retangulo
 * de altura h w largura w, iniciando no angulo a1 e terminando em a2
 * Os angulos sao dados em graus/64, isto e' 64 correspnde a um grau.
 * Exemplo, para desenhar um quadrante de um circulo:
 *   WFillArc(win, 100,100, 0, 90*64, 20, 20, 3);
 */
void WArc(WINDOW *win, int x, int y, int a1, int a2, int w, int h, Color c);
void WFillArc(WINDOW *win, int x, int y, int a1, int a2, int w, int h, Color c);

/* Limpa a janela win */
void WClear(WINDOW *win);

/* Força a atualização das janelas */
void WFlush();

/* Escreve msg na posicao (x,y) da janela win */
void WPrint(WINDOW *win, int x, int y, char *msg);

/* cria um PIC no formato interno da janela win, de largura w e altura h */
PIC NewPic(WINDOW *win, int w, int h);

/* destroy o PIC */
void FreePic(PIC pic);

/* desenha o PIC pic2, em pic1, posicao (x,y). x0, y0, w e h indicam 
 * o pedaco (retangulo) de pic2  a ser usado */
void PutPic(PIC pic1, PIC pic2, int x0, int y0, int w, int h, int x, int y);

/* escolhe uma cor pelo nome. Retorna o valor */
Color WNamedColor(char *name);

/* mostra a janela na tela */
void WShow(WINDOW *win);
/* esconde a janela */
void WHide(WINDOW *win);
/* destroy a janela */
void WDestroy(WINDOW *win);

/* Cria uma janela de altura HEIGHT e largura WIDTH, com nome name */
WINDOW* InitGraph(int WIDTH, int HEIGHT, char *name);

/* Finaliza o sistema grafico */
void CloseGraph();

/* Mascaras */
/* Cria uma mascara nova */
MASK NewMask(WINDOW *w, int wd, int h);

/* Ativa a mascara para a janela w */
void SetMask(PIC p, MASK mask);

/* Desativa a mascara */
void UnSetMask(PIC p);

/* Misc. */
/* Retorna o display */
Display *GetDisplay();

/* Seleciona o "drawable" */
#define GetDraw(X) (X)->ptr.p

/* Seleciona o "gc" */
#define GetGC(X) (X)->gc

#ifndef NOXPM
/* XPM */
/* No formato Xpm, a cor transparente é indicada por
 * 'None'.  Esta informação é usada para especificar
 * uma máscara ao mesmo tempo que se indica a figura
 */

/* Lê um arquivo xpm e retorna o PIC correspondente 
 * retorna a mascara associada em 'm', se este for
 * diferente de zero.
 */
PIC ReadPic(WINDOW *w, char *fname, MASK m);

/* Idem, mas lê da memória ao invés do arquivo */
PIC MountPic(WINDOW *w, char **data, MASK m);

/* escreve o conteúdo do PIC p no arquivo fname
 * mascarando com m, se m for diferente de zero
 */
int WritePic(PIC p, char *fname, MASK m);
#endif


/* Inicializa a captura de eventos de teclado */
void InitKBD(WINDOW *w);


/* Verifica se existe alguma tecla disponível */
int WCheckKBD(WINDOW *w);

/* pega a próxima tecla e retorna o código X.
 * Veja em $INC/X11/keysymdef.h */
KeyCode WGetKey(WINDOW *w);

/* Retorna o KeySym da última tecla lida */
KeySym WLastKeySym();
