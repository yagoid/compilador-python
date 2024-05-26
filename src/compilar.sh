bison -d -v gramatica_python.y        
# flex -o latino.lex.c lexico_latino.flex 
flex -o lexico_python.flex 
# gcc -o COMPILADO gramatica_latino.tab.c latino.lex.c
./COMPILADO ./codigo.latino
