
#-------------- Declaracion de variables --------------
.data
saltoLinea: .asciiz "\n"
zero: .float 0.0
var_0: .word 8
var_1: .word 2
var_2: .word 1

#--------------------- Ejecuciones ---------------------
.text
lwc1 $f31, zero
lw $t0, var_0     # Cargar var_0 en $t0
lw $t2, var_1     # Cargar var_1 en $t2
etiqueta_0:
slt $t5, $t2, $t0      # Comparar si $t2 < $t0, almacenar resultado en $t5
beqz $t5, etiqueta_1    # Si $t5 es 0, saltar a etiqueta de fin
li $v0, 1
move $a0, $t2
syscall     # Llamada al sistema
li $v0, 4
la $a0, saltoLinea
syscall #Llamada al sistema
lw $t7, var_2     # Cargar var_2 en $t7
add $t8, $t2, $t7    # Sumar $t2 y $t7, guardar en $t8
j etiqueta_0
etiqueta_1:
