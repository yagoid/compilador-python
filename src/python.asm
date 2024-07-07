
#-------------- Declaracion de variables --------------
.data
saltoLinea: .asciiz "\n"
zero: .float 0.0
var_0: .asciiz "Hola"
var_1: .asciiz " Peter"

#--------------------- Ejecuciones ---------------------
.text
lwc1 $f31, zero
lb $t0, var_0     # Cargar var_0 en $t0
lb $t2, var_1     # Cargar var_1 en $t2
concat_strings_5:
lb $t0, 0($t0)             # Cargar un byte de la primera cadena
beqz $t0, copy_second_5    # Si es el fin de la cadena (NUL), ir a copiar la segunda cadena
sb $t0, 0($t5)             # Almacenar el byte en el destino
addi $t0, $t0, 1          # Incrementar el puntero de la primera cadena
addi $t5, $t5, 1          # Incrementar el puntero del destino
j concat_strings_5         # Repetir el bucle
copy_second_5:
lb $t0, 0($t2)             # Cargar un byte de la segunda cadena
beqz $t0, end_concat_5     # Si es el fin de la cadena (NUL), terminar la concatenaci?n
sb $t0, 0($t5)             # Almacenar el byte en el destino
addi $t2, $t2, 1          # Incrementar el puntero de la segunda cadena
addi $t5, $t5, 1          # Incrementar el puntero del destino
j copy_second_5            # Repetir el bucle
end_concat_5:
sb $zero, 0($t5)           # Almacenar el car?cter NUL al final de la cadena concatenada
