
#-------------- Declaracion de variables --------------
.data
saltoLinea: .asciiz "\n"
zero: .float 0.0
uno: .float 1.0
resultado: .space 100
var_0: .asciiz "Buenas "
var_1: .asciiz "noches"

#--------------------- Ejecuciones ---------------------
.text
lwc1 $f31, zero
la $t0, var_0     # Cargar var_0 en $t0
la $t1, var_1     # Cargar var_1 en $t1
la $s0, resultado
cadena_0: 
  lb $s1, 0($t0)
  beqz $s1, finCadena_0
  sb $s1, 0($s0)
  addi $s0, $s0, 1
  addi $t0, $t0, 1
  j cadena_0
finCadena_0: 
  la $t1, var_1
cadena_1: 
  lb $s1, 0($t1)
  beqz $s1, fin_1
  sb $s1, 0($s0)
  addi $s0, $s0, 1
  addi $t1, $t1, 1
  j cadena_1
fin_1: 
  sb $zero, 0($s0)
li $v0, 4
la $a0, resultado
addi $v0, $0, 4  #Movemos el registro 12 al 30 iniciado a false
syscall     # Llamada al sistema
li $v0, 4
la $a0, saltoLinea
syscall #Llamada al sistema
