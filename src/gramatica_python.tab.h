/* A Bison parser, made by GNU Bison 2.4.2.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2006, 2009-2010 Free Software
   Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     FALSE = 258,
     NONE = 259,
     TRUE = 260,
     AND = 261,
     AS = 262,
     ASSERT = 263,
     ASYNC = 264,
     AWAIT = 265,
     BREAK = 266,
     CONTINUE = 267,
     CLASS = 268,
     DEF = 269,
     DEL = 270,
     ELIF = 271,
     ELSE = 272,
     EXCEPT = 273,
     FINALLY = 274,
     FOR = 275,
     FROM = 276,
     GLOBAL = 277,
     IF = 278,
     IMPORT = 279,
     IN = 280,
     IS = 281,
     LAMBDA = 282,
     NONLOCAL = 283,
     NOT = 284,
     OR = 285,
     PASS = 286,
     RAISE = 287,
     RETURN = 288,
     TRY = 289,
     WHILE = 290,
     WITH = 291,
     YIELD = 292,
     END = 293,
     IMPRIMIR = 294,
     CADENA = 295,
     VECTOR = 296,
     LISTA = 297,
     TUPLA = 298,
     SET = 299,
     DICT = 300,
     INT = 301,
     FLOAT = 302,
     COMPLEX = 303,
     BOOLEAN = 304,
     SUMA = 305,
     RESTA = 306,
     MULTIPLICACION = 307,
     DIVISION = 308,
     MODULO = 309,
     MENOR_QUE = 310,
     MAYOR_QUE = 311,
     AUMENTAR_VALOR = 312,
     IGUAL_QUE = 313,
     DISTINTO_QUE = 314,
     ASIGNACION = 315,
     PARENTESIS_IZQ = 316,
     PARENTESIS_DER = 317,
     DOS_PUNTOS = 318,
     MENOR_IGUAL_QUE = 319,
     MAYOR_IGUAL_QUE = 320,
     NUMERO = 321,
     DECIMAL = 322,
     VARIABLE = 323,
     STRING = 324
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1685 of yacc.c  */
#line 28 "gramatica_python.y"

  int intVal;
  float realVal;
  char* strVal;
  struct atributos{
    int numero;
    float decimal;
    char* texto;
    int boolean;
    char* tipo;             //Define el tipo que se esta usando
    struct ast *n;          //Para almacenar los nodos del AST
  }tr;



/* Line 1685 of yacc.c  */
#line 136 "gramatica_python.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


