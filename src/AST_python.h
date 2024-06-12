// ----------------------------- GLOSARIO DE IMPORTS -------------------------------------------
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// ----------------------------- DECLARACION DE VARIABLES Y ESTRUCTURAS --------------------------------------------
extern FILE *yyout;
int contadorEtiqueta = 0; // Variable para el control de las etiquetas
int numMaxRegistros = 32; // Variable que indica el numero maximo de registros disponibles
int nombreVariable = 0;   // Almacena el entero que se asociará al nombre de la variable

// Por defecto, tenemos 32 registros de tipo f para controlar los registros libres (true) o ocupados (false)
bool registros[32] = {
    true, true, true, true, true, true, true, true,
    true, true, true, true, true, true, true, true,
    true, true, true, true, true, true, true, true,
    true, true, true, true, true, true, true, true};
// Los registros 30 y 31 están reservados por defecto para imprimir por pantalla

// Estructura variable, se hará uso de la misma para almacenar e imprimir las variables del codigo python
struct variable
{
    char *nombre;      // Nombre de la variable, utilizando un puntero a char para nombres dinámicos
    float dato;        // Valor de la variable (en caso de ser flotante)
    int valorEntero;   // Valor de la variable (en caso de ser entero)
    char *valorCadena; // Valor de la variable (en caso de ser cadena)
    // int nombre;       //limite de caracteres de la variable
    bool disponible; // Indica si la variable está disponible
    char *tipo;      // Tipo de la variable: "int", "float", "string", etc.
};

struct variable variables[64]; // Declaramos el array de variables usando la estructura definida

struct funcion
{
    char *nombre;               // Nombre de la función
    char *tipoRetorno;          // Tipo de retorno de la función
    struct variable params[10]; // Parámetros de la función (puedes ajustar el tamaño según tus necesidades)
    int numParams;              // Número de parámetros
};

struct funcion funciones[32]; // Arreglo para almacenar las funciones

struct ambito
{
    struct variable variables[64];
    int numVariables;
};

struct ambito pilaAmbitos[32];
int indiceAmbito = 0;

// Estructura AST, se define la estructura de los nodos del arbol
struct ast
{
    struct ast *izq;  // Nodo izquierdo del arbol
    struct ast *dcha; // Nodo derecho del arbol
    int tipoNodo;     // Almacena el tipo de nodo
    union
    {
        double valorNumerico; // Valor si es numérico
        char *valorCadena;    // Valor si es una cadena
    };
    // double valor;        // Almacena el valor del nodo
    char *tipo;    // Tipo de dato: "int", "float", "string"
    int resultado; // Registro donde está el resultado
    int nombreVar; // Indica el nombre de la variable
};

//-----------------------------------------------  METODOS -------------------------------------------------------

