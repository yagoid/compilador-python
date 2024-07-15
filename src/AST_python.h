// ----------------------------- GLOSARIO DE IMPORTS -------------------------------------------
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// ----------------------------- DECLARACION DE VARIABLES Y ESTRUCTURAS --------------------------------------------

#define NUM_REG_V 2
#define NUM_REG_A 4
#define NUM_REG_T 10
#define NUM_REG_S 8
#define NUM_REG_F 32

extern FILE *yyout;
int contadorEtiqueta = 0; // Variable para el control de las etiquetas
int numMaxRegistros = 32; // Variable que indica el numero maximo de registros disponibles
int nombreVariable = 0;   // Almacena el entero que se asociará al nombre de la variable

// Por defecto, tenemos 32 registros de tipo f para controlar los registros libres (true) o ocupados (false)
bool registros[32] = {
    true, true, true, true, true, true, true, true,
    true, true, true, true, true, true, true, true,
    true, true, true, true, true, true, true, true,
    true, true, true, true, true, true, false, false};
// Los registros 30 y 31 están reservados por defecto para imprimir por pantalla

bool cadenaConcatenada = false;

// typedef struct
// {
//     bool reg_v[NUM_REG_V]; // Registros para valores de retorno de funciones
//     bool reg_a[NUM_REG_A]; // Argumentos para funciones
//     bool reg_t[NUM_REG_T]; // Registros temporales
//     bool reg_s[NUM_REG_S]; // Preservados por las subrutinas
//     bool reg_f[NUM_REG_F]; // Punto Flotante
// } Registros;

// Registros registros;

// Estructura variable, se hará uso de la misma para almacenar e imprimir las variables del codigo python
struct variable
{
    // char *nombre;      // Nombre de la variable, utilizando un puntero a char para nombres dinámicos
    double valorFloat; // Valor de la variable (en caso de ser flotante)
    int valorEntero;   // Valor de la variable (en caso de ser entero)
    char *valorCadena; // Valor de la variable (en caso de ser cadena)
    int valorBoolean;  // Valor de la variable (en caso de ser booleano)
    int nombre;        // limite de caracteres de la variable
    bool disponible;   // Indica si la variable está disponible
    char *tipo;        // Tipo de la variable: "int", "float", "string", "boolean".
};

struct variable variables[64]; // Declaramos el array de variables usando la estructura definida

// Estructura AST, se define la estructura de los nodos del arbol
struct ast
{
    struct ast *izq;    // Nodo izquierdo del arbol
    struct ast *center; // Nodo centro del arbol para if
    struct ast *dcha;   // Nodo derecho del arbol
    int tipoNodo;       // Almacena el tipo de nodo
    char tipoReg;       // Almacena el tipo de registro: v, a, t, s o f
    union
    {
        int valorEntero;     // Valor si es entero
        double valorDecimal; // Valor si es decimal
        char *valorCadena;   // Valor si es una cadena
        int valorBoolean;    // Valor si es una Boolean
    };
    // double valor;        // Almacena el valor del nodo
    char *tipo;    // Tipo de dato: "int", "float", "string", "boolean"
    int resultado; // Registro donde está el resultado
    int nombreVar; // Indica el nombre de la variable
};

//-----------------------------------------------  METODOS -------------------------------------------------------

// Función para comprobar el valor del nodo y generar el código correspondiente
struct ast *comprobarValorNodo(struct ast *n, int contadorEtiquetaLocal)
{
    struct ast *reg;

