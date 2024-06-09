bison -d -v gramatica_python.y        
# flex -o latino.lex.c lexico_latino.flex 
flex -o python.lex.c lexico_python.flex 

gcc -o COMPILADO gramatica_python.tab.c python.lex.c

./COMPILADO ./codigo.latino
