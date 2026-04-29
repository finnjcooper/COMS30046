
build/matmul.elf:     file format elf32-littleriscv


Disassembly of section .text:

00000000 <matmul>:
   0:	00050313          	mv	t1,a0
   4:	00060e93          	mv	t4,a2
   8:	40050f13          	addi	t5,a0,1024
   c:	44058e13          	addi	t3,a1,1088
  10:	40058813          	addi	a6,a1,1024
  14:	000e8893          	mv	a7,t4
  18:	c0080793          	addi	a5,a6,-1024
  1c:	00030693          	mv	a3,t1
  20:	00000613          	li	a2,0
  24:	0006a703          	lw	a4,0(a3)
  28:	0007a503          	lw	a0,0(a5)
  2c:	04078793          	addi	a5,a5,64
  30:	00468693          	addi	a3,a3,4
  34:	02a70733          	mul	a4,a4,a0
  38:	00e60633          	add	a2,a2,a4
  3c:	ff0794e3          	bne	a5,a6,24 <matmul+0x24>
  40:	00c8a023          	sw	a2,0(a7)
  44:	00478813          	addi	a6,a5,4
  48:	00488893          	addi	a7,a7,4
  4c:	fd0e16e3          	bne	t3,a6,18 <matmul+0x18>
  50:	04030313          	addi	t1,t1,64
  54:	040e8e93          	addi	t4,t4,64
  58:	fbe31ce3          	bne	t1,t5,10 <matmul+0x10>
  5c:	00008067          	ret

00000060 <main>:
  60:	000005b7          	lui	a1,0x0
  64:	09c58593          	addi	a1,a1,156 # 9c <b>
  68:	00001637          	lui	a2,0x1
  6c:	ff010113          	addi	sp,sp,-16
  70:	89c60613          	addi	a2,a2,-1892 # 89c <c>
  74:	40058513          	addi	a0,a1,1024
  78:	00112623          	sw	ra,12(sp)
  7c:	00000097          	auipc	ra,0x0
  80:	f84080e7          	jalr	-124(ra) # 0 <matmul>
  84:	000017b7          	lui	a5,0x1
  88:	89c78613          	addi	a2,a5,-1892 # 89c <c>
  8c:	00c12083          	lw	ra,12(sp)
  90:	00000513          	li	a0,0
  94:	01010113          	addi	sp,sp,16
  98:	00008067          	ret
