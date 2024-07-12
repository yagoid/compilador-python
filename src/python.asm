
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
etiqueta_0:
c.lt.s $f0, $f2         # Comparar si $f0 < $f2
bc1t label_true_5        # Si el resultado de la comparación es verdadero, saltar a label_true_5
li $t5, 0                # Si no, almacenar 0 (falso) en $t5
j label_end_5            # Saltar a la etiqueta de finalización
label_true_5:            # Etiqueta si la comparación es verdadera
li $t5, 1                # Almacenar 1 (verdadero) en $t5
label_end_5:             # Etiqueta de finalización
lw $t6, var_2     # Cargar var_2 en $t6
li $v0, 1
move $a0, $t6
syscall     # Llamada al sistema
li $v0, 4
la $a0, saltoLinea
syscall #Llamada al sistema
j etiqueta_0
etiqueta_1:
lwc1 $f12, var_3   # Cargar var_3 en $f12
add.s $f13, $f0, $f12  # Sumar $f0 y $f12, guardar en $f13
