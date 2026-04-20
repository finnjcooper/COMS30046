
G:/Finn/Stuff/Bristol/COMS30046/src/../bench/build/saxpy_vec.elf:     file format elf32-littleriscv


Disassembly of section .text:

00000000 <_start>:
   0:	00000097          	auipc	ra,0x0
   4:	040080e7          	jalr	64(ra) # 40 <main>
   8:	00000073          	ecall
   c:	0000006f          	j	c <_start+0xc>

00000010 <saxpy>:
  10:	02c05663          	blez	a2,3c <saxpy+0x2c>
  14:	0d0677d7          	vsetvli	a5,a2,e32,m1,ta,ma
  18:	02056107          	vle32.v	v2,(a0)
  1c:	0205e087          	vle32.v	v1,(a1)
  20:	00279713          	slli	a4,a5,0x2
  24:	40f60633          	sub	a2,a2,a5
  28:	00e50533          	add	a0,a0,a4
  2c:	b22550d7          	vfmacc.vf	v1,fa0,v2
  30:	0205e0a7          	vse32.v	v1,(a1)
  34:	00e585b3          	add	a1,a1,a4
  38:	fcc04ee3          	bgtz	a2,14 <saxpy+0x4>
  3c:	00008067          	ret

00000040 <main>:
  40:	00000e37          	lui	t3,0x0
  44:	cd027057          	vsetivli	zero,4,e32,m1,ta,ma
  48:	250e0e13          	addi	t3,t3,592 # 250 <main+0x210>
  4c:	00000337          	lui	t1,0x0
  50:	020e6607          	vle32.v	v12,(t3)
  54:	26030313          	addi	t1,t1,608 # 260 <main+0x220>
  58:	000008b7          	lui	a7,0x0
  5c:	02036587          	vle32.v	v11,(t1)
  60:	27088893          	addi	a7,a7,624 # 270 <main+0x230>
  64:	00000837          	lui	a6,0x0
  68:	0208e507          	vle32.v	v10,(a7)
  6c:	28080813          	addi	a6,a6,640 # 280 <main+0x240>
  70:	00000537          	lui	a0,0x0
  74:	02086487          	vle32.v	v9,(a6)
  78:	29050513          	addi	a0,a0,656 # 290 <main+0x250>
  7c:	000005b7          	lui	a1,0x0
  80:	000007b7          	lui	a5,0x0
  84:	df010113          	addi	sp,sp,-528
  88:	02056407          	vle32.v	v8,(a0)
  8c:	2a058593          	addi	a1,a1,672 # 2a0 <main+0x260>
  90:	2e078793          	addi	a5,a5,736 # 2e0 <main+0x2a0>
  94:	00000637          	lui	a2,0x0
  98:	20112623          	sw	ra,524(sp)
  9c:	0205e387          	vle32.v	v7,(a1)
  a0:	0207e107          	vle32.v	v2,(a5)
  a4:	2b060613          	addi	a2,a2,688 # 2b0 <main+0x270>
  a8:	01010793          	addi	a5,sp,16
  ac:	000006b7          	lui	a3,0x0
  b0:	0207e627          	vse32.v	v12,(a5)
  b4:	02066307          	vle32.v	v6,(a2)
  b8:	02010793          	addi	a5,sp,32
  bc:	2c068693          	addi	a3,a3,704 # 2c0 <main+0x280>
  c0:	00000737          	lui	a4,0x0
  c4:	0207e5a7          	vse32.v	v11,(a5)
  c8:	0206e287          	vle32.v	v5,(a3)
  cc:	03010793          	addi	a5,sp,48
  d0:	2d070713          	addi	a4,a4,720 # 2d0 <main+0x290>
  d4:	0207e527          	vse32.v	v10,(a5)
  d8:	02076207          	vle32.v	v4,(a4)
  dc:	00000f37          	lui	t5,0x0
  e0:	04010793          	addi	a5,sp,64
  e4:	0207e4a7          	vse32.v	v9,(a5)
  e8:	244f2787          	flw	fa5,580(t5) # 244 <main+0x204>
  ec:	05010793          	addi	a5,sp,80
  f0:	0207e427          	vse32.v	v8,(a5)
  f4:	06010793          	addi	a5,sp,96
  f8:	0207e3a7          	vse32.v	v7,(a5)
  fc:	07010793          	addi	a5,sp,112
 100:	5e07d0d7          	vfmv.v.f	v1,fa5
 104:	0207e327          	vse32.v	v6,(a5)
 108:	08010793          	addi	a5,sp,128
 10c:	0207e2a7          	vse32.v	v5,(a5)
 110:	09010793          	addi	a5,sp,144
 114:	0207e227          	vse32.v	v4,(a5)
 118:	11010793          	addi	a5,sp,272
 11c:	0207e0a7          	vse32.v	v1,(a5)
 120:	12010793          	addi	a5,sp,288
 124:	0207e0a7          	vse32.v	v1,(a5)
 128:	00000eb7          	lui	t4,0x0
 12c:	13010793          	addi	a5,sp,304
 130:	0207e0a7          	vse32.v	v1,(a5)
 134:	240e8e93          	addi	t4,t4,576 # 240 <main+0x200>
 138:	14010793          	addi	a5,sp,320
 13c:	0207e0a7          	vse32.v	v1,(a5)
 140:	020ee187          	vle32.v	v3,(t4)
 144:	15010793          	addi	a5,sp,336
 148:	0207e0a7          	vse32.v	v1,(a5)
 14c:	16010793          	addi	a5,sp,352
 150:	0207e0a7          	vse32.v	v1,(a5)
 154:	17010793          	addi	a5,sp,368
 158:	0207e0a7          	vse32.v	v1,(a5)
 15c:	18010793          	addi	a5,sp,384
 160:	0207e0a7          	vse32.v	v1,(a5)
 164:	19010793          	addi	a5,sp,400
 168:	0207e0a7          	vse32.v	v1,(a5)
 16c:	10010813          	addi	a6,sp,256
 170:	000005b7          	lui	a1,0x0
 174:	00000637          	lui	a2,0x0
 178:	000006b7          	lui	a3,0x0
 17c:	00000737          	lui	a4,0x0
 180:	000007b7          	lui	a5,0x0
 184:	020161a7          	vse32.v	v3,(sp)
 188:	2f058593          	addi	a1,a1,752 # 2f0 <main+0x2b0>
 18c:	020860a7          	vse32.v	v1,(a6)
 190:	30060613          	addi	a2,a2,768 # 300 <main+0x2c0>
 194:	0a010813          	addi	a6,sp,160
 198:	31068693          	addi	a3,a3,784 # 310 <main+0x2d0>
 19c:	32070713          	addi	a4,a4,800 # 320 <main+0x2e0>
 1a0:	33078793          	addi	a5,a5,816 # 330 <main+0x2f0>
 1a4:	02086127          	vse32.v	v2,(a6)
 1a8:	0205e307          	vle32.v	v6,(a1)
 1ac:	02066287          	vle32.v	v5,(a2)
 1b0:	0206e207          	vle32.v	v4,(a3)
 1b4:	02076187          	vle32.v	v3,(a4)
 1b8:	0207e107          	vle32.v	v2,(a5)
 1bc:	1a010313          	addi	t1,sp,416
 1c0:	020360a7          	vse32.v	v1,(t1)
 1c4:	1b010313          	addi	t1,sp,432
 1c8:	020360a7          	vse32.v	v1,(t1)
 1cc:	000007b7          	lui	a5,0x0
 1d0:	1c010313          	addi	t1,sp,448
 1d4:	020360a7          	vse32.v	v1,(t1)
 1d8:	2487a507          	flw	fa0,584(a5) # 248 <main+0x208>
 1dc:	1d010313          	addi	t1,sp,464
 1e0:	020360a7          	vse32.v	v1,(t1)
 1e4:	1e010313          	addi	t1,sp,480
 1e8:	020360a7          	vse32.v	v1,(t1)
 1ec:	0b010893          	addi	a7,sp,176
 1f0:	0c010813          	addi	a6,sp,192
 1f4:	0d010693          	addi	a3,sp,208
 1f8:	0e010713          	addi	a4,sp,224
 1fc:	0f010793          	addi	a5,sp,240
 200:	1f010313          	addi	t1,sp,496
 204:	00010513          	mv	a0,sp
 208:	10010593          	addi	a1,sp,256
 20c:	04000613          	li	a2,64
 210:	020360a7          	vse32.v	v1,(t1)
 214:	0208e327          	vse32.v	v6,(a7)
 218:	020862a7          	vse32.v	v5,(a6)
 21c:	0206e227          	vse32.v	v4,(a3)
 220:	020761a7          	vse32.v	v3,(a4)
 224:	0207e127          	vse32.v	v2,(a5)
 228:	00000097          	auipc	ra,0x0
 22c:	de8080e7          	jalr	-536(ra) # 10 <saxpy>
 230:	20c12083          	lw	ra,524(sp)
 234:	00000513          	li	a0,0
 238:	21010113          	addi	sp,sp,528
 23c:	00008067          	ret
