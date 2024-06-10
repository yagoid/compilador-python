%{

// ----------------------------- GLOSARIO DE IMPORTS -------------------------------------------
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "tabla_simbolos.h"
#include "AST_python.h"

// ----------------------------- DECLARACION DE VARIABLES Y ESTRUCTURAS -------------------------------------------

//Declaracion de variables "extern" sirve para declararlas como variables globales
FILE *yyout;
extern FILE* yyin;
extern int num_linea; //Almacena el numero de linea durante la ejecucion
extern tSimbolos tabla[256];
extern int indice; //Se almacena el índice de la tabla de tSimbolos
char* tipos[] = {"numero", "decimal", "texto", "bool"}; //Para parsear el tipo que se detecta en flex al tipo del nodo


%}

/*Definicion de tipos y estructuras empleadas*/
%union {
  int intVal;
  float realVal;
  char* strVal;
  struct atributos{
    int numero;
    float decimal;
    char* texto;
    char* tipo;             //Define el tipo que se esta usando
    struct ast *n;          //Para almacenar los nodos del AST
  }tr;
}

/*Declaración de los TOKENS*/
%token FALSE NONE TRUE AND AS ASSERT ASYNC AWAIT BREAK CONTINUE CLASS DEF DEL ELIF ELSE EXCEPT FINALLY 
%token FOR FROM GLOBAL IF IMPORT IN IS LAMBDA NONLOCAL NOT OR PASS RAISE RETURN TRY WHILE WITH YIELD END IMPRIMIR 
%token CADENA VECTOR LISTA TUPLA SET DICT INT FLOAT COMPLEX BOOLEAN 
%token SUMA RESTA MULTIPLICACION DIVISION MODULO MENOR_QUE MAYOR_QUE AUMENTAR_VALOR IGUAL_QUE DISTINTO_QUE ASIGNACION PARENTESIS_IZQ PARENTESIS_DER  

/*Declaración de los TOKENS que provienen de FLEX con su respectivo tipo*/
%token <intVal> NUMERO 
%token <realVal> DECIMAL 
%token <strVal> VARIABLE
%token <strVal> STRING

/*Declaración de los TOKENS NO TERMINALES con su estructura*/
%type <tr> sentencias sentencia tipos expresion asignacion imprimir  

/*Declaración de la precedencia siendo menor la del primero y mayor la del último*/
%left SUMA RESTA MULTIPLICACION

%start codigo
%%

//GRAMATICA
//X --> S
//S --> D | S D
//D --> A | I 
//A --> id = E 
//E --> E op T | T
//T --> id | num | decimal
//I --> imprimir ( E )

//-----------------------------------------------  PRODUCCIONES  -------------------------------------------------------

//PRODUCCION "codigo", formado por sentencias
//X --> S
codigo:
    sentencias  {
        comprobarAST($1.n); 
        printf("\n[FINALIZADO]\n");     
    }
;

//PRODUCCION "sentencias", puede estar formado por una sentencia o un grupo de sentencias
//S --> D | S D
sentencias:
    sentencia
    | sentencias sentencia { //para hacerlo recursivo
        $$.n = crearNodoNoTerminal($1.n, $2.n, 7);
    }
;

//PRODUCCION "sentencia", puede estar formado por asignaciones, condicionales, bucles whiles, imprimir
//D --> A | I 
sentencia:   //Por defecto bison, asigna $1 a $$ por lo que no es obligatoria realizar la asignacion
    asignacion              
    | imprimir       
;

//-------------------------------------------------------- ASIGNACION --------------------------------------------------------
//PRODUCCION "asignacion", formado por un identificador, un igual y una expresion
//A --> id = E 
asignacion:
    VARIABLE ASIGNACION expresion {
        printf("> [SENTENCIA] - Asignacion\n");

        //Para crear un nuevo simbolo de tipo numero
        if(strcmp($3.tipo, tipos[0]) == 0){ //comprobacion si es numero
        printf("Asignado el valor %d a la variable\n",$3.numero);
        tabla[indice].nombre = $1; tabla[indice].tipo = tipos[0]; tabla[indice].numero = $3.numero; tabla[indice].registro = $3.n->resultado;
        indice++; //incrementamos el valor del inidice para pasar a la siguiente posicion y dejar la anterior guardada
        }
        //Para crear un nuevo simbolo de tipo decimal
        else if(strcmp($3.tipo, tipos[1]) == 0){ //comprobacion si es decimal
        printf("Asignado el valor %d a la variable\n",$3.decimal);
        tabla[indice].nombre = $1; tabla[indice].tipo = tipos[1]; tabla[indice].decimal = $3.decimal; tabla[indice].registro = $3.n->resultado;
        indice++; //incrementamos el valor del inidice para pasar a la siguiente posicion y dejar la anterior guardada
        }
        $$.n=crearNodoNoTerminal($3.n, crearNodoVacio(), 5);
    }

