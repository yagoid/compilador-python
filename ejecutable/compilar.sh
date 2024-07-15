bison -d -v ../src/gramatica_python.y        

flex -o ../src/python.lex.c ../src/lexico_python.flex 

gcc -o COMPILADO ../src/gramatica_python.tab.c ../src/python.lex.c

./COMPILADO ../pruebas/validas/input_aritmetico.py
