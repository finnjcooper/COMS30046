
G:/Finn/Stuff/Bristol/COMS30046/src/../bench/build/vectoradd.elf:     file format elf32-littleriscv


Disassembly of section .text:

00000000 <_start>:
   0:	00000097          	auipc	ra,0x0
   4:	040080e7          	jalr	64(ra) # 40 <main>
   8:	00000073          	ecall
   c:	0000006f          	j	c <_start+0xc>

00000010 <vector_add>:
  10:	02d05663          	blez	a3,3c <vector_add+0x2c>
  14:	00269693          	slli	a3,a3,0x2
  18:	00d506b3          	add	a3,a0,a3
  1c:	00052783          	lw	a5,0(a0)
  20:	0005a703          	lw	a4,0(a1)
  24:	00450513          	addi	a0,a0,4
  28:	00458593          	addi	a1,a1,4
  2c:	00e787b3          	add	a5,a5,a4
  30:	00f62023          	sw	a5,0(a2)
  34:	00460613          	addi	a2,a2,4
  38:	fed512e3          	bne	a0,a3,1c <vector_add+0xc>
  3c:	00008067          	ret

00000040 <main>:
  40:	000007b7          	lui	a5,0x0
  44:	11878793          	addi	a5,a5,280 # 118 <main+0xd8>
  48:	0087a703          	lw	a4,8(a5)
  4c:	f7010113          	addi	sp,sp,-144
  50:	0047a683          	lw	a3,4(a5)
  54:	0007a603          	lw	a2,0(a5)
  58:	00e12823          	sw	a4,16(sp)
  5c:	0147a703          	lw	a4,20(a5)
  60:	00d12623          	sw	a3,12(sp)
  64:	0107a683          	lw	a3,16(a5)
  68:	00c12423          	sw	a2,8(sp)
  6c:	00e12e23          	sw	a4,28(sp)
  70:	00c7a603          	lw	a2,12(a5)
  74:	0207a703          	lw	a4,32(a5)
  78:	00d12c23          	sw	a3,24(sp)
  7c:	01c7a683          	lw	a3,28(a5)
  80:	00c12a23          	sw	a2,20(sp)
  84:	02e12423          	sw	a4,40(sp)
  88:	0187a603          	lw	a2,24(a5)
  8c:	02c7a703          	lw	a4,44(a5)
  90:	02d12223          	sw	a3,36(sp)
  94:	0287a683          	lw	a3,40(a5)
  98:	02c12023          	sw	a2,32(sp)
  9c:	02e12a23          	sw	a4,52(sp)
  a0:	0247a603          	lw	a2,36(a5)
  a4:	0307a703          	lw	a4,48(a5)
  a8:	02d12823          	sw	a3,48(sp)
  ac:	0347a683          	lw	a3,52(a5)
  b0:	08112623          	sw	ra,140(sp)
  b4:	0387ae03          	lw	t3,56(a5)
  b8:	03c7a303          	lw	t1,60(a5)
  bc:	0407a883          	lw	a7,64(a5)
  c0:	02c12623          	sw	a2,44(sp)
  c4:	02e12c23          	sw	a4,56(sp)
  c8:	0447a803          	lw	a6,68(a5)
  cc:	0487a703          	lw	a4,72(a5)
  d0:	04c7a783          	lw	a5,76(a5)
  d4:	02d12e23          	sw	a3,60(sp)
  d8:	05810613          	addi	a2,sp,88
  dc:	00a00693          	li	a3,10
  e0:	03010593          	addi	a1,sp,48
  e4:	00810513          	addi	a0,sp,8
  e8:	05c12023          	sw	t3,64(sp)
  ec:	04612223          	sw	t1,68(sp)
  f0:	05112423          	sw	a7,72(sp)
  f4:	05012623          	sw	a6,76(sp)
  f8:	04e12823          	sw	a4,80(sp)
  fc:	04f12a23          	sw	a5,84(sp)
 100:	00000097          	auipc	ra,0x0
 104:	f10080e7          	jalr	-240(ra) # 10 <vector_add>
 108:	08c12083          	lw	ra,140(sp)
 10c:	00000513          	li	a0,0
 110:	09010113          	addi	sp,sp,144
 114:	00008067          	ret
