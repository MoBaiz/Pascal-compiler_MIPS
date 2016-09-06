.data
str1:	.ASCIIZ	"Please Input a:"
str2:	.ASCIIZ	"Please Input b:"
str3:	.ASCIIZ	"gcd("
str4:	.ASCIIZ	","
str5:	.ASCIIZ	")="

.text
j	main
mod1:
sw	$t0,-16($fp)
sw	$t1,-20($fp)
sw	$t2,-24($fp)
sw	$t3,-28($fp)
sw	$t4,-32($fp)
sw	$t5,-36($fp)
sw	$t6,-40($fp)
sw	$t7,-44($fp)
sw	$a2,-48($fp)
sw	$a3,-52($fp)
move	$fp,$sp
sw	$ra,-12($fp)
move	$t8,$fp
lw	$t1,-60($t8)
move	$t8,$fp
lw	$t2,-56($t8)
div	$t2,$t1
mflo	$t3
mult	$t3,$t1
mflo	$t4
subu	$t5,$t2,$t4
move	$t8,$fp
lw	$t8,-4($t8)
lw	$t6,-68($t8)
move	$t6,$t5
move	$t8,$fp
sw	$t1,-60($t8)
move	$t8,$fp
sw	$t2,-56($t8)
move	$t8,$fp
lw	$t8,-4($t8)
sw	$t6,-68($t8)
lw	$ra,-12($fp)
addiu	$sp,$fp,4
lw	$fp,-8($fp)
lw	$t0,-16($fp)
lw	$t1,-20($fp)
lw	$t2,-24($fp)
lw	$t3,-28($fp)
lw	$t4,-32($fp)
lw	$t5,-36($fp)
lw	$t6,-40($fp)
lw	$t7,-44($fp)
lw	$a2,-48($fp)
lw	$a3,-52($fp)
jr	$ra
gcd0:
sw	$t0,-16($fp)
sw	$t1,-20($fp)
sw	$t2,-24($fp)
sw	$t3,-28($fp)
sw	$t4,-32($fp)
sw	$t5,-36($fp)
sw	$t6,-40($fp)
sw	$t7,-44($fp)
sw	$a2,-48($fp)
sw	$a3,-52($fp)
move	$fp,$sp
sw	$ra,-12($fp)
move	$t8,$fp
lw	$t7,-60($t8)
move	$v0,$t7
move	$t8,$fp
sw	$t7,-60($t8)
li	$v1,0
beq	$v0,$v1,label0
addiu	$sp,$fp,-72
move	$t8,$fp
lw	$t0,-60($t8)
sw	$t0,-56($sp)
addiu	$sp,$fp,-76
move	$t8,$fp
lw	$t1,-56($t8)
sw	$t1,-56($sp)
sw	$t0,-60($sp)
move	$t8,$fp
sw	$t0,-60($t8)
move	$t8,$fp
sw	$t1,-56($t8)
move	$t8,$fp
sw	$t8,-4($sp)
sw	$fp,-8($sp)
jal	mod1
move	$a2,$a3
move	$t8,$fp
lw	$t8,-4($t8)
lw	$a3,-68($t8)
move	$t8,$fp
lw	$t2,-68($t8)
sw	$t2,-60($sp)
move	$t8,$fp
sw	$t2,-68($t8)
move	$t8,$fp
lw	$t8,-4($t8)
sw	$t8,-4($sp)
sw	$fp,-8($sp)
jal	gcd0
move	$a2,$a3
move	$t8,$fp
lw	$t8,-4($t8)
lw	$a3,-68($t8)
move	$t8,$fp
lw	$t8,-4($t8)
lw	$t3,-64($t8)
move	$t3,$t3
move	$t8,$fp
lw	$t8,-4($t8)
sw	$t3,-64($t8)
j	label1
label0:
move	$t8,$fp
lw	$t4,-56($t8)
move	$t8,$fp
lw	$t8,-4($t8)
lw	$t5,-64($t8)
move	$t5,$t4
move	$t8,$fp
sw	$t4,-56($t8)
move	$t8,$fp
lw	$t8,-4($t8)
sw	$t5,-64($t8)
label1:
lw	$ra,-12($fp)
addiu	$sp,$fp,4
lw	$fp,-8($fp)
lw	$t0,-16($fp)
lw	$t1,-20($fp)
lw	$t2,-24($fp)
lw	$t3,-28($fp)
lw	$t4,-32($fp)
lw	$t5,-36($fp)
lw	$t6,-40($fp)
lw	$t7,-44($fp)
lw	$a2,-48($fp)
lw	$a3,-52($fp)
jr	$ra
main:
move	$fp,$sp
sw	$fp,-4($sp)
la	$a0,str1
li	$v0,4
syscall
li	$v0,5
syscall
move	$t8,$fp
lw	$t6,-56($t8)
move	$t6,$v0
la	$a0,str2
li	$v0,4
syscall
li	$v0,5
syscall
move	$t8,$fp
lw	$t7,-60($t8)
move	$t7,$v0
la	$a0,str3
li	$v0,4
syscall
move	$a0,$t6
li	$v0,1
syscall
la	$a0,str4
li	$v0,4
syscall
move	$a0,$t7
li	$v0,1
syscall
la	$a0,str5
li	$v0,4
syscall
addiu	$sp,$fp,-76
sw	$t6,-56($sp)
sw	$t7,-60($sp)
move	$t8,$fp
sw	$t6,-56($t8)
move	$t8,$fp
sw	$t7,-60($t8)
move	$t8,$fp
sw	$t8,-4($sp)
sw	$fp,-8($sp)
jal	gcd0
move	$a2,$a3
move	$t8,$fp
lw	$t8,-4($t8)
lw	$a3,-64($t8)
move	$t8,$fp
lw	$t0,-64($t8)
move	$a0,$t0
li	$v0,1
syscall
