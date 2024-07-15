
#-------------- Declaracion de variables --------------
.data
saltoLinea: .asciiz "\n"
zero: .float 0.0
uno: .float 1.0
resultado: .space 100
var_0: .word 6
var_1: .word 7

#--------------------- Ejecuciones ---------------------
.text
lwc1 $f31, zero
lw $t0, var_0     # Cargar var_0 en $t0
lw $t2, var_1     # Cargar var_1 en $t2
mul $t5, $t0, $t2
li $v0, 1
move $a0, $t5
syscall     # Llamada al sistema
li $v0, 4
la $a0, saltoLinea
syscall #Llamada al sistema
