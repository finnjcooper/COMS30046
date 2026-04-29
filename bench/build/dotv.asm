
build/dotv.elf:     file format elf32-littleriscv


Disassembly of section .text:

00000000 <dot_product>:
   0:	00050793          	mv	a5,a0
   4:	08c05263          	blez	a2,88 <dot_product+0x88>
   8:	0d067757          	vsetvli	a4,a2,e32,m1,ta,ma
   c:	5e003257          	vmv.v.i	v4,0
  10:	00271693          	slli	a3,a4,0x2
  14:	9e4030d7          	vmv1r.v	v1,v4
  18:	06e64c63          	blt	a2,a4,90 <dot_product+0x90>
  1c:	0207e187          	vle32.v	v3,(a5)
  20:	0205e107          	vle32.v	v2,(a1)
  24:	40e60633          	sub	a2,a2,a4
  28:	00d787b3          	add	a5,a5,a3
  2c:	00d585b3          	add	a1,a1,a3
  30:	b621a0d7          	vmacc.vv	v1,v3,v2
  34:	fee654e3          	bge	a2,a4,1c <dot_product+0x1c>
  38:	021220d7          	vredsum.vs	v1,v1,v4
  3c:	42102557          	vmv.x.s	a0,v1
  40:	04060663          	beqz	a2,8c <dot_product+0x8c>
  44:	0d007757          	vsetvli	a4,zero,e32,m1,ta,ma
  48:	5e0030d7          	vmv.v.i	v1,0
  4c:	09067757          	vsetvli	a4,a2,e32,m1,tu,ma
  50:	0207e187          	vle32.v	v3,(a5)
  54:	0205e107          	vle32.v	v2,(a1)
  58:	00271693          	slli	a3,a4,0x2
  5c:	40e60633          	sub	a2,a2,a4
  60:	00d787b3          	add	a5,a5,a3
  64:	00d585b3          	add	a1,a1,a3
  68:	b621a0d7          	vmacc.vv	v1,v3,v2
  6c:	fe0610e3          	bnez	a2,4c <dot_product+0x4c>
  70:	0d0077d7          	vsetvli	a5,zero,e32,m1,ta,ma
  74:	42006157          	vmv.s.x	v2,zero
  78:	021120d7          	vredsum.vs	v1,v1,v2
  7c:	421027d7          	vmv.x.s	a5,v1
  80:	00f50533          	add	a0,a0,a5
  84:	00008067          	ret
  88:	00000513          	li	a0,0
  8c:	00008067          	ret
  90:	02422257          	vredsum.vs	v4,v4,v4
  94:	42402557          	vmv.x.s	a0,v4
  98:	fadff06f          	j	44 <dot_product+0x44>

0000009c <main>:
  9c:	000005b7          	lui	a1,0x0
  a0:	00001537          	lui	a0,0x1
  a4:	ff010113          	addi	sp,sp,-16
  a8:	0d858593          	addi	a1,a1,216 # d8 <b>
  ac:	40000613          	li	a2,1024
  b0:	0d850513          	addi	a0,a0,216 # 10d8 <a>
  b4:	00112623          	sw	ra,12(sp)
  b8:	00000097          	auipc	ra,0x0
  bc:	f48080e7          	jalr	-184(ra) # 0 <dot_product>
  c0:	000027b7          	lui	a5,0x2
  c4:	0ca7ac23          	sw	a0,216(a5) # 20d8 <result>
  c8:	00c12083          	lw	ra,12(sp)
  cc:	00000513          	li	a0,0
  d0:	01010113          	addi	sp,sp,16
  d4:	00008067          	ret
