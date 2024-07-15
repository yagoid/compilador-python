
#-------------- Declaracion de variables --------------
.data
saltoLinea: .asciiz "\n"
zero: .float 0.0
uno: .float 1.0
resultado: .space 100
var_0: .word 7
var_1: .word 8
var_2: .word 8

#--------------------- Ejecuciones ---------------------
.text
lwc1 $f31, zero
lw $t0, var_0     # Cargar var_0 en $t0
lw $t2, var_1     # Cargar var_1 en $t2
l.s $f29, zero
slt $t5, $t0, $t2      # Comparar si $t0 < $t2, almacenar resultado en $t5
beqz $t5, etiqueta_0    # Si $t5 es 0, saltar a etiqueta fin if
lw $t6, var_2     # Cargar var_2 en $t6
li $v0, 1
move $a0, $t6
syscall     # Llamada al sistema
li $v0, 4
la $a0, saltoLinea
syscall #Llamada al sistema
etiqueta_0:
etiqueta_2:
