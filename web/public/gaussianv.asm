
build/gaussianv.elf:     file format elf32-littleriscv


Disassembly of section .text:

00000000 <gaussian_blur>:
   0:	ff010113          	addi	sp,sp,-16
   4:	00000e37          	lui	t3,0x0
   8:	00812623          	sw	s0,12(sp)
   c:	00058e93          	mv	t4,a1
  10:	104e0e13          	addi	t3,t3,260 # 104 <input>
  14:	00000f13          	li	t5,0
  18:	00000413          	li	s0,0
  1c:	000003b7          	lui	t2,0x0
  20:	01000f93          	li	t6,16
  24:	00c00813          	li	a6,12
  28:	00f00293          	li	t0,15
  2c:	00000893          	li	a7,0
  30:	411f8733          	sub	a4,t6,a7
  34:	0d077757          	vsetvli	a4,a4,e32,m1,ta,ma
  38:	01e886b3          	add	a3,a7,t5
  3c:	5e0030d7          	vmv.v.i	v1,0
  40:	00269693          	slli	a3,a3,0x2
  44:	00d506b3          	add	a3,a0,a3
  48:	0e038613          	addi	a2,t2,224 # e0 <kernel>
  4c:	00289313          	slli	t1,a7,0x2
  50:	00000793          	li	a5,0
  54:	00f685b3          	add	a1,a3,a5
  58:	0205e107          	vle32.v	v2,(a1)
  5c:	00f605b3          	add	a1,a2,a5
  60:	0005a787          	flw	fa5,0(a1)
  64:	00478793          	addi	a5,a5,4
  68:	b227d0d7          	vfmacc.vf	v1,fa5,v2
  6c:	ff0794e3          	bne	a5,a6,54 <gaussian_blur+0x54>
  70:	00c60613          	addi	a2,a2,12
  74:	04868693          	addi	a3,a3,72
  78:	fdc61ce3          	bne	a2,t3,50 <gaussian_blur+0x50>
  7c:	006e87b3          	add	a5,t4,t1
  80:	0207e0a7          	vse32.v	v1,(a5)
  84:	00e888b3          	add	a7,a7,a4
  88:	fb12d4e3          	bge	t0,a7,30 <gaussian_blur+0x30>
  8c:	00140413          	addi	s0,s0,1
  90:	040e8e93          	addi	t4,t4,64
  94:	012f0f13          	addi	t5,t5,18
  98:	f9f41ae3          	bne	s0,t6,2c <gaussian_blur+0x2c>
  9c:	00c12403          	lw	s0,12(sp)
  a0:	01010113          	addi	sp,sp,16
  a4:	00008067          	ret

000000a8 <main>:
  a8:	000005b7          	lui	a1,0x0
  ac:	00000537          	lui	a0,0x0
  b0:	ff010113          	addi	sp,sp,-16
  b4:	61458593          	addi	a1,a1,1556 # 614 <output>
  b8:	10450513          	addi	a0,a0,260 # 104 <input>
  bc:	00112623          	sw	ra,12(sp)
  c0:	00000097          	auipc	ra,0x0
  c4:	f40080e7          	jalr	-192(ra) # 0 <gaussian_blur>
  c8:	000007b7          	lui	a5,0x0
  cc:	61478593          	addi	a1,a5,1556 # 614 <output>
  d0:	00c12083          	lw	ra,12(sp)
  d4:	00000513          	li	a0,0
  d8:	01010113          	addi	sp,sp,16
  dc:	00008067          	ret
