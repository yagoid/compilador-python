
#-------------- Declaracion de variables --------------
.data
saltoLinea: .asciiz "\n"
zero: .float 0.0
uno: .float 1.0
resultado: .space 100
var_0: .float 1.500
var_1: .float 2.500
var_2: .float 3.500
var_3: .float 3.500
var_4: .float 1.500

#--------------------- Ejecuciones ---------------------
.text
lwc1 $f31, zero
lwc1 $f0, var_0   # Cargar var_0 en $f0
lwc1 $f2, var_1   # Cargar var_1 en $f2
lwc1 $f5, var_2   # Cargar var_2 en $f5
l.s $f29, zero
lwc1 $f8, var_3   # Cargar var_3 en $f8
c.le.s $f8, $f0
bc1t es_mayor_o_igual_2
nop
li $t0, 0
mtc1 $t0, $f9
j fin_mayor_o_igual_3
es_mayor_o_igual_2:
li $t0, 1065353216
mtc1 $t0, $f9
fin_mayor_o_igual_3:
c.eq.s $f9, $f29          # Comparar si $f9 es igual a 0.0
bc1t etiqueta_0          # Si es verdadero (igual a 0.0), saltar a etiqueta fin if
li $v0, 2
add.s $f12, $f31, $f0
mov.s $f30, $f12    # Movemos el registro 12 al 30 iniciado a false
syscall     # Llamada al sistema
li $v0, 4
la $a0, saltoLinea
syscall #Llamada al sistema
bc1f etiqueta_1          # Si es falso (no igual a 0.0), saltar a etiqueta de fin condición
etiqueta_0:
l.s $f29, zero
lwc1 $f11, var_4   # Cargar var_4 en $f11
c.le.s $f11, $f0
bc1t es_mayor_o_igual_4
nop
li $t0, 0
mtc1 $t0, $f12
j fin_mayor_o_igual_5
es_mayor_o_igual_4:
li $t0, 1065353216
mtc1 $t0, $f12
fin_mayor_o_igual_5:
c.eq.s $f12, $f29          # Comparar si $f12 es igual a 0.0
bc1t etiqueta_2          # Si es verdadero (igual a 0.0), saltar a etiqueta else
li $v0, 2
add.s $f12, $f31, $f2
mov.s $f30, $f12    # Movemos el registro 12 al 30 iniciado a false
syscall     # Llamada al sistema
li $v0, 4
la $a0, saltoLinea
syscall #Llamada al sistema
bc1f etiqueta_3          # Si es falso (no igual a 0.0), saltar a etiqueta de fin condición
etiqueta_2:
li $v0, 2
add.s $f12, $f31, $f5
mov.s $f30, $f12    # Movemos el registro 12 al 30 iniciado a false
syscall     # Llamada al sistema
li $v0, 4
la $a0, saltoLinea
syscall #Llamada al sistema
etiqueta_4:
etiqueta_3:
etiqueta_1:
