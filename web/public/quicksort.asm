
build/quicksort.elf:     file format elf32-littleriscv


Disassembly of section .text:

00000000 <partition>:
   0:	00261813          	slli	a6,a2,0x2
   4:	01050833          	add	a6,a0,a6
   8:	00082883          	lw	a7,0(a6)
   c:	fff58693          	addi	a3,a1,-1
  10:	04c5de63          	bge	a1,a2,6c <partition+0x6c>
  14:	00259593          	slli	a1,a1,0x2
  18:	00b505b3          	add	a1,a0,a1
  1c:	0005a703          	lw	a4,0(a1)
  20:	00e8ce63          	blt	a7,a4,3c <partition+0x3c>
  24:	00168693          	addi	a3,a3,1
  28:	00269793          	slli	a5,a3,0x2
  2c:	00f507b3          	add	a5,a0,a5
  30:	0007a603          	lw	a2,0(a5)
  34:	00e7a023          	sw	a4,0(a5)
  38:	00c5a023          	sw	a2,0(a1)
  3c:	00458593          	addi	a1,a1,4
  40:	fcb81ee3          	bne	a6,a1,1c <partition+0x1c>
  44:	00168693          	addi	a3,a3,1
  48:	00068593          	mv	a1,a3
  4c:	00269693          	slli	a3,a3,0x2
  50:	00d50533          	add	a0,a0,a3
  54:	00082883          	lw	a7,0(a6)
  58:	00052783          	lw	a5,0(a0)
  5c:	01152023          	sw	a7,0(a0)
  60:	00f82023          	sw	a5,0(a6)
  64:	00058513          	mv	a0,a1
  68:	00008067          	ret
  6c:	00058693          	mv	a3,a1
  70:	00269693          	slli	a3,a3,0x2
  74:	00d50533          	add	a0,a0,a3
  78:	00052783          	lw	a5,0(a0)
  7c:	01152023          	sw	a7,0(a0)
  80:	00058513          	mv	a0,a1
  84:	00f82023          	sw	a5,0(a6)
  88:	00008067          	ret

0000008c <quicksort>:
  8c:	06c5d663          	bge	a1,a2,f8 <quicksort+0x6c>
  90:	fe010113          	addi	sp,sp,-32
  94:	00812c23          	sw	s0,24(sp)
  98:	01212823          	sw	s2,16(sp)
  9c:	01312623          	sw	s3,12(sp)
  a0:	00112e23          	sw	ra,28(sp)
  a4:	00058413          	mv	s0,a1
  a8:	00060913          	mv	s2,a2
  ac:	00050993          	mv	s3,a0
  b0:	00040593          	mv	a1,s0
  b4:	00090613          	mv	a2,s2
  b8:	00098513          	mv	a0,s3
  bc:	00000097          	auipc	ra,0x0
  c0:	f44080e7          	jalr	-188(ra) # 0 <partition>
  c4:	00040593          	mv	a1,s0
  c8:	fff50613          	addi	a2,a0,-1
  cc:	00150413          	addi	s0,a0,1
  d0:	00098513          	mv	a0,s3
  d4:	00000097          	auipc	ra,0x0
  d8:	fb8080e7          	jalr	-72(ra) # 8c <quicksort>
  dc:	fd244ae3          	blt	s0,s2,b0 <quicksort+0x24>
  e0:	01c12083          	lw	ra,28(sp)
  e4:	01812403          	lw	s0,24(sp)
  e8:	01012903          	lw	s2,16(sp)
  ec:	00c12983          	lw	s3,12(sp)
  f0:	02010113          	addi	sp,sp,32
  f4:	00008067          	ret
  f8:	00008067          	ret

000000fc <main>:
  fc:	00000537          	lui	a0,0x0
 100:	ff010113          	addi	sp,sp,-16
 104:	13450513          	addi	a0,a0,308 # 134 <values>
 108:	07f00613          	li	a2,127
 10c:	00000593          	li	a1,0
 110:	00112623          	sw	ra,12(sp)
 114:	00000097          	auipc	ra,0x0
 118:	f78080e7          	jalr	-136(ra) # 8c <quicksort>
 11c:	000007b7          	lui	a5,0x0
 120:	13478513          	addi	a0,a5,308 # 134 <values>
 124:	00c12083          	lw	ra,12(sp)
 128:	00000513          	li	a0,0
 12c:	01010113          	addi	sp,sp,16
 130:	00008067          	ret
