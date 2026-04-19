
G:/Finn/Stuff/Bristol/COMS30046/src/../bench/build/conv2d.elf:     file format elf32-littleriscv


Disassembly of section .text:

00000000 <_start>:
   0:	00000097          	auipc	ra,0x0
   4:	0b4080e7          	jalr	180(ra) # b4 <main>
   8:	00000073          	ecall
   c:	0000006f          	j	c <_start+0xc>

00000010 <conv2d>:
  10:	ff010113          	addi	sp,sp,-16
  14:	00812623          	sw	s0,12(sp)
  18:	00912423          	sw	s1,8(sp)
  1c:	03058293          	addi	t0,a1,48
  20:	00050493          	mv	s1,a0
  24:	00000413          	li	s0,0
  28:	00000513          	li	a0,0
  2c:	00400393          	li	t2,4
  30:	00241f13          	slli	t5,s0,0x2
  34:	01e48f33          	add	t5,s1,t5
  38:	00060e13          	mv	t3,a2
  3c:	00000f93          	li	t6,0
  40:	000e2023          	sw	zero,0(t3)
  44:	00c58893          	addi	a7,a1,12
  48:	000f0e93          	mv	t4,t5
  4c:	00000813          	li	a6,0
  50:	ff488793          	addi	a5,a7,-12
  54:	000e8693          	mv	a3,t4
  58:	0006a703          	lw	a4,0(a3)
  5c:	0007a303          	lw	t1,0(a5)
  60:	00478793          	addi	a5,a5,4
  64:	00468693          	addi	a3,a3,4
  68:	02670733          	mul	a4,a4,t1
  6c:	00e80833          	add	a6,a6,a4
  70:	010e2023          	sw	a6,0(t3)
  74:	ff1792e3          	bne	a5,a7,58 <conv2d+0x48>
  78:	00c78893          	addi	a7,a5,12
  7c:	018e8e93          	addi	t4,t4,24
  80:	fd1298e3          	bne	t0,a7,50 <conv2d+0x40>
  84:	001f8f93          	addi	t6,t6,1
  88:	004e0e13          	addi	t3,t3,4
  8c:	004f0f13          	addi	t5,t5,4
  90:	fa7f98e3          	bne	t6,t2,40 <conv2d+0x30>
  94:	00150513          	addi	a0,a0,1
  98:	01060613          	addi	a2,a2,16
  9c:	00640413          	addi	s0,s0,6
  a0:	f9f518e3          	bne	a0,t6,30 <conv2d+0x20>
  a4:	00c12403          	lw	s0,12(sp)
  a8:	00812483          	lw	s1,8(sp)
  ac:	01010113          	addi	sp,sp,16
  b0:	00008067          	ret

000000b4 <main>:
  b4:	000007b7          	lui	a5,0x0
  b8:	ef010113          	addi	sp,sp,-272
  bc:	16878793          	addi	a5,a5,360 # 168 <main+0xb4>
  c0:	10112623          	sw	ra,268(sp)
  c4:	00078713          	mv	a4,a5
  c8:	09078613          	addi	a2,a5,144
  cc:	07010693          	addi	a3,sp,112
  d0:	00072883          	lw	a7,0(a4)
  d4:	00472803          	lw	a6,4(a4)
  d8:	00872503          	lw	a0,8(a4)
  dc:	00c72583          	lw	a1,12(a4)
  e0:	0116a023          	sw	a7,0(a3)
  e4:	0106a223          	sw	a6,4(a3)
  e8:	00a6a423          	sw	a0,8(a3)
  ec:	00b6a623          	sw	a1,12(a3)
  f0:	01070713          	addi	a4,a4,16
  f4:	01068693          	addi	a3,a3,16
  f8:	fcc71ce3          	bne	a4,a2,d0 <main+0x1c>
  fc:	0907af03          	lw	t5,144(a5)
 100:	0947ae83          	lw	t4,148(a5)
 104:	0987ae03          	lw	t3,152(a5)
 108:	09c7a303          	lw	t1,156(a5)
 10c:	0a07a883          	lw	a7,160(a5)
 110:	0a47a803          	lw	a6,164(a5)
 114:	0a87a683          	lw	a3,168(a5)
 118:	0ac7a703          	lw	a4,172(a5)
 11c:	0b07a783          	lw	a5,176(a5)
 120:	07010513          	addi	a0,sp,112
 124:	03010613          	addi	a2,sp,48
 128:	00c10593          	addi	a1,sp,12
 12c:	01e12623          	sw	t5,12(sp)
 130:	01d12823          	sw	t4,16(sp)
 134:	01c12a23          	sw	t3,20(sp)
 138:	00612c23          	sw	t1,24(sp)
 13c:	01112e23          	sw	a7,28(sp)
 140:	03012023          	sw	a6,32(sp)
 144:	02d12223          	sw	a3,36(sp)
 148:	02e12423          	sw	a4,40(sp)
 14c:	02f12623          	sw	a5,44(sp)
 150:	00000097          	auipc	ra,0x0
 154:	ec0080e7          	jalr	-320(ra) # 10 <conv2d>
 158:	10c12083          	lw	ra,268(sp)
 15c:	00000513          	li	a0,0
 160:	11010113          	addi	sp,sp,272
 164:	00008067          	ret
