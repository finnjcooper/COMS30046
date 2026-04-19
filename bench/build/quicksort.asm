
G:\Finn\Stuff\Bristol\COMS30046\bench\build\quicksort.elf:     file format elf32-littleriscv


Disassembly of section .text:

00000000 <_start>:
   0:	00000097          	auipc	ra,0x0
   4:	0f8080e7          	jalr	248(ra) # f8 <main>
   8:	00000073          	ecall
   c:	0000006f          	j	c <_start+0xc>

00000010 <quicksort>:
  10:	fc010113          	addi	sp,sp,-64
  14:	00b12023          	sw	a1,0(sp)
  18:	02c12023          	sw	a2,32(sp)
  1c:	00100713          	li	a4,1
  20:	ffc10393          	addi	t2,sp,-4
  24:	01c10293          	addi	t0,sp,28
  28:	00271793          	slli	a5,a4,0x2
  2c:	00f386b3          	add	a3,t2,a5
  30:	00f287b3          	add	a5,t0,a5
  34:	02070263          	beqz	a4,58 <quicksort+0x48>
  38:	0006a583          	lw	a1,0(a3)
  3c:	0007a603          	lw	a2,0(a5)
  40:	ffc68693          	addi	a3,a3,-4
  44:	ffc78793          	addi	a5,a5,-4
  48:	fff70813          	addi	a6,a4,-1
  4c:	00c5ca63          	blt	a1,a2,60 <quicksort+0x50>
  50:	00080713          	mv	a4,a6
  54:	fe0712e3          	bnez	a4,38 <quicksort+0x28>
  58:	04010113          	addi	sp,sp,64
  5c:	00008067          	ret
  60:	00261f93          	slli	t6,a2,0x2
  64:	01f50fb3          	add	t6,a0,t6
  68:	000fae83          	lw	t4,0(t6)
  6c:	00259793          	slli	a5,a1,0x2
  70:	00f507b3          	add	a5,a0,a5
  74:	fff58e13          	addi	t3,a1,-1
  78:	00058693          	mv	a3,a1
  7c:	0007a303          	lw	t1,0(a5)
  80:	00168693          	addi	a3,a3,1
  84:	006ece63          	blt	t4,t1,a0 <quicksort+0x90>
  88:	001e0e13          	addi	t3,t3,1
  8c:	002e1893          	slli	a7,t3,0x2
  90:	011508b3          	add	a7,a0,a7
  94:	0008af03          	lw	t5,0(a7)
  98:	0068a023          	sw	t1,0(a7)
  9c:	01e7a023          	sw	t5,0(a5)
  a0:	00478793          	addi	a5,a5,4
  a4:	fcc6cce3          	blt	a3,a2,7c <quicksort+0x6c>
  a8:	002e1793          	slli	a5,t3,0x2
  ac:	000fa883          	lw	a7,0(t6)
  b0:	00f507b3          	add	a5,a0,a5
  b4:	0047a683          	lw	a3,4(a5)
  b8:	0117a223          	sw	a7,4(a5)
  bc:	00dfa023          	sw	a3,0(t6)
  c0:	03c5c463          	blt	a1,t3,e8 <quicksort+0xd8>
  c4:	00080713          	mv	a4,a6
  c8:	002e0e13          	addi	t3,t3,2
  cc:	f4ce5ee3          	bge	t3,a2,28 <quicksort+0x18>
  d0:	00271793          	slli	a5,a4,0x2
  d4:	002786b3          	add	a3,a5,sp
  d8:	00170713          	addi	a4,a4,1
  dc:	01c6a023          	sw	t3,0(a3)
  e0:	02c6a023          	sw	a2,32(a3)
  e4:	f45ff06f          	j	28 <quicksort+0x18>
  e8:	00281813          	slli	a6,a6,0x2
  ec:	00280833          	add	a6,a6,sp
  f0:	03c82023          	sw	t3,32(a6)
  f4:	fd5ff06f          	j	c8 <quicksort+0xb8>

000000f8 <main>:
  f8:	000007b7          	lui	a5,0x0
  fc:	16c78793          	addi	a5,a5,364 # 16c <main+0x74>
 100:	0007ae83          	lw	t4,0(a5)
 104:	0047ae03          	lw	t3,4(a5)
 108:	0087a303          	lw	t1,8(a5)
 10c:	00c7a883          	lw	a7,12(a5)
 110:	0107a803          	lw	a6,16(a5)
 114:	0147a683          	lw	a3,20(a5)
 118:	0187a703          	lw	a4,24(a5)
 11c:	01c7a783          	lw	a5,28(a5)
 120:	fd010113          	addi	sp,sp,-48
 124:	00700613          	li	a2,7
 128:	00000593          	li	a1,0
 12c:	00010513          	mv	a0,sp
 130:	02112623          	sw	ra,44(sp)
 134:	01d12023          	sw	t4,0(sp)
 138:	01c12223          	sw	t3,4(sp)
 13c:	00612423          	sw	t1,8(sp)
 140:	01112623          	sw	a7,12(sp)
 144:	01012823          	sw	a6,16(sp)
 148:	00d12a23          	sw	a3,20(sp)
 14c:	00e12c23          	sw	a4,24(sp)
 150:	00f12e23          	sw	a5,28(sp)
 154:	00000097          	auipc	ra,0x0
 158:	ebc080e7          	jalr	-324(ra) # 10 <quicksort>
 15c:	02c12083          	lw	ra,44(sp)
 160:	00000513          	li	a0,0
 164:	03010113          	addi	sp,sp,48
 168:	00008067          	ret
