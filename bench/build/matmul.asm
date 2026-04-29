
build/matmul.elf:     file format elf32-littleriscv


Disassembly of section .text:

00000000 <matmul>:
   0:	00050313          	mv	t1,a0
   4:	00060e93          	mv	t4,a2
   8:	40050f13          	addi	t5,a0,1024
   c:	44058e13          	addi	t3,a1,1088
  10:	40058893          	addi	a7,a1,1024
  14:	000e8813          	mv	a6,t4
  18:	00082023          	sw	zero,0(a6)
  1c:	c0088793          	addi	a5,a7,-1024
  20:	00030613          	mv	a2,t1
  24:	00000693          	li	a3,0
  28:	00062703          	lw	a4,0(a2)
  2c:	0007a503          	lw	a0,0(a5)
  30:	04078793          	addi	a5,a5,64
  34:	00460613          	addi	a2,a2,4
  38:	02a70733          	mul	a4,a4,a0
  3c:	00e686b3          	add	a3,a3,a4
  40:	00d82023          	sw	a3,0(a6)
  44:	ff1792e3          	bne	a5,a7,28 <matmul+0x28>
  48:	00478893          	addi	a7,a5,4
  4c:	00480813          	addi	a6,a6,4
  50:	fd1e14e3          	bne	t3,a7,18 <matmul+0x18>
  54:	04030313          	addi	t1,t1,64
  58:	040e8e93          	addi	t4,t4,64
  5c:	fbe31ae3          	bne	t1,t5,10 <matmul+0x10>
  60:	00008067          	ret

00000064 <main>:
  64:	000005b7          	lui	a1,0x0
  68:	0a058593          	addi	a1,a1,160 # a0 <b>
  6c:	00001637          	lui	a2,0x1
  70:	ff010113          	addi	sp,sp,-16
  74:	8a060613          	addi	a2,a2,-1888 # 8a0 <c>
  78:	40058513          	addi	a0,a1,1024
  7c:	00112623          	sw	ra,12(sp)
  80:	00000097          	auipc	ra,0x0
  84:	f80080e7          	jalr	-128(ra) # 0 <matmul>
  88:	000017b7          	lui	a5,0x1
  8c:	8a078613          	addi	a2,a5,-1888 # 8a0 <c>
  90:	00c12083          	lw	ra,12(sp)
  94:	00000513          	li	a0,0
  98:	01010113          	addi	sp,sp,16
  9c:	00008067          	ret
