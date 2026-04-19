
G:\Finn\Stuff\Bristol\COMS30046\bench\build\factorial.elf:     file format elf32-littleriscv


Disassembly of section .text:

00000000 <_start>:
   0:	00000097          	auipc	ra,0x0
   4:	090080e7          	jalr	144(ra) # 90 <main>
   8:	00000073          	ecall
   c:	0000006f          	j	c <_start+0xc>

00000010 <factorial>:
  10:	00100793          	li	a5,1
  14:	02a7d063          	bge	a5,a0,34 <factorial+0x24>
  18:	00f50733          	add	a4,a0,a5
  1c:	00078513          	mv	a0,a5
  20:	00200793          	li	a5,2
  24:	02f50533          	mul	a0,a0,a5
  28:	00178793          	addi	a5,a5,1
  2c:	fee79ce3          	bne	a5,a4,24 <factorial+0x14>
  30:	00008067          	ret
  34:	00078513          	mv	a0,a5
  38:	00008067          	ret

0000003c <factorial_range>:
  3c:	ff010113          	addi	sp,sp,-16
  40:	00812423          	sw	s0,8(sp)
  44:	00912223          	sw	s1,4(sp)
  48:	01212023          	sw	s2,0(sp)
  4c:	00112623          	sw	ra,12(sp)
  50:	00050493          	mv	s1,a0
  54:	00000413          	li	s0,0
  58:	00c00913          	li	s2,12
  5c:	00140413          	addi	s0,s0,1
  60:	00040513          	mv	a0,s0
  64:	00000097          	auipc	ra,0x0
  68:	fac080e7          	jalr	-84(ra) # 10 <factorial>
  6c:	00a4a023          	sw	a0,0(s1)
  70:	00448493          	addi	s1,s1,4
  74:	ff2414e3          	bne	s0,s2,5c <factorial_range+0x20>
  78:	00c12083          	lw	ra,12(sp)
  7c:	00812403          	lw	s0,8(sp)
  80:	00412483          	lw	s1,4(sp)
  84:	00012903          	lw	s2,0(sp)
  88:	01010113          	addi	sp,sp,16
  8c:	00008067          	ret

00000090 <main>:
  90:	fc010113          	addi	sp,sp,-64
  94:	00010513          	mv	a0,sp
  98:	02112e23          	sw	ra,60(sp)
  9c:	00000097          	auipc	ra,0x0
  a0:	fa0080e7          	jalr	-96(ra) # 3c <factorial_range>
  a4:	03c12083          	lw	ra,60(sp)
  a8:	00000513          	li	a0,0
  ac:	04010113          	addi	sp,sp,64
  b0:	00008067          	ret
