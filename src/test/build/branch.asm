
G:/Finn/Stuff/Bristol/COMS30046/build/branch.elf:     file format elf32-littleriscv


Disassembly of section .text:

00000000 <main>:
   0:	fe010113          	addi	sp,sp,-32
   4:	00112e23          	sw	ra,28(sp)
   8:	00812c23          	sw	s0,24(sp)
   c:	02010413          	addi	s0,sp,32
  10:	00a00793          	li	a5,10
  14:	fef42423          	sw	a5,-24(s0)
  18:	00500793          	li	a5,5
  1c:	fef42223          	sw	a5,-28(s0)
  20:	fe042623          	sw	zero,-20(s0)
  24:	01c0006f          	j	40 <main+0x40>
  28:	fec42703          	lw	a4,-20(s0)
  2c:	fe442783          	lw	a5,-28(s0)
  30:	02e7c063          	blt	a5,a4,50 <main+0x50>
  34:	fec42783          	lw	a5,-20(s0)
  38:	00178793          	addi	a5,a5,1
  3c:	fef42623          	sw	a5,-20(s0)
  40:	fec42703          	lw	a4,-20(s0)
  44:	fe842783          	lw	a5,-24(s0)
  48:	fef740e3          	blt	a4,a5,28 <main+0x28>
  4c:	0080006f          	j	54 <main+0x54>
  50:	00000013          	nop
  54:	00000793          	li	a5,0
  58:	00078513          	mv	a0,a5
  5c:	01c12083          	lw	ra,28(sp)
  60:	01812403          	lw	s0,24(sp)
  64:	02010113          	addi	sp,sp,32
  68:	00008067          	ret

0000006c <_start>:
  6c:	00000097          	auipc	ra,0x0
  70:	f94080e7          	jalr	-108(ra) # 0 <main>
