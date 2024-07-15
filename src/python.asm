
#-------------- Declaracion de variables --------------
.data
saltoLinea: .asciiz "\n"
zero: .float 0.0
var_0: .asciiz "asdf"
var_1: .asciiz "pepe"

#--------------------- Ejecuciones ---------------------
.text
lwc1 $f31, zero
lb $t0, var_0     # Cargar var_0 en $t0
lb $t1, var_1     # Cargar var_1 en $t1
concat_strings_2:
  lb $t0, 0($t0)             # Cargar un byte de la primera cadena
  beqz $t0, copy_second_2    # Si es el fin de la cadena (NUL), ir a copiar la segunda cadena
  sb $t0, 0($t2)             # Almacenar el byte en el destino
  addi $t0, $t0, 1          # Incrementar el puntero de la primera cadena
  addi $t2, $t2, 1          # Incrementar el puntero del destino
  j concat_strings_2         # Repetir el bucle
copy_second_2:
  lb $t0, 0($t1)             # Cargar un byte de la segunda cadena
  beqz $t0, end_concat_2     # Si es el fin de la cadena (NUL), terminar la concatenación
  sb $t0, 0($t2)             # Almacenar el byte en el destino
  addi $t1, $t1, 1          # Incrementar el puntero de la segunda cadena
  addi $t2, $t2, 1          # Incrementar el puntero del destino
  j copy_second_2            # Repetir el bucle
end_concat_2:
  sb $zero, 0($t2)           # Almacenar el carácter NUL al final de la cadena concatenada