    switch (n->tipoNodo)
    {
    case 1: // Nueva hoja en el árbol
        printf("1\n");
        reg = n;
        // printf("tipo nueva variable: %s\n", n->tipo);

        if (strcmp(n->tipo, "int") == 0)
        {
            fprintf(yyout, "lw $t%d, var_%d     # Cargar var_%d en $t%d\n", n->resultado, n->nombreVar, n->nombreVar, n->resultado);
        }
        else if (strcmp(n->tipo, "float") == 0)
        {
            fprintf(yyout, "lwc1 $f%d, var_%d   # Cargar var_%d en $f%d\n", n->resultado, n->nombreVar, n->nombreVar, n->resultado);
        }
        else if (strcmp(n->tipo, "string") == 0)
        {
            fprintf(yyout, "la $t%d, var_%d     # Cargar var_%d en $t%d\n", n->resultado, n->nombreVar, n->nombreVar, n->resultado);
        }
        else if (strcmp(n->tipo, "boolean") == 0)
        {
            fprintf(yyout, "lw $t%d, var_%d     # Cargar var_%d en $t%d\n", n->resultado, n->nombreVar, n->nombreVar, n->resultado);
        }
        break;

    case 2: // Nueva suma
        printf("2\n");
        reg = comprobarValorNodo(n->izq, contadorEtiquetaLocal);
        comprobarValorNodo(n->dcha, contadorEtiquetaLocal);

        if (strcmp(n->izq->tipo, "int") == 0)
        {
            // fprintf(yyout, "add $t%d, $t%d, $t%d\n", n->resultado, n->izq->resultado, n->dcha->resultado);
            fprintf(yyout, "add $t%d, $t%d, $t%d    # Sumar $t%d y $t%d, guardar en $t%d\n", n->resultado, n->izq->resultado, n->dcha->resultado, n->izq->resultado, n->dcha->resultado, n->resultado);
        }
        else if (strcmp(n->izq->tipo, "float") == 0)
        {
            // fprintf(yyout, "add.s $f%d, $f%d, $f%d\n", n->resultado, n->izq->resultado, n->dcha->resultado);
            fprintf(yyout, "add.s $f%d, $f%d, $f%d  # Sumar $f%d y $f%d, guardar en $f%d\n", n->resultado, n->izq->resultado, n->dcha->resultado, n->izq->resultado, n->dcha->resultado, n->resultado);
        }
        else if (strcmp(n->izq->tipo, "string") == 0)
        {
            // registro t6 para el puntero de la cadena
            fprintf(yyout, "la $s0, resultado\n");

            fprintf(yyout, "cadena_%d: \n", n->izq->resultado);
            fprintf(yyout, "  lb $s1, 0($t%d)\n", n->izq->resultado);
            fprintf(yyout, "  beqz $s1, finCadena_%d\n", n->izq->resultado);
            fprintf(yyout, "  sb $s1, 0($s0)\n");
            fprintf(yyout, "  addi $s0, $s0, 1\n");
            fprintf(yyout, "  addi $t%d, $t%d, 1\n", n->izq->resultado, n->izq->resultado);
            fprintf(yyout, "  j cadena_%d\n", n->izq->resultado);

            fprintf(yyout, "finCadena_%d: \n", n->izq->resultado);
            fprintf(yyout, "  la $t%d, var_%d\n", n->dcha->resultado, n->dcha->resultado);

            fprintf(yyout, "cadena_%d: \n", n->dcha->resultado);
            fprintf(yyout, "  lb $s1, 0($t%d)\n", n->dcha->resultado);
            fprintf(yyout, "  beqz $s1, fin_%d\n", n->dcha->resultado);
            fprintf(yyout, "  sb $s1, 0($s0)\n");
            fprintf(yyout, "  addi $s0, $s0, 1\n");
            fprintf(yyout, "  addi $t%d, $t%d, 1\n", n->dcha->resultado, n->dcha->resultado);
            fprintf(yyout, "  j cadena_%d\n", n->dcha->resultado);

            fprintf(yyout, "fin_%d: \n", n->dcha->resultado);
            fprintf(yyout, "  sb $zero, 0($s0)\n"); // Fin de la cadena n->resultado);

            cadenaConcatenada = true;
        }
        borrarReg(n->izq, n->dcha);
        break;

    case 3: // Nueva resta
        printf("3\n");
        reg = comprobarValorNodo(n->izq, contadorEtiquetaLocal);
        comprobarValorNodo(n->dcha, contadorEtiquetaLocal);

        if (strcmp(n->izq->tipo, "int") == 0)
        {
            fprintf(yyout, "sub $t%d, $t%d, $t%d    # Restar $t%d de $t%d, guardar en $t%d\n", n->resultado, n->izq->resultado, n->dcha->resultado, n->dcha->resultado, n->izq->resultado, n->resultado);
        }
        else if (strcmp(n->izq->tipo, "float") == 0)
        {
            fprintf(yyout, "sub.s $f%d, $f%d, $f%d    # Restar $f%d de $f%d, guardar en $f%d\n", n->resultado, n->izq->resultado, n->dcha->resultado, n->dcha->resultado, n->izq->resultado, n->resultado);
        }
        borrarReg(n->izq, n->dcha);
        break;

    case 4: // Nueva multiplicación
        printf("4\n");
        reg = comprobarValorNodo(n->izq, contadorEtiquetaLocal);
        comprobarValorNodo(n->dcha, contadorEtiquetaLocal);

        if (strcmp(n->izq->tipo, "int") == 0)
        {
            fprintf(yyout, "mul $t%d, $t%d, $t%d\n", n->resultado, n->izq->resultado, n->dcha->resultado);
        }
        else if (strcmp(n->izq->tipo, "float") == 0)
        {
            fprintf(yyout, "mul.s $f%d, $f%d, $f%d\n", n->resultado, n->izq->resultado, n->dcha->resultado);
        }
        borrarReg(n->izq, n->dcha);
        break;

    case 5: // Nueva división
        printf("5\n");
        // if (comprobarValorNodo(n->dcha, contadorEtiquetaLocal) == 0)
        // {
        //     gestionarError("Error: División por cero.");
        //     return 0;
        // }
        reg = comprobarValorNodo(n->izq, contadorEtiquetaLocal);
        comprobarValorNodo(n->dcha, contadorEtiquetaLocal);

        if (strcmp(n->izq->tipo, "int") == 0)
        {
            fprintf(yyout, "div $t%d, $t%d, $t%d\n", n->resultado, n->izq->resultado, n->dcha->resultado);
        }
        else if (strcmp(n->izq->tipo, "float") == 0)
        {
            fprintf(yyout, "div.s $f%d, $f%d, $f%d\n", n->resultado, n->izq->resultado, n->dcha->resultado);
        }

        borrarReg(n->izq, n->dcha);
        break;

    case 6: // Nueva variable
        printf("6\n");
        reg = n;
        break;

    case 7: // Lista de sentencias
        printf("7\n");
        reg = comprobarValorNodo(n->izq, contadorEtiquetaLocal);
        comprobarValorNodo(n->dcha, contadorEtiquetaLocal);
        break;

    case 8: // Operación AND
        printf("8\n");
        reg = comprobarValorNodo(n->izq, contadorEtiquetaLocal);
        comprobarValorNodo(n->dcha, contadorEtiquetaLocal);
        fprintf(yyout, "and $t%d, $t%d, $t%d\n", n->resultado, n->izq->resultado, n->dcha->resultado);
        borrarReg(n->izq, n->dcha);
        break;

    case 9: // Operación OR
        printf("9\n");
        reg = comprobarValorNodo(n->izq, contadorEtiquetaLocal);
        comprobarValorNodo(n->dcha, contadorEtiquetaLocal);
        fprintf(yyout, "or $t%d, $t%d, $t%d\n", n->resultado, n->izq->resultado, n->dcha->resultado);
        borrarReg(n->izq, n->dcha);
        break;

    case 10: // Operación NOT
        printf("10\n");
        reg = !comprobarValorNodo(n->izq, contadorEtiquetaLocal);
        fprintf(yyout, "not $t%d, $t%d\n", n->resultado, n->izq->resultado);
        borrarReg(n->izq, n->dcha);
        break;

    case 11: // Bucle while
    {
        printf("11\n");
        int etiquetaInicio = contadorEtiquetaLocal++;
        int etiquetaFin = contadorEtiquetaLocal++;

        fprintf(yyout, "l.s $f29, zero\n");
        fprintf(yyout, "etiqueta_%d:\n", etiquetaInicio);

        // n->izq es la condición del while
        struct ast *reg_cond = comprobarValorNodo(n->izq, contadorEtiquetaLocal);

        // Evaluar la condición del while
        if (strcmp(reg_cond->tipo, "int") == 0 || strcmp(reg_cond->tipo, "boolean") == 0)
        {
            fprintf(yyout, "beqz $t%d, etiqueta_%d    # Si $t%d es 0, saltar a etiqueta de fin\n",
                    n->izq->resultado, etiquetaFin, n->izq->resultado);
        }
        else if (strcmp(reg_cond->tipo, "float") == 0)
        {
            // Evaluar condición para flotantes
            fprintf(yyout, "c.eq.s $f%d, $f29          # Comparar si $f%d es igual a 0.0\n",
                    n->izq->resultado, n->izq->resultado);
            fprintf(yyout, "bc1t etiqueta_%d          # Si es verdadero (igual a 0.0), saltar a etiqueta de fin\n", etiquetaFin);
            fprintf(yyout, "nop\n");
        }

        // n->dcha es el cuerpo del while
        comprobarValorNodo(n->dcha, contadorEtiquetaLocal);

        fprintf(yyout, "j etiqueta_%d\n", etiquetaInicio);
        fprintf(yyout, "etiqueta_%d:\n", etiquetaFin);
    }
    break;

    case 12: // Instrucción if
    {
        printf("12\n");
        int etiquetaFinIf = contadorEtiquetaLocal++;
        int etiquetaFinCondicion = contadorEtiquetaLocal++;

        fprintf(yyout, "l.s $f29, zero\n");
        // n->izq es la condición del if
        struct ast *reg_cond = comprobarValorNodo(n->izq, contadorEtiquetaLocal);

        // Evaluar la condición del if es false
        if (strcmp(reg_cond->tipo, "int") == 0 || strcmp(reg_cond->tipo, "boolean") == 0)
        {
            fprintf(yyout, "beqz $t%d, etiqueta_%d    # Si $t%d es 0, saltar a etiqueta fin if\n",
                    n->izq->resultado, etiquetaFinIf, n->izq->resultado);

            // n->center: cuerpo del if
            comprobarValorNodo(n->center, contadorEtiquetaLocal);

            // Si existe else o elif
            if (n->dcha->tipoNodo == 13 || n->dcha->tipoNodo == 14)
            {
                fprintf(yyout, "bnez $t%d, etiqueta_%d    # Si $t%d no es 0, saltar a etiqueta de fin condición\n",
                        n->izq->resultado, etiquetaFinCondicion, n->izq->resultado);
            }
        }
        else if (strcmp(reg_cond->tipo, "float") == 0)
        {
            fprintf(yyout, "c.eq.s $f%d, $f29          # Comparar si $f%d es igual a 0.0\n",
                    n->izq->resultado, n->izq->resultado);
            fprintf(yyout, "bc1t etiqueta_%d          # Si es verdadero (igual a 0.0), saltar a etiqueta fin if\n", etiquetaFinIf);

            // n->center: cuerpo del if
            comprobarValorNodo(n->center, contadorEtiquetaLocal);

            // Si existe else o elif
            if (n->dcha->tipoNodo == 13 || n->dcha->tipoNodo == 14)
            {
                fprintf(yyout, "bc1f etiqueta_%d          # Si es falso (no igual a 0.0), saltar a etiqueta de fin condición\n", etiquetaFinCondicion);
            }
        }

        fprintf(yyout, "etiqueta_%d:\n", etiquetaFinIf);

        // n->dcha: else, elif o END
        // printf("tipo nodo: %d:\n", n->dcha->tipoNodo);
        comprobarValorNodo(n->dcha, contadorEtiquetaLocal);

        // Si existe else o elif
        if (n->dcha->tipoNodo == 13 || n->dcha->tipoNodo == 14)
        {
            // Si el if es true, se saltan las condiciones else y elif
            fprintf(yyout, "etiqueta_%d:\n", etiquetaFinCondicion);
        }
    }
    break;

    case 13: // Instrucción elif (else if)
    {
        printf("13\n");
        int etiquetaFinElif = contadorEtiquetaLocal++;
        int etiquetaFinCondicion = contadorEtiquetaLocal++;

        fprintf(yyout, "l.s $f29, zero\n");
        // n->izq es la condición del if
        struct ast *reg_cond = comprobarValorNodo(n->izq, contadorEtiquetaLocal);

        // Evaluar la condición del if
        if (strcmp(reg_cond->tipo, "int") == 0 || strcmp(reg_cond->tipo, "boolean") == 0)
        {
            // printf("resultado %d\n", reg_cond->resultado);
            fprintf(yyout, "beqz $t%d, etiqueta_%d    # Si $t%d es 0, saltar a etiqueta else\n",
                    n->izq->resultado, etiquetaFinElif, n->izq->resultado);

            // n->center es el cuerpo del if
            comprobarValorNodo(n->center, contadorEtiquetaLocal);

            // Si existe else o elif
            if (n->dcha->tipoNodo == 13 || n->dcha->tipoNodo == 14)
            {
                fprintf(yyout, "bnez $t%d, etiqueta_%d    # Si $t%d no es 0, saltar a etiqueta de fin condición\n",
                        n->izq->resultado, etiquetaFinCondicion, n->izq->resultado);
            }
        }
        else if (strcmp(reg_cond->tipo, "float") == 0)
        {
            // Evaluar condición para flotantes
            fprintf(yyout, "c.eq.s $f%d, $f29          # Comparar si $f%d es igual a 0.0\n",
                    n->izq->resultado, n->izq->resultado);
            fprintf(yyout, "bc1t etiqueta_%d          # Si es verdadero (igual a 0.0), saltar a etiqueta else\n", etiquetaFinElif);

            // n->center es el cuerpo del if
            comprobarValorNodo(n->center, contadorEtiquetaLocal);

            // Si existe else o elif
            if (n->dcha->tipoNodo == 13 || n->dcha->tipoNodo == 14)
            {
                fprintf(yyout, "bc1f etiqueta_%d          # Si es falso (no igual a 0.0), saltar a etiqueta de fin condición\n", etiquetaFinCondicion);
            }
        }

        // printf("tipoNodo %d\n", n->center->tipoNodo);

        fprintf(yyout, "etiqueta_%d:\n", etiquetaFinElif);

        // n->dcha: else, elif o END
        comprobarValorNodo(n->dcha, contadorEtiquetaLocal);

        // Si existe else o elif
        if (n->dcha->tipoNodo == 13 || n->dcha->tipoNodo == 14)
        {
            // Si el elif es true, se saltan las condiciones else y elif
            fprintf(yyout, "etiqueta_%d:\n", etiquetaFinCondicion);
        }
    }
    break;

    case 14: // Instrucción else
    {
        printf("14\n");

        // n->izq: Cuerpo de else
        comprobarValorNodo(n->izq, contadorEtiquetaLocal);

        // n->dcha: END
        comprobarValorNodo(n->dcha, contadorEtiquetaLocal);
    }
    break;

    case 15: // Nueva asignación
    {
        printf("15\n");
        reg = comprobarValorNodo(n->izq, contadorEtiquetaLocal);
    }
    break;

    case 16: // Nuevo imprimir
    {
        printf("16\n");
        // printf("tipo variable: %s\n", n->izq->tipo);
        comprobarValorNodo(n->izq, contadorEtiquetaLocal);
        funcionImprimir(n->izq);
    }
    break;

    case 17: // Comprobación igual que ( == )
    {
        printf("17\n");

        int etiquetaIgual = contadorEtiquetaLocal++;
        int etiquetaFinIgual = contadorEtiquetaLocal++;

        reg = comprobarValorNodo(n->izq, contadorEtiquetaLocal);
        comprobarValorNodo(n->dcha, contadorEtiquetaLocal);

        if (strcmp(n->izq->tipo, "int") == 0)
        {
            fprintf(yyout, "seq $t%d, $t%d, $t%d      # Comparar si $t%d == $t%d, almacenar resultado en $t%d\n",
                    n->resultado, n->izq->resultado, n->dcha->resultado,
                    n->izq->resultado, n->dcha->resultado, n->resultado);

            // printf("valor izq: %d, valor dcha: %d\n",
            //         variables[n->izq->resultado].valorEntero, variables[n->dcha->resultado].valorEntero);
        }
        else if (strcmp(n->izq->tipo, "float") == 0)
        {
            fprintf(yyout, "c.eq.s $f%d, $f%d\n", n->izq->resultado, n->dcha->resultado);
            fprintf(yyout, "bc1t son_iguales_%d\n", etiquetaIgual);
            fprintf(yyout, "nop\n");
            fprintf(yyout, "li $t0, 0\n");
            fprintf(yyout, "mtc1 $t0, $f%d\n", n->resultado);
            fprintf(yyout, "j fin_igual_%d\n", etiquetaFinIgual);
            fprintf(yyout, "son_iguales_%d:\n", etiquetaIgual);
            fprintf(yyout, "li $t0, 1065353216\n");
            fprintf(yyout, "mtc1 $t0, $f%d\n", n->resultado);
            fprintf(yyout, "fin_igual_%d:\n", etiquetaFinIgual);
        }

        borrarReg(n->izq, n->dcha);
        break;
    }

    case 18: // Comprobación distinto que ( != )
    {
        printf("18\n");

        int etiquetaDistinto = contadorEtiquetaLocal++;
        int etiquetaFinDistinto = contadorEtiquetaLocal++;

        reg = comprobarValorNodo(n->izq, contadorEtiquetaLocal);
        comprobarValorNodo(n->dcha, contadorEtiquetaLocal);

        if (strcmp(n->izq->tipo, "int") == 0)
        {
            fprintf(yyout, "sne $t%d, $t%d, $t%d      # Comparar si $t%d != $t%d, almacenar resultado en $t%d\n",
                    n->resultado, n->izq->resultado, n->dcha->resultado,
                    n->izq->resultado, n->dcha->resultado, n->resultado);
        }
        else if (strcmp(n->izq->tipo, "float") == 0)
        {
            fprintf(yyout, "c.eq.s $f%d, $f%d\n", n->izq->resultado, n->dcha->resultado);
            fprintf(yyout, "bc1t son_distintos_%d\n", etiquetaDistinto);
            fprintf(yyout, "nop\n");
            fprintf(yyout, "li $t0, 1065353216\n");
            fprintf(yyout, "mtc1 $t0, $f%d\n", n->resultado);
            fprintf(yyout, "j fin_son_distintos_%d\n", etiquetaFinDistinto);
            fprintf(yyout, "son_distintos_%d:\n", etiquetaDistinto);
            fprintf(yyout, "li $t0, 0\n");
            fprintf(yyout, "mtc1 $t0, $f%d\n", n->resultado);
            fprintf(yyout, "fin_son_distintos_%d:\n", etiquetaFinDistinto);
        }

        borrarReg(n->izq, n->dcha);
        break;
    }

    case 19: // Comprobación menor que ( < )
    {
        printf("19\n");

        int etiquetaMenor = contadorEtiquetaLocal++;
        int etiquetaFinMenor = contadorEtiquetaLocal++;

        reg = comprobarValorNodo(n->izq, contadorEtiquetaLocal);
        comprobarValorNodo(n->dcha, contadorEtiquetaLocal);

        if (strcmp(n->izq->tipo, "int") == 0)
        {
            fprintf(yyout, "slt $t%d, $t%d, $t%d      # Comparar si $t%d < $t%d, almacenar resultado en $t%d\n", n->resultado, n->izq->resultado, n->dcha->resultado, n->izq->resultado, n->dcha->resultado, n->resultado);
        }
        else if (strcmp(n->izq->tipo, "float") == 0)
        {
            fprintf(yyout, "c.lt.s $f%d, $f%d\n", n->izq->resultado, n->dcha->resultado);
            fprintf(yyout, "bc1t es_menor_%d\n", etiquetaMenor);
            fprintf(yyout, "nop\n");
            fprintf(yyout, "li $t0, 0\n");
            fprintf(yyout, "mtc1 $t0, $f%d\n", n->resultado);
            fprintf(yyout, "j fin_menor_%d\n", etiquetaFinMenor);
            fprintf(yyout, "es_menor_%d:\n", etiquetaMenor);
            fprintf(yyout, "li $t0, 1065353216\n");
            fprintf(yyout, "mtc1 $t0, $f%d\n", n->resultado);
            fprintf(yyout, "fin_menor_%d:\n", etiquetaFinMenor);
        }

        // fprintf(yyout, "c.lt.s $f%d, $f%d, $f%d\n", n->resultado, n->izq->resultado, n->dcha->resultado);
        borrarReg(n->izq, n->dcha);
        break;
    }

    case 20: // Comprobación menor igual que  ( <= )
    {
        printf("20\n");

        int etiquetaMenorIgual = contadorEtiquetaLocal++;
        int etiquetaFinMenorIgual = contadorEtiquetaLocal++;

        reg = comprobarValorNodo(n->izq, contadorEtiquetaLocal);
        comprobarValorNodo(n->dcha, contadorEtiquetaLocal);

        if (strcmp(n->izq->tipo, "int") == 0)
        {
            fprintf(yyout, "slt $t%d, $t%d, $t%d      # Comparar si $t%d < $t%d, almacenar resultado en $t%d\n",
                    n->resultado, n->dcha->resultado, n->izq->resultado,
                    n->dcha->resultado, n->izq->resultado, n->resultado);
            fprintf(yyout, "xori $t%d, $t%d, 1        # Invertir el resultado para obtener <= \n",
                    n->resultado, n->resultado);
        }
        else if (strcmp(n->izq->tipo, "float") == 0)
        {
            fprintf(yyout, "c.le.s $f%d, $f%d\n", n->izq->resultado, n->dcha->resultado);
            fprintf(yyout, "bc1t es_menor_o_igual_%d\n", etiquetaMenorIgual);
            fprintf(yyout, "nop\n");
            fprintf(yyout, "li $t0, 0\n");
            fprintf(yyout, "mtc1 $t0, $f%d\n", n->resultado);
            fprintf(yyout, "j fin_menor_o_igual_%d\n", etiquetaFinMenorIgual);
            fprintf(yyout, "es_menor_o_igual_%d:\n", etiquetaMenorIgual);
            fprintf(yyout, "li $t0, 1065353216\n");
            fprintf(yyout, "mtc1 $t0, $f%d\n", n->resultado);
            fprintf(yyout, "fin_menor_o_igual_%d:\n", etiquetaFinMenorIgual);
        }

        borrarReg(n->izq, n->dcha);
        break;
    }

    case 21: // Comprobación mayor que  ( > )
    {
        printf("21\n");

        int etiquetaMayor = contadorEtiquetaLocal++;
        int etiquetaFinMayor = contadorEtiquetaLocal++;

        reg = comprobarValorNodo(n->izq, contadorEtiquetaLocal);
        comprobarValorNodo(n->dcha, contadorEtiquetaLocal);

        if (strcmp(n->izq->tipo, "int") == 0)
        {
            fprintf(yyout, "slt $t%d, $t%d, $t%d      # Comparar si $t%d < $t%d, almacenar resultado en $t%d\n",
                    n->resultado, n->dcha->resultado, n->izq->resultado,
                    n->dcha->resultado, n->izq->resultado, n->resultado);
        }
        else if (strcmp(n->izq->tipo, "float") == 0)
        {
            fprintf(yyout, "c.lt.s $f%d, $f%d\n", n->dcha->resultado, n->izq->resultado);
            fprintf(yyout, "bc1t es_mayor_%d\n", etiquetaMayor);
            fprintf(yyout, "nop\n");
            fprintf(yyout, "li $t0, 0\n");
            fprintf(yyout, "mtc1 $t0, $f%d\n", n->resultado);
            fprintf(yyout, "j fin_mayor_%d\n", etiquetaFinMayor);
            fprintf(yyout, "es_mayor_%d:\n", etiquetaMayor);
            fprintf(yyout, "li $t0, 1065353216\n");
            fprintf(yyout, "mtc1 $t0, $f%d\n", n->resultado);
            fprintf(yyout, "fin_mayor_%d:\n", etiquetaFinMayor);
        }

        borrarReg(n->izq, n->dcha);
        break;
    }

    case 22: // Comprobación mayor igual que ( >= )
    {
        printf("22\n");

        int etiquetaMayorIgual = contadorEtiquetaLocal++;
        int etiquetaFinMayorIgual = contadorEtiquetaLocal++;

        reg = comprobarValorNodo(n->izq, contadorEtiquetaLocal);
        comprobarValorNodo(n->dcha, contadorEtiquetaLocal);

        if (strcmp(n->izq->tipo, "int") == 0)
        {
            fprintf(yyout, "slt $t%d, $t%d, $t%d      # Comparar si $t%d < $t%d, almacenar resultado en $t%d\n",
                    n->resultado, n->izq->resultado, n->dcha->resultado,
                    n->izq->resultado, n->dcha->resultado, n->resultado);
            fprintf(yyout, "xori $t%d, $t%d, 1        # Invertir el resultado para obtener >= \n",
                    n->resultado, n->resultado);
        }
        else if (strcmp(n->izq->tipo, "float") == 0)
        {
            fprintf(yyout, "c.le.s $f%d, $f%d\n", n->dcha->resultado, n->izq->resultado);
            fprintf(yyout, "bc1t es_mayor_o_igual_%d\n", etiquetaMayorIgual);
            fprintf(yyout, "nop\n");
            fprintf(yyout, "li $t0, 0\n");
            fprintf(yyout, "mtc1 $t0, $f%d\n", n->resultado);
            fprintf(yyout, "j fin_mayor_o_igual_%d\n", etiquetaFinMayorIgual);
            fprintf(yyout, "es_mayor_o_igual_%d:\n", etiquetaMayorIgual);
            fprintf(yyout, "li $t0, 1065353216\n");
            fprintf(yyout, "mtc1 $t0, $f%d\n", n->resultado);
            fprintf(yyout, "fin_mayor_o_igual_%d:\n", etiquetaFinMayorIgual);
        }

        borrarReg(n->izq, n->dcha);
        break;
    }

    case 23: // END
    {
        printf("23\n");
        int etiquetaElse = contadorEtiquetaLocal++;

        fprintf(yyout, "etiqueta_%d:\n", etiquetaElse);
    }
    break;

    case 24: // Comentario (no hace nada)
        break;

    case 25: // For
    {
        printf("25\n");
        int etiqueta = contadorEtiquetaLocal;
        contadorEtiquetaLocal++;

        fprintf(yyout, "l.s $f29, zero\n"); // cargar esto al final de zero nuevamente
        comprobarValorNodo(n->izq, contadorEtiquetaLocal);
        fprintf(yyout, "etiqueta%d:\n", etiqueta);
        // Comparo si el valor de f29 es menor que el valor del nodo izq
        fprintf(yyout, "c.lt.s $f%d, $f%d\n", 29, n->izq->resultado);
        fprintf(yyout, "  bc1f fin_bucle%d\n", etiqueta); // Si es 0, salimos del bucle
        fprintf(yyout, "    nop\n");

        comprobarValorNodo(n->dcha, 7); // Comprobamos el valor del nodo derecho
        // Incremento el valor de f29 en 1

        fprintf(yyout, "l.s $f30, uno\n");
        fprintf(yyout, "add.s $f29, $f29, $f30\n");
        fprintf(yyout, "j etiqueta%d\n", etiqueta); // Volvemos a la etiqueta
        fprintf(yyout, "fin_bucle%d:\n", etiqueta); // Etiqueta de fin de bucle
        fprintf(yyout, "l.s $f29, zero\n");         // cargar esto al final de zero nuevamente

        borrarReg(n->izq, n->dcha); // borrado de registros (se ponen a true)
    }
    break;

    default: // Nodo no reconocido, manejo de errores
        gestionarError("Error: Tipo de nodo no reconocido.");
        break;
    }