//-----------------------------------------------  EXPRESION ---------------------------------------------
//PRODUCCION "expresion", en esta gramática se representa la suma, resta y otros terminos
//E --> E op T | T
expresion:
    
    //SUMA
    expresion SUMA tipos {

        //Suma de numero + numero
        if (strcmp($1.tipo, tipos[0]) == 0 && strcmp($3.tipo, tipos[0]) == 0) { //comprobacion del tipo
            printf("> [OPERACION] - SUMA {numero / numero}\n");
            $$.n = crearNodoNoTerminal($1.n, $3.n, 2);
            $$.tipo = tipos[0]; $$.numero = $1.numero + $3.numero;
        }

        //Suma de decimal + decimal
        else if (strcmp($1.tipo, tipos[1]) == 0 && strcmp($3.tipo, tipos[1]) == 0){  //comprobacion del tipo
            printf("> [OPERACION] - SUMA {decimal / decimal}\n");
            $$.n = crearNodoNoTerminal($1.n, $3.n, 2);
            $$.tipo = tipos[1]; $$.decimal = $1.decimal + $3.decimal;
        }

        //Suma de str + str
        else if (strcmp($1.tipo, tipos[2]) == 0 && strcmp($3.tipo, tipos[2]) == 0){  //comprobacion del tipo
            printf("> [OPERACION] - SUMA {texto / texto}\n");
            $$.n = crearNodoNoTerminal($1.n, $3.n, 2);
            $$.tipo = tipos[1]; $$.texto = $1.texto + $3.texto;
        }
    }
    //RESTA
    | expresion RESTA tipos {
        
        //Resta de numero - numero
        if (strcmp($1.tipo, tipos[0]) == 0 && strcmp($3.tipo, tipos[0]) == 0) {  //comprobacion del tipo
            printf("> [OPERACION] - RESTA {numero / numero}\n");
            $$.n = crearNodoNoTerminal($1.n, $3.n, 3);
            $$.tipo = tipos[0]; $$.numero = $1.numero + $3.numero;
        }
        //Resta de decimal - decimal
        else if (strcmp($1.tipo, tipos[1]) == 0 && strcmp($3.tipo, tipos[1]) == 0){  //comprobacion del tipo
            printf("> [OPERACION] - RESTA {decimal / decimal}\n");
            $$.n = crearNodoNoTerminal($1.n, $3.n, 3);
            $$.tipo = tipos[1]; $$.decimal = $1.decimal + $3.decimal;
        }
    }
    //MULTIPLICACION
    | expresion MULTIPLICACION tipos {
        
        //Multiplicacion de numero * numero
        if (strcmp($1.tipo, tipos[0]) == 0 && strcmp($3.tipo, tipos[0]) == 0) {  //comprobacion del tipo
            printf("> [OPERACION] - MULTIPLICACION {numero / numero}\n");
            $$.n = crearNodoNoTerminal($1.n, $3.n, 4);
            $$.tipo = tipos[0]; $$.numero = $1.numero + $3.numero;
        }
        //Multiplicacion de decimal * decimal
        else if (strcmp($1.tipo, tipos[1]) == 0 && strcmp($3.tipo, tipos[1]) == 0){  //comprobacion del tipo
            printf("> [OPERACION] - MULTIPLICACION {decimal / decimal}\n");
            $$.n = crearNodoNoTerminal($1.n, $3.n, 4);
            $$.tipo = tipos[1]; $$.decimal = $1.decimal + $3.decimal;
        }
    }
    //DIVISION
    | expresion DIVISION tipos {
        
        //Division de numero / numero
        if (strcmp($1.tipo, tipos[0]) == 0 && strcmp($3.tipo, tipos[0]) == 0) {  //comprobacion del tipo
            printf("> [OPERACION] - DIVISION {numero / numero}\n");
            $$.n = crearNodoNoTerminal($1.n, $3.n, 5);
            $$.tipo = tipos[0]; $$.numero = $1.numero + $3.numero;
        }
        //Division de decimal / decimal
        else if (strcmp($1.tipo, tipos[1]) == 0 && strcmp($3.tipo, tipos[1]) == 0){  //comprobacion del tipo
            printf("> [OPERACION] - DIVISION {decimal / decimal}\n");
            $$.n = crearNodoNoTerminal($1.n, $3.n, 5);
            $$.tipo = tipos[1]; $$.decimal = $1.decimal + $3.decimal;
        }
    }
    | tipos {$$ = $1;} //la produccion operacion puede ser tipos, un subnivel para realizar la jerarquia de operaciones
