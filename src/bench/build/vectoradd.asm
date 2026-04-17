
G:/Finn/Stuff/Bristol/COMS30046/src/bench/build/vectoradd.elf:     file format elf32-littleriscv


Disassembly of section .text:

00000000 <main>:
   0:	000007b7          	lui	a5,0x0
   4:	0e878793          	addi	a5,a5,232 # e8 <main+0xe8>
   8:	0087a703          	lw	a4,8(a5)
   c:	f8010113          	addi	sp,sp,-128
  10:	0047a683          	lw	a3,4(a5)
  14:	0007a603          	lw	a2,0(a5)
  18:	00e12823          	sw	a4,16(sp)
  1c:	0147a703          	lw	a4,20(a5)
  20:	00d12623          	sw	a3,12(sp)
  24:	0107a683          	lw	a3,16(a5)
  28:	00c12423          	sw	a2,8(sp)
  2c:	00e12e23          	sw	a4,28(sp)
  30:	00c7a603          	lw	a2,12(a5)
  34:	0207a703          	lw	a4,32(a5)
  38:	00d12c23          	sw	a3,24(sp)
  3c:	01c7a683          	lw	a3,28(a5)
  40:	00c12a23          	sw	a2,20(sp)
  44:	02e12423          	sw	a4,40(sp)
  48:	0187a603          	lw	a2,24(a5)
  4c:	02c7a703          	lw	a4,44(a5)
  50:	02d12223          	sw	a3,36(sp)
  54:	0287a683          	lw	a3,40(a5)
  58:	02c12023          	sw	a2,32(sp)
  5c:	02e12a23          	sw	a4,52(sp)
  60:	0247a603          	lw	a2,36(a5)
  64:	0307a703          	lw	a4,48(a5)
  68:	02d12823          	sw	a3,48(sp)
  6c:	0347a683          	lw	a3,52(a5)
  70:	03c7a503          	lw	a0,60(a5)
  74:	0387a803          	lw	a6,56(a5)
  78:	0407a583          	lw	a1,64(a5)
  7c:	02c12623          	sw	a2,44(sp)
  80:	02e12c23          	sw	a4,56(sp)
  84:	0447a603          	lw	a2,68(a5)
  88:	0487a703          	lw	a4,72(a5)
  8c:	04c7a783          	lw	a5,76(a5)
  90:	02d12e23          	sw	a3,60(sp)
  94:	03010693          	addi	a3,sp,48
  98:	04a12223          	sw	a0,68(sp)
  9c:	04c12623          	sw	a2,76(sp)
  a0:	04f12a23          	sw	a5,84(sp)
  a4:	05012023          	sw	a6,64(sp)
  a8:	04b12423          	sw	a1,72(sp)
  ac:	04e12823          	sw	a4,80(sp)
  b0:	00068513          	mv	a0,a3
  b4:	00810793          	addi	a5,sp,8
  b8:	05810613          	addi	a2,sp,88
  bc:	0007a703          	lw	a4,0(a5)
  c0:	0006a583          	lw	a1,0(a3)
  c4:	00478793          	addi	a5,a5,4
  c8:	00468693          	addi	a3,a3,4
  cc:	00b70733          	add	a4,a4,a1
  d0:	00e62023          	sw	a4,0(a2)
  d4:	00460613          	addi	a2,a2,4
  d8:	fea792e3          	bne	a5,a0,bc <main+0xbc>
  dc:	07c12503          	lw	a0,124(sp)
  e0:	08010113          	addi	sp,sp,128
  e4:	00008067          	ret
