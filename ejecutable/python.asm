
#-------------- Declaracion de variables --------------
.data
saltoLinea: .asciiz "\n"
zero: .float 0.0
uno: .float 1.0
resultado: .space 100
var_0: .float 4.000
var_1: .word 2
var_2: .float 2.000
var_3: .float 1.000

#--------------------- Ejecuciones ---------------------
.text
lwc1 $f31, zero
lwc1 $f0, var_0   # Cargar var_0 en $f0
l.s $f29, zero
lw $t2, var_1     # Cargar var_1 en $t2
etiqueta0:
c.lt.s $f29, $f2
  bc1f fin_bucle0
    nop
lwc1 $f3, var_2   # Cargar var_2 en $f3
sub.s $f4, $f0, $f3    # Restar $f3 de $f0, guardar en $f4
l.s $f30, uno
add.s $f29, $f29, $f30
j etiqueta0
fin_bucle0:
l.s $f29, zero
lwc1 $f8, var_3   # Cargar var_3 en $f8
add.s $f9, $f4, $f8  # Sumar $f4 y $f8, guardar en $f9
li $v0, 2
add.s $f12, $f31, $f9
mov.s $f30, $f12    # Movemos el registro 12 al 30 iniciado a false
syscall     # Llamada al sistema
li $v0, 4
la $a0, saltoLinea
syscall #Llamada al sistema