    return reg; // Devolvemos el registro
}

// METODO "crearNombreVariable", incremente el valor de la variable "nombreVariable"
int crearNombreVariable()
{
    return nombreVariable++; // retorna la variable y luego la incrementa
}

// METODO "comprobarAST", imprime el codigo .asm y generas sus respectivos pasos
comprobarAST(struct ast *n)
{
    imprimirVariables();
    // imprimirVariables(); // Metodo que realiza la impresion de la parte de variables para Mips
    fprintf(yyout, "\n#--------------------- Ejecuciones ---------------------");
    fprintf(yyout, "\n.text\n");
    fprintf(yyout, "lwc1 $f31, zero\n");
    printf("\nNodos del arbol utilizados:\n");
    comprobarValorNodo(n, contadorEtiqueta); // Comprueba el valor del nodo
}

// METODO "imprimir", imprime el codigo .asm que hace referencia a la funcion imprimir de python
funcionImprimir(struct ast *n)
{
    if (strcmp(n->tipo, "int") == 0)
    {
        // Imprimir entero
        fprintf(yyout, "li $v0, 1\n");                    // Código de sistema para imprimir entero
        fprintf(yyout, "move $a0, $t%d\n", n->resultado); // Mover el resultado al registro $a0
    }
    else if (strcmp(n->tipo, "float") == 0)
    {
        // Imprimir punto flotante
        fprintf(yyout, "li $v0, 2\n"); // Código de sistema para imprimir float
        // fprintf(yyout, "mov.s $f12, $f%d\n", n->resultado); // Mover el resultado al registro $f12
        fprintf(yyout, "add.s $f12, $f31, $f%d\n", n->resultado); // Mover del registro n al registro 30 (es el que empleamos para imprimir)
        fprintf(yyout, "mov.s $f30, $f12    # Movemos el registro 12 al 30 iniciado a false\n");
    }
    else if (strcmp(n->tipo, "string") == 0)
    {
        // Imprimir carácter
        fprintf(yyout, "li $v0, 4\n"); // Código de sistema para imprimir cadena

        if (cadenaConcatenada)
        {
            fprintf(yyout, "la $a0, resultado\n");
        }
        else
        {
            fprintf(yyout, "la $a0, var_%d\n", n->resultado); // Mover del registro n al registro 30 (es el que empleamos para imprimir)
        }

        fprintf(yyout, "addi $v0, $0, 4  #Movemos el registro 12 al 30 iniciado a false\n");
        // fprintf(yyout, "li $v0, 11\n");                   // Código de sistema para imprimir carácter
        // fprintf(yyout, "move $a0, $t%d\n", n->resultado); // Mover el resultado al registro $a0
    }
    else if (strcmp(n->tipo, "bool") == 0)
    {
        // Imprimir booleano (0 o 1)
        fprintf(yyout, "li $v0, 1\n");                    // Código de sistema para imprimir entero
        fprintf(yyout, "move $a0, $t%d\n", n->resultado); // Mover el resultado al registro $a0
    }

    fprintf(yyout, "syscall     # Llamada al sistema\n");
    saltoLinea(); // Introducimos un salto de linea
}