// Función para comprobar el valor del nodo y generar el código correspondiente
double comprobarValorNodo(struct ast *n, int contadorEtiquetaLocal)
{
    double dato;

    switch (n->tipoNodo)
    {
    case 1: // Nueva hoja en el árbol
        dato = n->valorNumerico;
        fprintf(yyout, "lwc1 $f%d, var_%d\n", n->resultado, n->nombreVar);
        break;

    case 2: // Nueva suma
        dato = comprobarValorNodo(n->izq, contadorEtiquetaLocal) + comprobarValorNodo(n->dcha, contadorEtiquetaLocal);
        fprintf(yyout, "add.s $f%d, $f%d, $f%d\n", n->resultado, n->izq->resultado, n->dcha->resultado);
        borrarReg(n->izq, n->dcha);
        break;

    case 3: // Nueva resta
        dato = comprobarValorNodo(n->izq, contadorEtiquetaLocal) - comprobarValorNodo(n->dcha, contadorEtiquetaLocal);
        fprintf(yyout, "sub.s $f%d, $f%d, $f%d\n", n->resultado, n->izq->resultado, n->dcha->resultado);
        borrarReg(n->izq, n->dcha);
        break;

    case 4: // Nueva multiplicación
        dato = comprobarValorNodo(n->izq, contadorEtiquetaLocal) * comprobarValorNodo(n->dcha, contadorEtiquetaLocal);
        fprintf(yyout, "mul.s $f%d, $f%d, $f%d\n", n->resultado, n->izq->resultado, n->dcha->resultado);
        borrarReg(n->izq, n->dcha);
        break;

    case 5: // Nueva división
        if (comprobarValorNodo(n->dcha, contadorEtiquetaLocal) == 0)
        {
            gestionarError("Error: División por cero.");
            return 0;
        }
        dato = comprobarValorNodo(n->izq, contadorEtiquetaLocal) / comprobarValorNodo(n->dcha, contadorEtiquetaLocal);
        fprintf(yyout, "div.s $f%d, $f%d, $f%d\n", n->resultado, n->izq->resultado, n->dcha->resultado);
        borrarReg(n->izq, n->dcha);
        break;

    case 6: // Nueva variable
        dato = n->valorNumerico;
        break;

    case 7: // Lista de sentencias
        dato = comprobarValorNodo(n->izq, contadorEtiquetaLocal);
        comprobarValorNodo(n->dcha, contadorEtiquetaLocal);
        break;

    case 8: // Operación AND
        dato = comprobarValorNodo(n->izq, contadorEtiquetaLocal) && comprobarValorNodo(n->dcha, contadorEtiquetaLocal);
        fprintf(yyout, "and $t%d, $t%d, $t%d\n", n->resultado, n->izq->resultado, n->dcha->resultado);
        borrarReg(n->izq, n->dcha);
        break;

    case 9: // Operación OR
        dato = comprobarValorNodo(n->izq, contadorEtiquetaLocal) || comprobarValorNodo(n->dcha, contadorEtiquetaLocal);
        fprintf(yyout, "or $t%d, $t%d, $t%d\n", n->resultado, n->izq->resultado, n->dcha->resultado);
        borrarReg(n->izq, n->dcha);
        break;

    case 10: // Operación NOT
        dato = !comprobarValorNodo(n->izq, contadorEtiquetaLocal);
        fprintf(yyout, "not $t%d, $t%d\n", n->resultado, n->izq->resultado);
        borrarReg(n->izq, n->dcha);
        break;

    case 11: // Bucle while
    {
        int etiquetaInicio = contadorEtiquetaLocal++;
        int etiquetaFin = contadorEtiquetaLocal++;
        fprintf(yyout, "etiqueta_%d:\n", etiquetaInicio);
        if (!comprobarValorNodo(n->izq, contadorEtiquetaLocal))
        {
            fprintf(yyout, "j etiqueta_%d\n", etiquetaFin);
        }
        comprobarValorNodo(n->dcha, contadorEtiquetaLocal);
        fprintf(yyout, "j etiqueta_%d\n", etiquetaInicio);
        fprintf(yyout, "etiqueta_%d:\n", etiquetaFin);
    }
    break;

    case 12: // Condición if-else
    {
        int etiquetaElse = contadorEtiquetaLocal++;
        int etiquetaFin = contadorEtiquetaLocal++;
        if (!comprobarValorNodo(n->izq, contadorEtiquetaLocal))
        {
            fprintf(yyout, "j etiqueta_%d\n", etiquetaElse);
        }
        comprobarValorNodo(n->dcha->izq, contadorEtiquetaLocal);
        fprintf(yyout, "j etiqueta_%d\n", etiquetaFin);
        fprintf(yyout, "etiqueta_%d:\n", etiquetaElse);
        comprobarValorNodo(n->dcha->dcha, contadorEtiquetaLocal);
        fprintf(yyout, "etiqueta_%d:\n", etiquetaFin);
    }
    break;

    case 13: // Nueva asignación
    {
        dato = comprobarValorNodo(n->izq, contadorEtiquetaLocal);
    }
    break;

    case 14: // Nuevo imprimir
    {
        comprobarValorNodo(n->izq, contadorEtiquetaLocal);
        funcionImprimir(n->izq);
    }
    break;

    case 15: // Comentario (no hace nada)
        break;

    default: // Nodo no reconocido, manejo de errores
        gestionarError("Error: Tipo de nodo no reconocido.");
        break;
    }

    return dato; // Devolvemos el valor
}

// METODO "crearNombreVariable", incremente el valor de la variable "nombreVariable"
int crearNombreVariable()
{
    return nombreVariable++; // retorna la variable y luego la incrementa
}

// METODO "comprobarAST", imprime el codigo .asm y generas sus respectivos pasos
comprobarAST(struct ast *n)
{
    printf("Imprimiendo variables\n");
    imprimirVariables();
    printf("Generando código .text\n");
    // imprimirVariables(); // Metodo que realiza la impresion de la parte de variables para Mips
    fprintf(yyout, "\n#--------------------- Ejecuciones ---------------------");
    fprintf(yyout, "\n.text\n");
    fprintf(yyout, "lwc1 $f31, zero\n");
    comprobarValorNodo(n, contadorEtiqueta); // Comprueba el valor del nodo
}

