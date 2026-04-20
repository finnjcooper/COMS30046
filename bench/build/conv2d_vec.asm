
G:/Finn/Stuff/Bristol/COMS30046/src/../bench/build/conv2d_vec.elf:     file format elf32-littleriscv


Disassembly of section .text:

00000000 <_start>:
   0:	00000097          	auipc	ra,0x0
   4:	0c0080e7          	jalr	192(ra) # c0 <main>
   8:	00000073          	ecall
   c:	0000006f          	j	c <_start+0xc>

00000010 <conv2d_vec>:
  10:	01850893          	addi	a7,a0,24
  14:	00c58e13          	addi	t3,a1,12
  18:	01858313          	addi	t1,a1,24
  1c:	00000e93          	li	t4,0
  20:	03000f13          	li	t5,48
  24:	008e8e93          	addi	t4,t4,8
  28:	fe888793          	addi	a5,a7,-24
  2c:	00060693          	mv	a3,a2
  30:	cd01f057          	vsetivli	zero,3,e32,m1,ta,ma
  34:	0207e087          	vle32.v	v1,(a5)
  38:	0205e107          	vle32.v	v2,(a1)
  3c:	5e003057          	vmv.v.i	v0,0
  40:	5e0031d7          	vmv.v.i	v3,0
  44:	b620a1d7          	vmacc.vv	v3,v1,v2
  48:	023021d7          	vredsum.vs	v3,v3,v0
  4c:	42302757          	vmv.x.s	a4,v3
  50:	02078813          	addi	a6,a5,32
  54:	cd01f057          	vsetivli	zero,3,e32,m1,ta,ma
  58:	02086087          	vle32.v	v1,(a6)
  5c:	020e6107          	vle32.v	v2,(t3)
  60:	5e003057          	vmv.v.i	v0,0
  64:	5e0031d7          	vmv.v.i	v3,0
  68:	b620a1d7          	vmacc.vv	v3,v1,v2
  6c:	023021d7          	vredsum.vs	v3,v3,v0
  70:	42302557          	vmv.x.s	a0,v3
  74:	00a70733          	add	a4,a4,a0
  78:	04078813          	addi	a6,a5,64
  7c:	cd01f057          	vsetivli	zero,3,e32,m1,ta,ma
  80:	02086087          	vle32.v	v1,(a6)
  84:	02036107          	vle32.v	v2,(t1)
  88:	5e003057          	vmv.v.i	v0,0
  8c:	5e0031d7          	vmv.v.i	v3,0
  90:	b620a1d7          	vmacc.vv	v3,v1,v2
  94:	023021d7          	vredsum.vs	v3,v3,v0
  98:	42302557          	vmv.x.s	a0,v3
  9c:	00a70733          	add	a4,a4,a0
  a0:	00e6a023          	sw	a4,0(a3)
  a4:	00478793          	addi	a5,a5,4
  a8:	00468693          	addi	a3,a3,4
  ac:	f8f892e3          	bne	a7,a5,30 <conv2d_vec+0x20>
  b0:	01860613          	addi	a2,a2,24
  b4:	02088893          	addi	a7,a7,32
  b8:	f7ee96e3          	bne	t4,t5,24 <conv2d_vec+0x14>
  bc:	00008067          	ret

000000c0 <main>:
  c0:	000005b7          	lui	a1,0x0
  c4:	0fc58593          	addi	a1,a1,252 # fc <kernel>
  c8:	00000637          	lui	a2,0x0
  cc:	ff010113          	addi	sp,sp,-16
  d0:	22060613          	addi	a2,a2,544 # 220 <output>
  d4:	02458513          	addi	a0,a1,36
  d8:	00112623          	sw	ra,12(sp)
  dc:	00000097          	auipc	ra,0x0
  e0:	f34080e7          	jalr	-204(ra) # 10 <conv2d_vec>
  e4:	000007b7          	lui	a5,0x0
  e8:	22078613          	addi	a2,a5,544 # 220 <output>
  ec:	00c12083          	lw	ra,12(sp)
  f0:	00000513          	li	a0,0
  f4:	01010113          	addi	sp,sp,16
  f8:	00008067          	ret
