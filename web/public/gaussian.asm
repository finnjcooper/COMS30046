
build/gaussian.elf:     file format elf32-littleriscv


Disassembly of section .text:

00000000 <gaussian_blur>:
   0:	00000e37          	lui	t3,0x0
   4:	00050293          	mv	t0,a0
   8:	00058f93          	mv	t6,a1
   c:	0e8e0e13          	addi	t3,t3,232 # e8 <input>
  10:	00000393          	li	t2,0
  14:	00000f37          	lui	t5,0x0
  18:	00c00513          	li	a0,12
  1c:	01000e93          	li	t4,16
  20:	00028313          	mv	t1,t0
  24:	000f8893          	mv	a7,t6
  28:	00000813          	li	a6,0
  2c:	f00007d3          	fmv.w.x	fa5,zero
  30:	0c4f0713          	addi	a4,t5,196 # c4 <kernel>
  34:	00030593          	mv	a1,t1
  38:	00000793          	li	a5,0
  3c:	00f58633          	add	a2,a1,a5
  40:	00f706b3          	add	a3,a4,a5
  44:	00062687          	flw	fa3,0(a2)
  48:	0006a707          	flw	fa4,0(a3)
  4c:	00478793          	addi	a5,a5,4
  50:	78e6f7c3          	fmadd.s	fa5,fa3,fa4,fa5
  54:	fea794e3          	bne	a5,a0,3c <gaussian_blur+0x3c>
  58:	00c70713          	addi	a4,a4,12
  5c:	04858593          	addi	a1,a1,72
  60:	fdc71ce3          	bne	a4,t3,38 <gaussian_blur+0x38>
  64:	00f8a027          	fsw	fa5,0(a7)
  68:	00180813          	addi	a6,a6,1
  6c:	00488893          	addi	a7,a7,4
  70:	00430313          	addi	t1,t1,4
  74:	fbd81ce3          	bne	a6,t4,2c <gaussian_blur+0x2c>
  78:	00138393          	addi	t2,t2,1
  7c:	040f8f93          	addi	t6,t6,64
  80:	04828293          	addi	t0,t0,72
  84:	f9039ee3          	bne	t2,a6,20 <gaussian_blur+0x20>
  88:	00008067          	ret

0000008c <main>:
  8c:	000005b7          	lui	a1,0x0
  90:	00000537          	lui	a0,0x0
  94:	ff010113          	addi	sp,sp,-16
  98:	5f858593          	addi	a1,a1,1528 # 5f8 <output>
  9c:	0e850513          	addi	a0,a0,232 # e8 <input>
  a0:	00112623          	sw	ra,12(sp)
  a4:	00000097          	auipc	ra,0x0
  a8:	f5c080e7          	jalr	-164(ra) # 0 <gaussian_blur>
  ac:	000007b7          	lui	a5,0x0
  b0:	5f878593          	addi	a1,a5,1528 # 5f8 <output>
  b4:	00c12083          	lw	ra,12(sp)
  b8:	00000513          	li	a0,0
  bc:	01010113          	addi	sp,sp,16
  c0:	00008067          	ret