// Función para imprimir variables en el archivo .asm
void imprimirVariables()
{
    fprintf(yyout, "\n#-------------- Declaracion de variables --------------\n");
    fprintf(yyout, ".data\n");
    fprintf(yyout, "saltoLinea: .asciiz \"\\n\"\n"); // Variable salto de linea
    fprintf(yyout, "zero: .float 0.0\n");            // Se inserta una variable auxiliar var_0 con valor 0.000
    fprintf(yyout, "uno: .float 1.0\n");             // Se inserta una variable auxiliar var_0 con valor 1.000
    fprintf(yyout, "resultado: .space 100\n");
    // Bucle que recorre el array de variables y las imprime en el archivo .asm
    for (int i = 0; i < 64; i++)
    {
        // printf("Variable %d: tipo=%s, nombre=%s\n", i, variables[i].tipo, variables[i].nombre);
        if (variables[i].disponible == true)
        {
            // printf("\ni=%d --> nombre de variable=%c\n", i, variables[i].nombre);

            if (strcmp(variables[i].tipo, "int") == 0)
            {
                fprintf(yyout, "var_%d: .word %d\n", variables[i].nombre, variables[i].valorEntero);
            }
            else if (strcmp(variables[i].tipo, "float") == 0)
            {
                fprintf(yyout, "var_%d: .float %.3f\n", variables[i].nombre, variables[i].valorFloat);
            }
            else if (strcmp(variables[i].tipo, "string") == 0)
            {
                fprintf(yyout, "var_%d: .asciiz %s\n", variables[i].nombre, variables[i].valorCadena);
            }
            else if (strcmp(variables[i].tipo, "boolean") == 0)
            {
                fprintf(yyout, "var_%d: .word %d\n", variables[i].nombre, variables[i].valorBoolean);
            }
        }
    }
}

