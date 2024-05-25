%{
#include <stdio.h>
#include "parser.tab.h"

int n_linea = 1; // Variable para contar el número de línea
%}

%option noyywrap

digit           [0-9]
letter          [a-zA-Z]
id              {letter}({letter}|{digit})*

%%

False 		{return "FALSE";}
None 		{return "NONE";}
True 		{return "TRUE";}
and 		{return "AND";}
as 			{return "AS";}
assert 		{return "ASSERT";}
async 		{return "ASYNC";}
await 		{return "AWAIT";}
break 		{return "BREAK";}
class 		{return "CLASS";}
continue 	{return "CONTINUE";}
def 		{return "DEF";}
del 		{return "DEL";}
elif 		{return "ELIF";}
else 		{return "ELSE";}
except 		{return "EXCEPT";}
finally 	{return "FINALLY";}
for 		{return "FOR";}
from 		{return "FROM";}
global 		{return "GLOBAL";}
if 			{return "IF";}
import 		{return "IMPORT";}
in 			{return "IN";}
is 			{return "IS";}
lambda 		{return "LAMBDA";}
nonlocal 	{return "NONLOCAL";}
not 		{return "NOT";}
or 			{return "OR";}
pass 		{return "PASS";}
raise 		{return "RAISE";}
return 		{return "RETURN";}
try 		{return "TRY";}
while 		{return "WHILE";}
with 		{return "WITH";}
yield 		{return "YIELD";}
end 		{return "END";}
def 		{return "FUNCION";}

str 		{return "CADENA";}
bytes 		{return "VECTOR";}
list 		{return "SECUENCIA";}
tuple 		{return "SECUENCIA";}
set 		{return "CONJUNTO";}
frozenset	{return "CONJUNTO";}
dict 		{return "DICCIONARIO";}
int 		{return "NUMERO ENTERO";}
float 		{return "NUMERO DECIMAL";}
complex		{return "NUMERO COMPLEJO";}
bool 		{return "BOOLEANO";}

"+"         {return "SUMA";}
"-"         {return "RESTA";}
"*"         {return "MULTIPLICACION";}
"/"         {return "DIVISION";}
"%"			{return "MODULO";}
"<"         {return "MENOR QUE";}
">"         {return "MAYOR QUE";}
"+="        {return "AUMENTAR VALOR";}
"=="        {return "IGUAL QUE";}
"!="        {return "DISTINTO QUE";}
"="         {return "ASIGNACION";}
"("         {return "PARENTESISIZQ";}
")"         {return "PARENTESISDER";}

{id}(_{id})*   					{return "VARIABLE";}
{digit}+       	 				{return "NUMERO";}
{digit}+"."{digit}*    			{return "DECIMAL";}

\[[[:alnum:]]*\]     							{ return "ARRAY_SIMPLE";}
\[[[:alnum:]]*\]\[[[:alnum:]]*\]      			{ return "ARRAY_2D";}
#.*[^\n]+										; //ignora comentario de una linea
"/*"([^*]|[\r\n]|(\*+([^*/]|[\r\n])))*\*+"/" 	; //ignora comentarios de varias lineas
[ \t]											; //ignora tabulaciones


\n              { n_linea++;}
%%

int main(int argc,char *argv[]) {
	char entrada[100] = "../pruebas/";
	char salida[100] = "../pruebas/";
	strcat(entrada, argv[1]);
	strcat(salida, argv[2]);
	yyin = fopen(entrada, "rt");
	yyout = fopen(salida, "wt");
	if (yyin == NULL || yyout == NULL) {
		printf("\nNo se puede abrir el archivo: %s\n", salida);
		exit(-1);
	} else {
		fprintf(yyout,"ANALIZADOR LÉXICO\n\n\n");
		fprintf(yyout,"%-50s %-90s %-15s \n", "TOKEN","LEXEMA","LINEA");
		fprintf(yyout,"%-50s %-90s %-15s \n", "-----","------","-----");
		yylex();
		printf("\nFichero %s correcto!!\n", salida);
	}
	fclose(yyin);
	fclose(yyout);
	return 0;
}
