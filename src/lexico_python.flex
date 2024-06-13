%{
#include <stdio.h>
#include "gramatica_python.tab.h"

int n_linea = 1; // Variable para contar el número de línea
%}

%option noyywrap

digit           [0-9]
letter          [a-zA-Z]
id              {letter}({letter}|{digit})*

%%

False 		{return FALSE; }
None 		{return NONE; }
True 		{return TRUE; }
and 		{return AND; }
as 			{return AS; }
assert 		{return ASSERT; }
async 		{return ASYNC; }
await 		{return AWAIT; }
break 		{return BREAK; }
class 		{return CLASS; }
continue 	{return CONTINUE; }
def 		{return DEF; }
del 		{return DEL; }
elif 		{return ELIF; }
else 		{return ELSE; }
except 		{return EXCEPT; }
finally 	{return FINALLY; }
for 		{return FOR; }
from 		{return FROM; }
global 		{return GLOBAL; }
if 			{return IF; }
import 		{return IMPORT; }
in 			{return IN; }
is 			{return IS; }
lambda 		{return LAMBDA; }
nonlocal 	{return NONLOCAL; }
not 		{return NOT; }
or 			{return OR; }
pass 		{return PASS; }
raise 		{return RAISE; }
return 		{return RETURN; }
try 		{return TRY; }
while 		{return WHILE; }
with 		{return WITH; }
yield 		{return YIELD; }
end 		{return END; }
print 		{return IMPRIMIR; }

str 		{return CADENA; }
bytes 		{return VECTOR; }
list 		{return LISTA; }
tuple 		{return TUPLA; }
set 		{return SET; }
dict 		{return DICT; }
int 		{return INT; }
float 		{return FLOAT; }
complex		{return COMPLEX; }
bool 		{return BOOLEAN; }

"+"         {return SUMA; }
"-"         {return RESTA; }
"*"         {return MULTIPLICACION; }
"/"         {return DIVISION; }
"%"			{return MODULO; }
"<"         {return MENOR_QUE; }
">"         {return MAYOR_QUE; }
"+="        {return AUMENTAR_VALOR; }
"=="        {return IGUAL_QUE; }
"!="        {return DISTINTO_QUE; }
"="         {return ASIGNACION; }
"("         {return PARENTESIS_IZQ; }
")"         {return PARENTESIS_DER; }
":"         {return DOS_PUNTOS; }

{digit}+                   { yylval.intVal = atoi(yytext); return NUMERO; }  
{digit}+"."{digit}*        { yylval.realVal = atof(yytext); return DECIMAL; } 
{id}(_{id})*               { yylval.strVal = strdup(yytext); return VARIABLE; }
\"([^\\"]|\\.)*\"          { yylval.strVal = strdup(yytext); return STRING; } 

\[[[:alnum:]]*\]     							{ /* return ARRAY_SIMPLE; */ }
\[[[:alnum:]]*\]\[[[:alnum:]]*\]      			{ /* return ARRAY_2D; */}
#.*[^\n]+										{ /* ignorar comentario una linea */ }
"/*"([^*]|[\r\n]|(\*+([^*/]|[\r\n])))*\*+"/" 	{ /* ignorar comentario varias lineas */ }
[ \t]											{ /* ignorar tabulaciones */ }

\n              { n_linea++; }

%%
