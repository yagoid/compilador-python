# Proyecto de Compilador de Python a MIPS

## Descripción

El objetivo de esta actividad es construir un compilador utilizando Flex y Bison para el lenguaje de programación Python. El compilador generará código ejecutable en ensamblador de MIPS, el cual se validará en el emulador MARS (MIPS Assembly and Runtime Simulator).

## Tareas

### Flex

- [ ] **Completar Flex**
  - Implementar el analizador léxico utilizando Flex.
  - Definir las expresiones regulares necesarias para identificar los diferentes tokens del lenguaje Python.
  - Asegurarse de que el analizador léxico sea capaz de manejar correctamente los comentarios, las cadenas de texto, las palabras clave, los identificadores, los operadores y los delimitadores.

### Bison

- [ ] **Implementar el Analizador Sintáctico**
  - Utilizar Bison para crear el analizador sintáctico que interpretará la gramática de Python.
  - Definir la gramática completa del lenguaje Python, incluyendo las reglas de producción para expresiones, declaraciones y estructuras de control.
  - Asegurarse de que el analizador sintáctico maneje correctamente los errores de sintaxis y proporcione mensajes de error útiles.

### Tabla de Símbolos

- [ ] **Hacer la Tabla de Símbolos para Python**
  - Implementar una estructura de datos para la tabla de símbolos que almacene información sobre las variables, funciones y otros identificadores.
  - Asegurar que la tabla de símbolos maneje correctamente los diferentes alcances (scopes) del lenguaje Python.
  - Proveer mecanismos para la inserción, búsqueda y actualización de símbolos en la tabla.

### Árbol Sintáctico Abstracto (AST)

- [ ] **Crear el Árbol Sintáctico Abstracto para Python**
  - Implementar una estructura de datos para representar el Árbol Sintáctico Abstracto del código fuente de Python.
  - Diseñar las clases y nodos necesarios para representar las diferentes construcciones sintácticas del lenguaje.

- [ ] **Analizar el AST con MARS**
  - Generar el código ensamblador de MIPS a partir del AST.
  - Asegurarse de que el código generado sea correcto y eficiente.
  - Validar el código ensamblador utilizando el emulador MARS para garantizar que se ejecute correctamente.





