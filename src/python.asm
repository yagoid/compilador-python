
#-------------- Declaracion de variables --------------
.data
saltoLinea: .asciiz "\n"
zero: .float 0.0
var_0: .word 7
var_1: .word 0

#--------------------- Ejecuciones ---------------------
.text
lwc1 $f31, zero
lw $t0, var_0     # Cargar var_0 en $t0
lw $t2, var_1     # Cargar var_1 en $t2
