
#-------------- Declaracion de variables --------------
.data
saltoLinea: .asciiz "\n"
zero: .float 0.0
uno: .float 1.0
resultado: .space 100
var_0: .word 40
var_1: .word 8

#--------------------- Ejecuciones ---------------------
.text
lwc1 $f31, zero
lw $t0, var_0     # Cargar var_0 en $t0
lw $t2, var_1     # Cargar var_1 en $t2
li $t3, 0                # Inicializar i a 0 en $t3
etiqueta_0:
bge $t3, $t2, etiqueta_2 # Si i >= límite, saltar a fin del bucle
li $v0, 1
move $a0, $t0
syscall     # Llamada al sistema
li $v0, 4
la $a0, saltoLinea
syscall #Llamada al sistema
addi $t3, $t3, 1       # Incrementar i
j etiqueta_0             # Saltar al inicio del bucle
etiqueta_2:
