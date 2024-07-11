
#-------------- Declaracion de variables --------------
.data
saltoLinea: .asciiz "\n"
zero: .float 0.0
var_0: .float 7.500
var_1: .float 7.600
var_2: .word 1
var_3: .float 0.100

#--------------------- Ejecuciones ---------------------
.text
lwc1 $f31, zero
lwc1 $f0, var_0   # Cargar var_0 en $f0
lwc1 $f2, var_1   # Cargar var_1 en $f2
c.lt.s $f5, $f0, $f2
j etiqueta_1
etiqueta_0:
etiqueta_1:
lwc1 $f12, var_3   # Cargar var_3 en $f12