// METODO "imprimir", imprime el codigo .asm que hace referencia a la funcion imprimir de latino
funcionImprimir(struct ast *n)
{
    fprintf(yyout, "li $v0, 2\n");                            // entero
    fprintf(yyout, "add.s $f12, $f31, $f%d\n", n->resultado); // Mover del registro n al registro 30 (es el que empleamos para imprimir)
    fprintf(yyout, "mov.s $f30, $f12  #Movemos el registro 12 al 30 iniciado a false\n");
    fprintf(yyout, "syscall #Llamada al sistema\n");
    saltoLinea(); // Introducimos un salto de linea
}

// Función para imprimir variables en el archivo .asm
void imprimirVariables()
{
    fprintf(yyout, "\n#-------------- Declaracion de variables --------------\n");
    fprintf(yyout, ".data\n");
    fprintf(yyout, "saltoLinea: .asciiz \"\\n\"\n"); // Variable salto de linea
    fprintf(yyout, "zero: .float 0.0\n");            // Se inserta una variable auxiliar var_0 con valor 0.000
    // Bucle que recorre el array de variables y las imprime en el archivo .asm
    for (int i = 0; i < 64; i++)
    {
        printf("Variable %d: tipo=%s, nombre=%s\n", i, variables[i].tipo, variables[i].nombre);

        if (strcmp(variables[i].tipo, "float") == 0)
        {
            printf("if float\n");
            fprintf(yyout, "var_%s: .float %.3f\n", variables[i].nombre, variables[i].dato);
        }
        else if (strcmp(variables[i].tipo, "int") == 0)
        {
            printf("if int\n");
            fprintf(yyout, "var_%s: .word %d\n", variables[i].nombre, variables[i].valorEntero);
        }
        else if (strcmp(variables[i].tipo, "string") == 0)
        {
            printf("if string\n");
            fprintf(yyout, "var_%s: .asciiz \"%s\"\n", variables[i].nombre, variables[i].valorCadena);
        }
        else
        {
            printf("Tipo desconocido: %s\n", variables[i].tipo);
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
    for (int posicion = 0; posicion < numMaxRegistros; posicion++)
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

struct ast *crearNodoTerminal(double valor)
{
    struct ast *n = malloc(sizeof(struct ast)); // Asigna memoria dinámicamente para el nuevo nodo
    n->izq = NULL;
    n->dcha = NULL;
    n->tipoNodo = 1;
    n->tipo = "float";
    n->valorNumerico = valor;

    n->resultado = encontrarReg();        // Hacemos llamada al método para buscar un nuevo registro
    n->nombreVar = crearNombreVariable(); // Genera un nombre único para la variable
    printf("# [AST] - Registro $f%d ocupado para var_%d = %.3f\n", n->resultado, n->nombreVar, n->valorNumerico);

    // Actualizar el registro de variables
    variables[n->resultado].dato = n->valorNumerico;
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
    n->valorCadena = valor;

    n->resultado = encontrarReg();        // Hacemos llamada al método para buscar un nuevo registro
    n->nombreVar = crearNombreVariable(); // Genera un nombre único para la variable
    printf("# [AST] - Registro $f%d ocupado para var_%d = %c\n", n->resultado, n->nombreVar, n->valorCadena);

    // Actualizar el registro de variables
    variables[n->resultado].valorCadena = n->valorCadena;
    variables[n->resultado].nombre = n->nombreVar;
    variables[n->resultado].disponible = true;

    return n;
}

// METODO "crearNodoNoTerminal", crea un nuevo nodo, asignamos sus hijos y tipo, y buscamos nuevo registro
struct ast *crearNodoNoTerminal(struct ast *izq, struct ast *dcha, int tipoNodo)
{
    struct ast *n = malloc(sizeof(struct ast)); // Crea un nuevo nodo
    n->izq = izq;
    n->dcha = dcha;
    n->tipoNodo = tipoNodo;        // Asignamos al nodo genérico sus hijos y tipo
    n->resultado = encontrarReg(); // Hacemos llamada al método para buscar un nuevo registro
    return n;
}

// METODO "crearVariableTerminal", crear el nodo hoja para una variable ya creada
struct ast *crearVariableTerminal(double valor, int registro)
{
    struct ast *n = malloc(sizeof(struct ast)); // Asigna memoria dinámicamente para el nuevo nodo
    n->izq = NULL;
    n->dcha = NULL;
    n->tipoNodo = 6;
    n->valorNumerico = valor;

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
    n->valorCadena = valor;

    n->valorCadena = strdup(valor); // Asigna memoria y copia el texto
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
