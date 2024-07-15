
#-------------- Declaracion de variables --------------
.data
saltoLinea: .asciiz "\n"
zero: .float 0.0
uno: .float 1.0
resultado: .space 100
var_0: .word 3
var_1: .word 5
var_2: .word 2
var_3: .word 2

#--------------------- Ejecuciones ---------------------
.text
lwc1 $f31, zero
lw $t0, var_0     # Cargar var_0 en $t0
l.s $f29, zero
lw $t2, var_1     # Cargar var_1 en $t2
etiqueta0:
c.lt.s $f29, $f2
  bc1f fin_bucle0
    nop
lw $t3, var_2     # Cargar var_2 en $t3
add $t4, $t0, $t3    # Sumar $t0 y $t3, guardar en $t4
l.s $f30, uno
add.s $f29, $f29, $f30
j etiqueta0
fin_bucle0:
l.s $f29, zero
lw $t8, var_3     # Cargar var_3 en $t8
sub $t9, $t4, $t8    # Restar $t8 de $t4, guardar en $t9
li $v0, 1
move $a0, $t9
syscall     # Llamada al sistema
li $v0, 4
la $a0, saltoLinea
syscall #Llamada al sistema
