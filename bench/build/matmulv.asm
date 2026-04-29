
build/matmulv.elf:     file format elf32-littleriscv


Disassembly of section .text:

00000000 <matmul>:
   0:	ff010113          	addi	sp,sp,-16
   4:	00812623          	sw	s0,12(sp)
   8:	40058393          	addi	t2,a1,1024
   c:	00050413          	mv	s0,a0
  10:	00000e93          	li	t4,0
  14:	01000293          	li	t0,16
  18:	00f00f93          	li	t6,15
  1c:	40000513          	li	a0,1024
  20:	00000e13          	li	t3,0
  24:	01d40f33          	add	t5,s0,t4
  28:	41c286b3          	sub	a3,t0,t3
  2c:	0d06f6d7          	vsetvli	a3,a3,e32,m1,ta,ma
  30:	5e0030d7          	vmv.v.i	v1,0
  34:	002e1313          	slli	t1,t3,0x2
  38:	006587b3          	add	a5,a1,t1
  3c:	006388b3          	add	a7,t2,t1
  40:	000f0713          	mv	a4,t5
  44:	0207e107          	vle32.v	v2,(a5)
  48:	00072803          	lw	a6,0(a4)
  4c:	04078793          	addi	a5,a5,64
  50:	00470713          	addi	a4,a4,4
  54:	b62860d7          	vmacc.vx	v1,a6,v2
  58:	fef896e3          	bne	a7,a5,44 <matmul+0x44>
  5c:	01d30333          	add	t1,t1,t4
  60:	00660333          	add	t1,a2,t1
  64:	020360a7          	vse32.v	v1,(t1)
  68:	00de0e33          	add	t3,t3,a3
  6c:	fbcfdee3          	bge	t6,t3,28 <matmul+0x28>
  70:	040e8e93          	addi	t4,t4,64
  74:	faae96e3          	bne	t4,a0,20 <matmul+0x20>
  78:	00c12403          	lw	s0,12(sp)
  7c:	01010113          	addi	sp,sp,16
  80:	00008067          	ret

00000084 <main>:
  84:	000005b7          	lui	a1,0x0
  88:	0c058593          	addi	a1,a1,192 # c0 <b>
  8c:	00001637          	lui	a2,0x1
  90:	ff010113          	addi	sp,sp,-16
  94:	8c060613          	addi	a2,a2,-1856 # 8c0 <c>
  98:	40058513          	addi	a0,a1,1024
  9c:	00112623          	sw	ra,12(sp)
  a0:	00000097          	auipc	ra,0x0
  a4:	f60080e7          	jalr	-160(ra) # 0 <matmul>
  a8:	000017b7          	lui	a5,0x1
  ac:	8c078613          	addi	a2,a5,-1856 # 8c0 <c>
  b0:	00c12083          	lw	ra,12(sp)
  b4:	00000513          	li	a0,0
  b8:	01010113          	addi	sp,sp,16
  bc:	00008067          	ret
