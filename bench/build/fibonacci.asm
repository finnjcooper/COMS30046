
build/fibonacci.elf:     file format elf32-littleriscv


Disassembly of section .text:

00000000 <fibonacci>:
   0:	00100793          	li	a5,1
   4:	08a7d263          	bge	a5,a0,88 <fibonacci+0x88>
   8:	fe010113          	addi	sp,sp,-32
   c:	01412423          	sw	s4,8(sp)
  10:	ffe50a13          	addi	s4,a0,-2
  14:	01312623          	sw	s3,12(sp)
  18:	ffea7793          	andi	a5,s4,-2
  1c:	ffd50993          	addi	s3,a0,-3
  20:	00812c23          	sw	s0,24(sp)
  24:	00912a23          	sw	s1,20(sp)
  28:	01212823          	sw	s2,16(sp)
  2c:	00112e23          	sw	ra,28(sp)
  30:	00050493          	mv	s1,a0
  34:	40f989b3          	sub	s3,s3,a5
  38:	fff50413          	addi	s0,a0,-1
  3c:	00000913          	li	s2,0
  40:	00040513          	mv	a0,s0
  44:	00000097          	auipc	ra,0x0
  48:	fbc080e7          	jalr	-68(ra) # 0 <fibonacci>
  4c:	ffe40413          	addi	s0,s0,-2
  50:	00a90933          	add	s2,s2,a0
  54:	ff3416e3          	bne	s0,s3,40 <fibonacci+0x40>
  58:	01c12083          	lw	ra,28(sp)
  5c:	01812403          	lw	s0,24(sp)
  60:	ffe4f493          	andi	s1,s1,-2
  64:	ffe48493          	addi	s1,s1,-2
  68:	409a0a33          	sub	s4,s4,s1
  6c:	012a0533          	add	a0,s4,s2
  70:	01412483          	lw	s1,20(sp)
  74:	01012903          	lw	s2,16(sp)
  78:	00c12983          	lw	s3,12(sp)
  7c:	00812a03          	lw	s4,8(sp)
  80:	02010113          	addi	sp,sp,32
  84:	00008067          	ret
  88:	00008067          	ret

0000008c <main>:
  8c:	ff010113          	addi	sp,sp,-16
  90:	00e00513          	li	a0,14
  94:	00112623          	sw	ra,12(sp)
  98:	00000097          	auipc	ra,0x0
  9c:	f68080e7          	jalr	-152(ra) # 0 <fibonacci>
  a0:	000007b7          	lui	a5,0x0
  a4:	0aa7ac23          	sw	a0,184(a5) # b8 <output>
  a8:	00c12083          	lw	ra,12(sp)
  ac:	00000513          	li	a0,0
  b0:	01010113          	addi	sp,sp,16
  b4:	00008067          	ret
