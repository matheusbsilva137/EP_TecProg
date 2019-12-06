%{
        #include "spacewar.h"
        Planeta  planetaJogo;
        Nave nave1, nave2;
        double tmpTotalSimul, tempoDeVida,qntProjeteis;       
        int cont = 0;
        
        
%}
FLOAT ([0-9]+\.?[0-9]*){1}(e[+-]?[0-9]+)?
NUM [0-9]+\.?[0-9]*
NOME [A-Za-z]{3}
INT [0-9]+
COORDENADA (-?[0-9]+\.?[0-9]*){1}(e[+-]?[0-9]+)?
%%
^{FLOAT}\ {FLOAT}\ {NUM}$ {
    planetaJogo.raio= atof(yytext);       
    while (yytext[0] != ' ') 
    yytext++;
    yytext++;
    planetaJogo.massa= atof(yytext);      
    while (yytext[0] != ' ') 
    yytext++; 
    yytext++;    
    tmpTotalSimul = atof(yytext);       
    printf("O tamanho é: %lf, a massa é: %lf, o tempo é: %lf\n", planetaJogo.raio, planetaJogo.massa, tmpTotalSimul);    
}
^{NOME}\ {FLOAT}\ {COORDENADA}\ {COORDENADA}\ {NUM}\ {NUM}$ {
    if (cont == 0){
    nave1.nome[0]= yytext[0]; 
    nave1.nome[1]= yytext[1];
    nave1.nome[2]= yytext[2];        
    while (yytext[0] != ' ') 
    yytext++;
    yytext++;
    nave1.massa= atof(yytext);      
    while (yytext[0] != ' ') 
    yytext++; 
    yytext++;  
    nave1.coordenadas.pos_x= atof(yytext);      
    while (yytext[0] != ' ') 
    yytext++; 
    yytext++;   
    nave1.coordenadas.pos_y= atof(yytext);      
    while (yytext[0] != ' ') 
    yytext++; 
    yytext++;  
    nave1.vel_x= atof(yytext);      
    while (yytext[0] != ' ') 
    yytext++; 
    yytext++;    
    nave1.vel_y= atof(yytext);    
    printf("O nome da nave 1 e %s, sua massa e %lf, sua posicao x e %lf, sua posicao y e %lf, sua velocidade x e %lf, sua velocidade y e %lf\n", nave1.nome, nave1.massa, nave1.coordenadas.pos_x, nave1.coordenadas.pos_y, nave1.vel_x, nave1.vel_y);
    cont++;
}

else {
    nave2.nome[0]= yytext[0]; 
    nave2.nome[1]= yytext[1];
    nave2.nome[2]= yytext[2];        
    while (yytext[0] != ' ') 
    yytext++;
    yytext++;
    nave2.massa= atof(yytext);      
    while (yytext[0] != ' ') 
    yytext++; 
    yytext++;  
    nave2.coordenadas.pos_x= atof(yytext);      
    while (yytext[0] != ' ') 
    yytext++; 
    yytext++;   
    nave2.coordenadas.pos_y= atof(yytext);      
    while (yytext[0] != ' ') 
    yytext++; 
    yytext++;  
    nave2.vel_x= atof(yytext);      
    while (yytext[0] != ' ') 
    yytext++; 
    yytext++;    
    nave2.vel_y= atof(yytext);    
    printf("O nome da nave 2 e %s, sua massa e %lf, sua posicao x e %lf, sua posicao y e %lf, sua velocidade x e %lf, sua velocidade y e %lf\n", nave2.nome, nave2.massa, nave2.coordenadas.pos_x, nave2.coordenadas.pos_y, nave2.vel_x, nave2.vel_y);
    }
}


 
\n

<<EOF>> {
    yyterminate();
    }
%%

int main (int arc, char **argv) {
yyin = fopen(argv[1], "r");
if ( !yyin )
yypush_buffer_state(yy_create_buffer( yyin, YY_BUF_SIZE ));
    yylex();

}
