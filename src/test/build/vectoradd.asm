
G:/Finn/Stuff/Bristol/COMS30046/build/vectoradd.elf:     file format elf32-littleriscv


Disassembly of section .text:

00000000 <main>:
   0:	f7010113          	addi	sp,sp,-144
   4:	08112623          	sw	ra,140(sp)
   8:	08812423          	sw	s0,136(sp)
   c:	09010413          	addi	s0,sp,144
  10:	000007b7          	lui	a5,0x0
  14:	14078793          	addi	a5,a5,320 # 140 <_start+0x8>
  18:	0007ae03          	lw	t3,0(a5)
  1c:	0047a303          	lw	t1,4(a5)
  20:	0087a883          	lw	a7,8(a5)
  24:	00c7a803          	lw	a6,12(a5)
  28:	0107a503          	lw	a0,16(a5)
  2c:	0147a583          	lw	a1,20(a5)
  30:	0187a603          	lw	a2,24(a5)
  34:	01c7a683          	lw	a3,28(a5)
  38:	0207a703          	lw	a4,32(a5)
  3c:	fdc42223          	sw	t3,-60(s0)
  40:	fc642423          	sw	t1,-56(s0)
  44:	fd142623          	sw	a7,-52(s0)
  48:	fd042823          	sw	a6,-48(s0)
  4c:	fca42a23          	sw	a0,-44(s0)
  50:	fcb42c23          	sw	a1,-40(s0)
  54:	fcc42e23          	sw	a2,-36(s0)
  58:	fed42023          	sw	a3,-32(s0)
  5c:	fee42223          	sw	a4,-28(s0)
  60:	0247a783          	lw	a5,36(a5)
  64:	fef42423          	sw	a5,-24(s0)
  68:	000007b7          	lui	a5,0x0
  6c:	16878793          	addi	a5,a5,360 # 168 <_start+0x30>
  70:	0007ae03          	lw	t3,0(a5)
  74:	0047a303          	lw	t1,4(a5)
  78:	0087a883          	lw	a7,8(a5)
  7c:	00c7a803          	lw	a6,12(a5)
  80:	0107a503          	lw	a0,16(a5)
  84:	0147a583          	lw	a1,20(a5)
  88:	0187a603          	lw	a2,24(a5)
  8c:	01c7a683          	lw	a3,28(a5)
  90:	0207a703          	lw	a4,32(a5)
  94:	f9c42e23          	sw	t3,-100(s0)
  98:	fa642023          	sw	t1,-96(s0)
  9c:	fb142223          	sw	a7,-92(s0)
  a0:	fb042423          	sw	a6,-88(s0)
  a4:	faa42623          	sw	a0,-84(s0)
  a8:	fab42823          	sw	a1,-80(s0)
  ac:	fac42a23          	sw	a2,-76(s0)
  b0:	fad42c23          	sw	a3,-72(s0)
  b4:	fae42e23          	sw	a4,-68(s0)
  b8:	0247a783          	lw	a5,36(a5)
  bc:	fcf42023          	sw	a5,-64(s0)
  c0:	fe042623          	sw	zero,-20(s0)
  c4:	0500006f          	j	114 <main+0x114>
  c8:	fec42703          	lw	a4,-20(s0)
  cc:	fc440793          	addi	a5,s0,-60
  d0:	00271713          	slli	a4,a4,0x2
  d4:	00f707b3          	add	a5,a4,a5
  d8:	0007a703          	lw	a4,0(a5)
  dc:	fec42683          	lw	a3,-20(s0)
  e0:	f9c40793          	addi	a5,s0,-100
  e4:	00269693          	slli	a3,a3,0x2
  e8:	00f687b3          	add	a5,a3,a5
  ec:	0007a783          	lw	a5,0(a5)
  f0:	00f70733          	add	a4,a4,a5
  f4:	fec42683          	lw	a3,-20(s0)
  f8:	f7440793          	addi	a5,s0,-140
  fc:	00269693          	slli	a3,a3,0x2
 100:	00f687b3          	add	a5,a3,a5
 104:	00e7a023          	sw	a4,0(a5)
 108:	fec42783          	lw	a5,-20(s0)
 10c:	00178793          	addi	a5,a5,1
 110:	fef42623          	sw	a5,-20(s0)
 114:	fec42703          	lw	a4,-20(s0)
 118:	00900793          	li	a5,9
 11c:	fae7d6e3          	bge	a5,a4,c8 <main+0xc8>
 120:	00000793          	li	a5,0
 124:	00078513          	mv	a0,a5
 128:	08c12083          	lw	ra,140(sp)
 12c:	08812403          	lw	s0,136(sp)
 130:	09010113          	addi	sp,sp,144
 134:	00008067          	ret

00000138 <_start>:
 138:	00000097          	auipc	ra,0x0
 13c:	ec8080e7          	jalr	-312(ra) # 0 <main>