// METODO "saltoLinea", incorpora un salto de linea en la salida de nuestro codigo
saltoLinea()
{
    fprintf(yyout, "li $v0, 4\n");          // especifica al registro $v0 que va a imprimir una cadena de caracteres
    fprintf(yyout, "la $a0, saltoLinea\n"); // carga en $a0 el valor del salto de linea
    fprintf(yyout, "syscall #Llamada al sistema\n");
}

int encontrarReg()
{
    // for (int posicion = 0; posicion < NUM_REG_V; posicion++)
    // {
    //     if (registros[posicion] == true)
    //     {                                // Encuentra el primer registro libre
    //         registros[posicion] = false; // Marca el registro como ocupado
    //         return posicion;             // Retorna la posición del registro libre
    //     }
    // }
    // for (int posicion = 0; posicion < NUM_REG_A; posicion++)
    // {
    //     if (registros[posicion] == true)
    //     {                                // Encuentra el primer registro libre
    //         registros[posicion] = false; // Marca el registro como ocupado
    //         return posicion;             // Retorna la posición del registro libre
    //     }
    // }
    // for (int posicion = 0; posicion < NUM_REG_T; posicion++)
    // {
    //     if (registros[posicion] == true)
    //     {                                // Encuentra el primer registro libre
    //         registros[posicion] = false; // Marca el registro como ocupado
    //         return posicion;             // Retorna la posición del registro libre
    //     }
    // }
    // for (int posicion = 0; posicion < NUM_REG_S; posicion++)
    // {
    //     if (registros[posicion] == true)
    //     {                                // Encuentra el primer registro libre
    //         registros[posicion] = false; // Marca el registro como ocupado
    //         return posicion;             // Retorna la posición del registro libre
    //     }
    // }
    for (int posicion = 0; posicion < NUM_REG_F; posicion++)
    {
        if (registros[posicion] == true)
        {                                // Encuentra el primer registro libre
            registros[posicion] = false; // Marca el registro como ocupado
            return posicion;             // Retorna la posición del registro libre
        }
    }
    fprintf(stderr, "Error: No hay registros libres disponibles.\n");
    exit(EXIT_FAILURE); // Termina el programa si no hay registros libres
}

