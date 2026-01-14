	.file	"test.c"
	.option nopic
	.option norelax
	.attribute arch, "rv32i2p1"
	.attribute unaligned_access, 0
	.attribute stack_align, 16
	.text
	.align	2
	.globl	main
	.type	main, @function
main:
	addi	sp,sp,-64
	sw	ra,60(sp)
	sw	s0,56(sp)
	addi	s0,sp,64
	li	a5,6
	sw	a5,-32(s0)
	li	a5,7
	sw	a5,-28(s0)
	li	a5,8
	sw	a5,-24(s0)
	li	a5,4
	sw	a5,-44(s0)
	li	a5,4
	sw	a5,-40(s0)
	li	a5,4
	sw	a5,-36(s0)
	sw	zero,-20(s0)
	j	.L2
.L3:
	lw	a4,-20(s0)
	addi	a5,s0,-32
	slli	a4,a4,2
	add	a5,a4,a5
	lw	a4,0(a5)
	lw	a3,-20(s0)
	addi	a5,s0,-44
	slli	a3,a3,2
	add	a5,a3,a5
	lw	a5,0(a5)
	add	a4,a4,a5
	lw	a3,-20(s0)
	addi	a5,s0,-56
	slli	a3,a3,2
	add	a5,a3,a5
	sw	a4,0(a5)
	lw	a5,-20(s0)
	addi	a5,a5,1
	sw	a5,-20(s0)
.L2:
	lw	a4,-20(s0)
	li	a5,2
	ble	a4,a5,.L3
	lw	a5,-48(s0)
	mv	a0,a5
	lw	ra,60(sp)
	lw	s0,56(sp)
	addi	sp,sp,64
	jr	ra
	.size	main, .-main
	.ident	"GCC: (GNU) 15.1.0"
	.section	.note.GNU-stack,"",@progbits
