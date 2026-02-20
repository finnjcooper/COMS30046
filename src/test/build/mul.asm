
G:/Finn/Stuff/Bristol/COMS30046/build/mul.elf:     file format elf32-littleriscv


Disassembly of section .text:

00000000 <main>:
   0:	fd010113          	addi	sp,sp,-48
   4:	02112623          	sw	ra,44(sp)
   8:	02812423          	sw	s0,40(sp)
   c:	03010413          	addi	s0,sp,48
  10:	00500793          	li	a5,5
  14:	fef42623          	sw	a5,-20(s0)
  18:	00300793          	li	a5,3
  1c:	fef42423          	sw	a5,-24(s0)
  20:	fec42703          	lw	a4,-20(s0)
  24:	fe842783          	lw	a5,-24(s0)
  28:	02f707b3          	mul	a5,a4,a5
  2c:	fef42223          	sw	a5,-28(s0)
  30:	fec42703          	lw	a4,-20(s0)
  34:	fe842783          	lw	a5,-24(s0)
  38:	02f747b3          	div	a5,a4,a5
  3c:	fef42023          	sw	a5,-32(s0)
  40:	fec42703          	lw	a4,-20(s0)
  44:	fe842783          	lw	a5,-24(s0)
  48:	02f767b3          	rem	a5,a4,a5
  4c:	fcf42e23          	sw	a5,-36(s0)
  50:	00000793          	li	a5,0
  54:	00078513          	mv	a0,a5
  58:	02c12083          	lw	ra,44(sp)
  5c:	02812403          	lw	s0,40(sp)
  60:	03010113          	addi	sp,sp,48
  64:	00008067          	ret

00000068 <_start>:
  68:	00000097          	auipc	ra,0x0
  6c:	f98080e7          	jalr	-104(ra) # 0 <main>
