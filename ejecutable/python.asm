
#-------------- Declaracion de variables --------------
.data
saltoLinea: .asciiz "\n"
zero: .float 0.0
uno: .float 1.0
resultado: .space 100
var_0: .asciiz "Buenas sdnoches"

#--------------------- Ejecuciones ---------------------
.text
lwc1 $f31, zero
la $t0, var_0     # Cargar var_0 en $t0
li $v0, 4
la $a0, var_0
addi $v0, $0, 4  #Movemos el registro 12 al 30 iniciado a false
syscall     # Llamada al sistema
li $v0, 4
la $a0, saltoLinea
syscall #Llamada al sistema