// METODO "borrarReg", pone a true de nuevo el registro para que pueda volver a usarse
void borrarReg(struct ast *izq, struct ast *dcha)
{
    if (izq != NULL)
    {
        registros[izq->resultado] = true;
    }
    if (dcha != NULL)
    {
        registros[dcha->resultado] = true;
    }
}

// METODO "crearNodoVacio", crea un nuevo nodo sin contenido
struct ast *crearNodoVacio()
{
    struct ast *n = malloc(sizeof(struct ast)); // Asigna memoria dinámicamente para el nuevo nodo
    n->izq = NULL;
    n->dcha = NULL;
    n->tipoNodo = 0;
    n->tipo = NULL;
    return n;
}

struct ast *crearNodoEnd(int tipoNodo)
{
    struct ast *n = malloc(sizeof(struct ast)); // Crea un nuevo nodo
    n->tipoNodo = tipoNodo;                     // Asignamos al nodo genérico sus hijos y tipo
    // n->resultado = encontrarReg();              // Hacemos llamada al método para buscar un nuevo registro

    // printf("tipoNodo: %d\n", tipoNodo);
    return n;
}

struct ast *crearNodoTerminalDouble(double valor)
{
    struct ast *n = malloc(sizeof(struct ast)); // Asigna memoria dinámicamente para el nuevo nodo
    n->izq = NULL;
    n->dcha = NULL;
    n->tipoNodo = 1;
    n->tipo = "float";
    n->tipoReg = "f";
    n->valorDecimal = valor;

    n->resultado = encontrarReg();        // Hacemos llamada al método para buscar un nuevo registro
    n->nombreVar = crearNombreVariable(); // Genera un nombre único para la variable
    printf("# [AST] - Registro $f%d ocupado para var_%d = %.3f\n", n->resultado, n->nombreVar, n->valorDecimal);

    // Actualizar el registro de variables
    variables[n->resultado].tipo = n->tipo;
    variables[n->resultado].valorFloat = n->valorDecimal;
    variables[n->resultado].nombre = n->nombreVar;
    variables[n->resultado].disponible = true;

    return n;
}

struct ast *crearNodoTerminalInt(int valor)
{
    struct ast *n = malloc(sizeof(struct ast)); // Asigna memoria dinámicamente para el nuevo nodo
    n->izq = NULL;
    n->dcha = NULL;
    n->tipoNodo = 1;
    n->tipo = "int";
    n->tipoReg = "t";
    n->valorEntero = valor;

    n->resultado = encontrarReg();        // Hacemos llamada al método para buscar un nuevo registro
    n->nombreVar = crearNombreVariable(); // Genera un nombre único para la variable
    printf("# [AST] - Registro $f%d ocupado para var_%d = %d\n", n->resultado, n->nombreVar, n->valorEntero);

    // Actualizar el registro de variables
    variables[n->resultado].tipo = n->tipo;
    variables[n->resultado].valorEntero = n->valorEntero;
    variables[n->resultado].nombre = n->nombreVar;
    variables[n->resultado].disponible = true;

