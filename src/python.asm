
#-------------- Declaracion de variables --------------
.data
saltoLinea: .asciiz "\n"
zero: .float 0.0
var_0: .float 7.500
var_1: .float 7.600
var_2: .word 1
var_3: .word 2

#--------------------- Ejecuciones ---------------------
.text
lwc1 $f31, zero
lwc1 $f0, var_0   # Cargar var_0 en $f0
lwc1 $f2, var_1   # Cargar var_1 en $f2
c.lt.s $f0, $f2         # Comparar si $f0 < $f2
bc1t label_true_5        # Si el resultado de la comparación es verdadero, saltar a label_true_5
li $t5, 0                # Si no, almacenar 0 (falso) en $t5
j label_end_5            # Saltar a la etiqueta de finalización
label_true_5:            # Etiqueta si la comparación es verdadera
li $t5, 1                # Almacenar 1 (verdadero) en $t5
label_end_5:             # Etiqueta de finalización
c.eq.s $f0, $f31          # Comparar si $f0 es igual a 0.0
bc1t etiqueta_0          # Si es verdadero (igual a 0.0), saltar a etiqueta fin if
lw $t6, var_2     # Cargar var_2 en $t6
li $v0, 1
move $a0, $t6
syscall     # Llamada al sistema
li $v0, 4
la $a0, saltoLinea
syscall #Llamada al sistema
bc1f etiqueta_1          # Si es falso (no igual a 0.0), saltar a etiqueta de fin condición
etiqueta_0:
c.eq.s $f0, $f2         # Comparar si $f0 == $f2
bc1t label_true_10        # Si el resultado de la comparación es verdadero, saltar a label_true_10
li $t10, 0                # Si no, almacenar 0 (falso) en $t10
j label_end_10            # Saltar a la etiqueta de finalización
label_true_10:            # Etiqueta si la comparación es verdadera
li $t10, 1                # Almacenar 1 (verdadero) en $t10
label_end_10:             # Etiqueta de finalización
c.eq.s $f0, $f31          # Comparar si $f0 es igual a 0.0
bc1t etiqueta_2          # Si es verdadero (igual a 0.0), saltar a etiqueta else
lw $t11, var_3     # Cargar var_3 en $t11
bc1f etiqueta_3          # Si es falso (no igual a 0.0), saltar a etiqueta de fin condición
etiqueta_2:
li $v0, 2
add.s $f12, $f31, $f2
mov.s $f30, $f12    # Movemos el registro 12 al 30 iniciado a false
syscall     # Llamada al sistema
li $v0, 4
la $a0, saltoLinea
syscall #Llamada al sistema
etiqueta_3:
etiqueta_1:
