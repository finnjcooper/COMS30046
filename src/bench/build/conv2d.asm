
G:/Finn/Stuff/Bristol/COMS30046/src/bench/build/conv2d.elf:     file format elf32-littleriscv


Disassembly of section .text:

00000000 <conv2d>:
   0:	ff010113          	addi	sp,sp,-16
   4:	00812623          	sw	s0,12(sp)
   8:	00912423          	sw	s1,8(sp)
   c:	03058293          	addi	t0,a1,48
  10:	00050493          	mv	s1,a0
  14:	00000413          	li	s0,0
  18:	00000513          	li	a0,0
  1c:	00400393          	li	t2,4
  20:	00241f13          	slli	t5,s0,0x2
  24:	01e48f33          	add	t5,s1,t5
  28:	00060e13          	mv	t3,a2
  2c:	00000f93          	li	t6,0
  30:	000e2023          	sw	zero,0(t3)
  34:	00c58893          	addi	a7,a1,12
  38:	000f0e93          	mv	t4,t5
  3c:	00000813          	li	a6,0
  40:	ff488793          	addi	a5,a7,-12
  44:	000e8693          	mv	a3,t4
  48:	0006a703          	lw	a4,0(a3)
  4c:	0007a303          	lw	t1,0(a5)
  50:	00478793          	addi	a5,a5,4
  54:	00468693          	addi	a3,a3,4
  58:	02670733          	mul	a4,a4,t1
  5c:	00e80833          	add	a6,a6,a4
  60:	010e2023          	sw	a6,0(t3)
  64:	ff1792e3          	bne	a5,a7,48 <conv2d+0x48>
  68:	00c78893          	addi	a7,a5,12
  6c:	018e8e93          	addi	t4,t4,24
  70:	fd1298e3          	bne	t0,a7,40 <conv2d+0x40>
  74:	001f8f93          	addi	t6,t6,1
  78:	004e0e13          	addi	t3,t3,4
  7c:	004f0f13          	addi	t5,t5,4
  80:	fa7f98e3          	bne	t6,t2,30 <conv2d+0x30>
  84:	00150513          	addi	a0,a0,1
  88:	01060613          	addi	a2,a2,16
  8c:	00640413          	addi	s0,s0,6
  90:	f9f518e3          	bne	a0,t6,20 <conv2d+0x20>
  94:	00c12403          	lw	s0,12(sp)
  98:	00812483          	lw	s1,8(sp)
  9c:	01010113          	addi	sp,sp,16
  a0:	00008067          	ret

000000a4 <main>:
  a4:	000005b7          	lui	a1,0x0
  a8:	fb010113          	addi	sp,sp,-80
  ac:	0d458593          	addi	a1,a1,212 # d4 <kernel.0>
  b0:	02458513          	addi	a0,a1,36
  b4:	00010613          	mv	a2,sp
  b8:	04112623          	sw	ra,76(sp)
  bc:	00000097          	auipc	ra,0x0
  c0:	f44080e7          	jalr	-188(ra) # 0 <conv2d>
  c4:	04c12083          	lw	ra,76(sp)
  c8:	00000513          	li	a0,0
  cc:	05010113          	addi	sp,sp,80
  d0:	00008067          	ret
