
G:/Finn/Stuff/Bristol/COMS30046/build/add.elf:     file format elf32-littleriscv


Disassembly of section .text:

00000000 <main>:
   0:	fe010113          	addi	sp,sp,-32
   4:	00112e23          	sw	ra,28(sp)
   8:	00812c23          	sw	s0,24(sp)
   c:	02010413          	addi	s0,sp,32
  10:	00600793          	li	a5,6
  14:	fef42623          	sw	a5,-20(s0)
  18:	00400793          	li	a5,4
  1c:	fef42423          	sw	a5,-24(s0)
  20:	fec42703          	lw	a4,-20(s0)
  24:	fe842783          	lw	a5,-24(s0)
  28:	00f707b3          	add	a5,a4,a5
  2c:	fef42223          	sw	a5,-28(s0)
  30:	00000793          	li	a5,0
  34:	00078513          	mv	a0,a5
  38:	01c12083          	lw	ra,28(sp)
  3c:	01812403          	lw	s0,24(sp)
  40:	02010113          	addi	sp,sp,32
  44:	00008067          	ret

00000048 <_start>:
  48:	00000097          	auipc	ra,0x0
  4c:	fb8080e7          	jalr	-72(ra) # 0 <main>
