
#-------------- Declaracion de variables --------------
.data
saltoLinea: .asciiz "\n"
zero: .float 0.0
var_0: .float 7.500
var_1: .float 7.600
var_2: .float 0.100
var_3: .word 1

#--------------------- Ejecuciones ---------------------
.text
lwc1 $f31, zero
lwc1 $f0, var_0   # Cargar var_0 en $f0
lwc1 $f2, var_1   # Cargar var_1 en $f2
etiqueta_0:
c.lt.s $f5, $f0, $f2
lwc1 $f6, var_2   # Cargar var_2 en $f6
lw $t9, var_3     # Cargar var_3 en $t9
li $v0, 1
move $a0, $t9
syscall     # Llamada al sistema
li $v0, 4
la $a0, saltoLinea
syscall #Llamada al sistema
j etiqueta_0
etiqueta_1:
