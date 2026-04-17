
G:/Finn/Stuff/Bristol/COMS30046/src/bench/build/matmul.elf:     file format elf32-littleriscv


Disassembly of section .text:

00000000 <mat_mul>:
   0:	01450e13          	addi	t3,a0,20
   4:	07850f93          	addi	t6,a0,120
   8:	01458f13          	addi	t5,a1,20
   c:	00058e93          	mv	t4,a1
  10:	00060313          	mv	t1,a2
  14:	00032023          	sw	zero,0(t1)
  18:	000e8813          	mv	a6,t4
  1c:	00050793          	mv	a5,a0
  20:	00000693          	li	a3,0
  24:	0007a703          	lw	a4,0(a5)
  28:	00082883          	lw	a7,0(a6)
  2c:	00478793          	addi	a5,a5,4
  30:	01480813          	addi	a6,a6,20
  34:	03170733          	mul	a4,a4,a7
  38:	00e686b3          	add	a3,a3,a4
  3c:	00d32023          	sw	a3,0(t1)
  40:	ffc792e3          	bne	a5,t3,24 <mat_mul+0x24>
  44:	004e8e93          	addi	t4,t4,4
  48:	00430313          	addi	t1,t1,4
  4c:	fdee94e3          	bne	t4,t5,14 <mat_mul+0x14>
  50:	01478e13          	addi	t3,a5,20
  54:	01450513          	addi	a0,a0,20
  58:	01460613          	addi	a2,a2,20
  5c:	fbfe18e3          	bne	t3,t6,c <mat_mul+0xc>
  60:	00008067          	ret

00000064 <main>:
  64:	000006b7          	lui	a3,0x0
  68:	ec010113          	addi	sp,sp,-320
  6c:	11468693          	addi	a3,a3,276 # 114 <main+0xb0>
  70:	12112e23          	sw	ra,316(sp)
  74:	00068793          	mv	a5,a3
  78:	06468613          	addi	a2,a3,100
  7c:	00410713          	addi	a4,sp,4
  80:	0007a303          	lw	t1,0(a5)
  84:	0047a883          	lw	a7,4(a5)
  88:	0087a803          	lw	a6,8(a5)
  8c:	00c7a503          	lw	a0,12(a5)
  90:	0107a583          	lw	a1,16(a5)
  94:	00672023          	sw	t1,0(a4)
  98:	01172223          	sw	a7,4(a4)
  9c:	01072423          	sw	a6,8(a4)
  a0:	00a72623          	sw	a0,12(a4)
  a4:	00b72823          	sw	a1,16(a4)
  a8:	01478793          	addi	a5,a5,20
  ac:	01470713          	addi	a4,a4,20
  b0:	fcc798e3          	bne	a5,a2,80 <main+0x1c>
  b4:	0c868693          	addi	a3,a3,200
  b8:	06810713          	addi	a4,sp,104
  bc:	0007a883          	lw	a7,0(a5)
  c0:	0047a803          	lw	a6,4(a5)
  c4:	0087a503          	lw	a0,8(a5)
  c8:	00c7a583          	lw	a1,12(a5)
  cc:	0107a603          	lw	a2,16(a5)
  d0:	01172023          	sw	a7,0(a4)
  d4:	01072223          	sw	a6,4(a4)
  d8:	00a72423          	sw	a0,8(a4)
  dc:	00b72623          	sw	a1,12(a4)
  e0:	00c72823          	sw	a2,16(a4)
  e4:	01478793          	addi	a5,a5,20
  e8:	01470713          	addi	a4,a4,20
  ec:	fcd798e3          	bne	a5,a3,bc <main+0x58>
  f0:	00410513          	addi	a0,sp,4
  f4:	06810593          	addi	a1,sp,104
  f8:	0cc10613          	addi	a2,sp,204
  fc:	00000097          	auipc	ra,0x0
 100:	f04080e7          	jalr	-252(ra) # 0 <mat_mul>
 104:	13c12083          	lw	ra,316(sp)
 108:	00000513          	li	a0,0
 10c:	14010113          	addi	sp,sp,320
 110:	00008067          	ret