    return n;
}

struct ast *crearNodoTerminalString(char *valor)
{
    struct ast *n = malloc(sizeof(struct ast)); // Asigna memoria dinámicamente para el nuevo nodo
    n->izq = NULL;
    n->dcha = NULL;
    n->tipoNodo = 1;
    n->tipo = "string";
    n->tipoReg = "t";
    n->valorCadena = valor;
    n->resultado = encontrarReg();        // Hacemos llamada al método para buscar un nuevo registro
    n->nombreVar = crearNombreVariable(); // Genera un nombre único para la variable
    printf("# [AST] - Registro $f%d ocupado para var_%d = %s\n", n->resultado, n->nombreVar, n->valorCadena);

    // Actualizar el registro de variables
    variables[n->resultado].tipo = n->tipo;
    variables[n->resultado].valorCadena = n->valorCadena;
    variables[n->resultado].nombre = n->nombreVar;
    variables[n->resultado].disponible = true;

    return n;
}

struct ast *crearNodoTerminalBoolean(int valor)
{
    struct ast *n = malloc(sizeof(struct ast)); // Asigna memoria dinámicamente para el nuevo nodo
    n->izq = NULL;
    n->dcha = NULL;
    n->tipoNodo = 1;
    n->tipo = "boolean";
    n->tipoReg = "t";
    n->valorBoolean = valor;

    n->resultado = encontrarReg();        // Hacemos llamada al método para buscar un nuevo registro
    n->nombreVar = crearNombreVariable(); // Genera un nombre único para la variable
    printf("# [AST] - Registro $f%d ocupado para var_%d = %d\n", n->resultado, n->nombreVar, n->valorBoolean);

    // Actualizar el registro de variables
    variables[n->resultado].tipo = n->tipo;
    variables[n->resultado].valorBoolean = n->valorBoolean;
    variables[n->resultado].nombre = n->nombreVar;
    variables[n->resultado].disponible = true;

    return n;
}

// METODO "crearNodoNoTerminal", crea un nuevo nodo, asignamos sus hijos y tipo, y buscamos nuevo registro
struct ast *crearNodoNoTerminal(struct ast *izq, struct ast *dcha, int tipoNodo)
{
    printf("tipo nodo: %d\n", tipoNodo);
    struct ast *n = malloc(sizeof(struct ast)); // Crea un nuevo nodo
    n->izq = izq;
    n->dcha = dcha;
    n->tipoNodo = tipoNodo;        // Asignamos al nodo genérico sus hijos y tipo
    n->resultado = encontrarReg(); // Hacemos llamada al método para buscar un nuevo registro
    return n;
}

struct ast *crearNodoNoTerminalIf(struct ast *izq, struct ast *center, struct ast *dcha, int tipoNodo)
{
    struct ast *n = malloc(sizeof(struct ast)); // Crea un nuevo nodo
    n->izq = izq;
    n->center = center;
    n->dcha = dcha;
    n->tipoNodo = tipoNodo; // Asignamos al nodo genérico sus hijos y tipo
    // n->resultado = encontrarReg(); // Hacemos llamada al método para buscar un nuevo registro

    // printf("tipoNodo: %d\n", tipoNodo);
    return n;
}

// METODO "crearVariableTerminalDouble", crear el nodo hoja para una variable ya creada con valor double
struct ast *crearVariableTerminalDouble(double valor, int registro)
{
    struct ast *n = malloc(sizeof(struct ast)); // Asigna memoria dinámicamente para el nuevo nodo
    n->izq = NULL;
    n->dcha = NULL;
    n->tipoNodo = 6;
    n->tipo = "float";
    n->valorDecimal = valor;

    n->resultado = registro;
    return n;
}

// METODO "crearVariableTerminalInt", crear el nodo hoja para una variable ya creada con valor entero
struct ast *crearVariableTerminalInt(int valor, int registro)
{
    struct ast *n = malloc(sizeof(struct ast)); // Asigna memoria dinámicamente para el nuevo nodo
    n->izq = NULL;
    n->dcha = NULL;
    n->tipoNodo = 6;
    n->tipo = "int";
    n->valorEntero = valor;

    n->resultado = registro;

    return n;
}

// METODO "crearVariableTerminalString", crear el nodo hoja para una variable ya creada con valor texto
struct ast *crearVariableTerminalString(const char *valor, int registro)
{
    struct ast *n = malloc(sizeof(struct ast)); // Asigna memoria dinámicamente para el nuevo nodo
    n->izq = NULL;
    n->dcha = NULL;
    n->tipoNodo = 6;
    n->tipo = "string";
    n->valorCadena = valor;

    n->valorCadena = strdup(valor); // Asigna memoria y copia el texto
    n->resultado = registro;
    return n;
}

// METODO "crearVariableTerminal", crear el nodo hoja para una variable ya creada
struct ast *crearVariableTerminalBoolean(int valor, int registro)
{
    struct ast *n = malloc(sizeof(struct ast)); // Asigna memoria dinámicamente para el nuevo nodo
    n->izq = NULL;
    n->dcha = NULL;
    n->tipoNodo = 6;
    n->tipo = "boolean";
    n->valorBoolean = valor;

    n->resultado = registro;
    return n;
}

// Función para gestionar errores
gestionarError(const char *mensaje)
{
    fprintf(stderr, "%s\n", mensaje);
    // Aquí puedes agregar manejo adicional de errores, como terminar el programa o registrar el error en un archivo
}

void comprobarTipos(struct ast *nodo)
{
    if (nodo->izq != NULL && nodo->dcha != NULL)
    {
        if (strcmp(nodo->izq->tipo, nodo->dcha->tipo) != 0)
        {
            gestionarError("Tipos incompatibles en la operación.");
        }
    }
}
