	.set noreorder

.global HijackvshCtrlReadBufferPositive
.ent	HijackvshCtrlReadBufferPositive
HijackvshCtrlReadBufferPositive:
	addiu $sp, $sp, -4
	sw $v1, 0($sp)
	jal vshCtrlReadBufferPositivePatched
	move $a0, $s0
	lw $v1, 0($sp)
	addiu $sp, $sp, 4
	lw $v0, 4($s0)
	lw $ra, 4($sp)
	lui $a1, %hi(jump_vshctrl)
	addiu $a1, $a1, %lo(jump_vshctrl)
	lw $a1, 0($a1)
	jr $a1
	nop
.end HijackvshCtrlReadBufferPositive