;

//-----------------------------------------------  TIPOS  ---------------------------------------------
/*PRODUCCION "tipos", en esta gramática se represetan los tipos de datos:
- identificadores (variables) - numeros enteros o decimales positivos o negativos
- cadenas de texto - estructura parentesis
T --> id | num | decimal | texto*/
tipos:

    //Identificador
    VARIABLE {
        printf(" VARIABLE %s\n",$1);
        //Buscamos en la tabla el VARIABLE
        if(buscarTabla(indice, $1, tabla) != -1){     //En este IF entra si buscarTabla devuelve la posicion
            int pos = buscarTabla(indice, $1, tabla);
            //Para si es de tipo numero
            if(tabla[pos].tipo==tipos[0]){
                $$.tipo = tabla[pos].tipo; $$.numero=tabla[pos].numero; 
                $$.n = crearVariableTerminal(tabla[pos].numero, tabla[pos].registro);  //Creamos un nodo terminal con los numeros   
            }
            //Para si es de tipo decimal
            else if(tabla[pos].tipo==tipos[1]){
                $$.tipo = tabla[pos].tipo; $$.decimal=tabla[pos].decimal;
                $$.n = crearVariableTerminal(tabla[pos].decimal, tabla[pos].registro); //Creamos un nodo terminal con los numeros        
            }
            //Para si es de tipo texto
            else if(tabla[pos].tipo==tipos[2]){
                $$.tipo = tabla[pos].tipo; $$.texto=tabla[pos].texto;
                $$.n = crearVariableTerminal(tabla[pos].texto, tabla[pos].registro); //Creamos un nodo terminal con el texto        
            }
        }
    }

    //Numero entero normal
    | NUMERO {
        $$.numero = $1;
        printf("\n> [TIPO] - Numero Positivo: %ld\n", $$.numero);
        $$.n = crearNodoTerminal($1); 
        $$.tipo = tipos[0]; 
    }

    //Numero decimal normal
    | DECIMAL {
        $$.decimal = $1;
        printf("\n> [TIPO] - Decimal: %.3f\n", $$.decimal); 
        $$.n = crearNodoTerminal($1); 
        $$.tipo = tipos[1];  
    }

    //Numero entero normal
    | STRING {
        $$.numero = $1;
        printf("\n> [TIPO] - Texto: %ld\n", $$.texto);
        $$.n = crearNodoTerminalString($1); 
        $$.tipo = tipos[0]; 
    }
;

//-----------------------------------------------  IMPRIMIR  ---------------------------------------------
//Representa la estructura del print en lenguaje latino
//I --> imprimir ( E ) 
imprimir: 
    IMPRIMIR PARENTESIS_IZQ expresion PARENTESIS_DER { 
        printf("> [SENTENCIA] - Imprimir\n");
        $$.n = crearNodoNoTerminal($3.n, crearNodoVacio(), 4);        
    }
;

%% 

//--------------------------------------------------- METODO MAIN -----------------------------------------------
int main(int argc, char** argv) {
    yyin = fopen(argv[1], "rt");            //Apertura del archivo codigo.latino
    yyout = fopen( "./python.asm", "wt" );  //Para el archivo .ASM con nombre "latino.asm"
	yyparse();
    fclose(yyin);
    return 0;
}

//Metodo yyerror, generado por defecto
void yyerror(const char* s) {
    fprintf(stderr, "%s\n", s);
}