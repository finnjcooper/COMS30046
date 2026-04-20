
G:\Finn\Stuff\Bristol\COMS30046\bench\build\matmul.elf:     file format elf32-littleriscv


Disassembly of section .text:

00000000 <_start>:
   0:	00000097          	auipc	ra,0x0
   4:	074080e7          	jalr	116(ra) # 74 <main>
   8:	00000073          	ecall
   c:	0000006f          	j	c <_start+0xc>

00000010 <mat_mul>:
  10:	01450e13          	addi	t3,a0,20
  14:	07850f93          	addi	t6,a0,120
  18:	01458f13          	addi	t5,a1,20
  1c:	00058e93          	mv	t4,a1
  20:	00060313          	mv	t1,a2
  24:	00032023          	sw	zero,0(t1)
  28:	000e8813          	mv	a6,t4
  2c:	00050793          	mv	a5,a0
  30:	00000693          	li	a3,0
  34:	0007a703          	lw	a4,0(a5)
  38:	00082883          	lw	a7,0(a6)
  3c:	00478793          	addi	a5,a5,4
  40:	01480813          	addi	a6,a6,20
  44:	03170733          	mul	a4,a4,a7
  48:	00e686b3          	add	a3,a3,a4
  4c:	00d32023          	sw	a3,0(t1)
  50:	ffc792e3          	bne	a5,t3,34 <mat_mul+0x24>
  54:	004e8e93          	addi	t4,t4,4
  58:	00430313          	addi	t1,t1,4
  5c:	fdee94e3          	bne	t4,t5,24 <mat_mul+0x14>
  60:	01478e13          	addi	t3,a5,20
  64:	01450513          	addi	a0,a0,20
  68:	01460613          	addi	a2,a2,20
  6c:	fbfe18e3          	bne	t3,t6,1c <mat_mul+0xc>
  70:	00008067          	ret

00000074 <main>:
  74:	000006b7          	lui	a3,0x0
  78:	ec010113          	addi	sp,sp,-320
  7c:	12468693          	addi	a3,a3,292 # 124 <main+0xb0>
  80:	12112e23          	sw	ra,316(sp)
  84:	00068793          	mv	a5,a3
  88:	06468613          	addi	a2,a3,100
  8c:	00410713          	addi	a4,sp,4
  90:	0007a303          	lw	t1,0(a5)
  94:	0047a883          	lw	a7,4(a5)
  98:	0087a803          	lw	a6,8(a5)
  9c:	00c7a503          	lw	a0,12(a5)
  a0:	0107a583          	lw	a1,16(a5)
  a4:	00672023          	sw	t1,0(a4)
  a8:	01172223          	sw	a7,4(a4)
  ac:	01072423          	sw	a6,8(a4)
  b0:	00a72623          	sw	a0,12(a4)
  b4:	00b72823          	sw	a1,16(a4)
  b8:	01478793          	addi	a5,a5,20
  bc:	01470713          	addi	a4,a4,20
  c0:	fcc798e3          	bne	a5,a2,90 <main+0x1c>
  c4:	0c868693          	addi	a3,a3,200
  c8:	06810713          	addi	a4,sp,104
  cc:	0007a883          	lw	a7,0(a5)
  d0:	0047a803          	lw	a6,4(a5)
  d4:	0087a503          	lw	a0,8(a5)
  d8:	00c7a583          	lw	a1,12(a5)
  dc:	0107a603          	lw	a2,16(a5)
  e0:	01172023          	sw	a7,0(a4)
  e4:	01072223          	sw	a6,4(a4)
  e8:	00a72423          	sw	a0,8(a4)
  ec:	00b72623          	sw	a1,12(a4)
  f0:	00c72823          	sw	a2,16(a4)
  f4:	01478793          	addi	a5,a5,20
  f8:	01470713          	addi	a4,a4,20
  fc:	fcd798e3          	bne	a5,a3,cc <main+0x58>
 100:	06810593          	addi	a1,sp,104
 104:	00410513          	addi	a0,sp,4
 108:	0cc10613          	addi	a2,sp,204
 10c:	00000097          	auipc	ra,0x0
 110:	f04080e7          	jalr	-252(ra) # 10 <mat_mul>
 114:	13c12083          	lw	ra,316(sp)
 118:	00000513          	li	a0,0
 11c:	14010113          	addi	sp,sp,320
 120:	00008067          	ret
