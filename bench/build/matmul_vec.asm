
G:/Finn/Stuff/Bristol/COMS30046/src/../bench/build/matmul_vec.elf:     file format elf32-littleriscv


Disassembly of section .text:

00000000 <_start>:
   0:	00000097          	auipc	ra,0x0
   4:	0ac080e7          	jalr	172(ra) # ac <main>
   8:	00000073          	ecall
   c:	0000006f          	j	c <_start+0xc>

00000010 <matmul_vec>:
  10:	08d05c63          	blez	a3,a8 <matmul_vec+0x98>
  14:	00050f13          	mv	t5,a0
  18:	00060293          	mv	t0,a2
  1c:	00269f93          	slli	t6,a3,0x2
  20:	00000393          	li	t2,0
  24:	00058e13          	mv	t3,a1
  28:	00028313          	mv	t1,t0
  2c:	00000e93          	li	t4,0
  30:	000e0513          	mv	a0,t3
  34:	00068713          	mv	a4,a3
  38:	000f0613          	mv	a2,t5
  3c:	00000813          	li	a6,0
  40:	0d0777d7          	vsetvli	a5,a4,e32,m1,ta,ma
  44:	02066087          	vle32.v	v1,(a2)
  48:	02056107          	vle32.v	v2,(a0)
  4c:	5e003057          	vmv.v.i	v0,0
  50:	5e0031d7          	vmv.v.i	v3,0
  54:	b620a1d7          	vmacc.vv	v3,v1,v2
  58:	023021d7          	vredsum.vs	v3,v3,v0
  5c:	423028d7          	vmv.x.s	a7,v3
  60:	40f70733          	sub	a4,a4,a5
  64:	00279793          	slli	a5,a5,0x2
  68:	00f60633          	add	a2,a2,a5
  6c:	00f50533          	add	a0,a0,a5
  70:	01180833          	add	a6,a6,a7
  74:	fce046e3          	bgtz	a4,40 <matmul_vec+0x30>
  78:	01032023          	sw	a6,0(t1)
  7c:	001e8793          	addi	a5,t4,1
  80:	00430313          	addi	t1,t1,4
  84:	01fe0e33          	add	t3,t3,t6
  88:	00f68663          	beq	a3,a5,94 <matmul_vec+0x84>
  8c:	00078e93          	mv	t4,a5
  90:	fa1ff06f          	j	30 <matmul_vec+0x20>
  94:	01ff0f33          	add	t5,t5,t6
  98:	01f282b3          	add	t0,t0,t6
  9c:	01d38663          	beq	t2,t4,a8 <matmul_vec+0x98>
  a0:	00138393          	addi	t2,t2,1
  a4:	f81ff06f          	j	24 <matmul_vec+0x14>
  a8:	00008067          	ret

000000ac <main>:
  ac:	000005b7          	lui	a1,0x0
  b0:	0ec58593          	addi	a1,a1,236 # ec <bt>
  b4:	00000637          	lui	a2,0x0
  b8:	ff010113          	addi	sp,sp,-16
  bc:	2ec60613          	addi	a2,a2,748 # 2ec <c>
  c0:	10058513          	addi	a0,a1,256
  c4:	00800693          	li	a3,8
  c8:	00112623          	sw	ra,12(sp)
  cc:	00000097          	auipc	ra,0x0
  d0:	f44080e7          	jalr	-188(ra) # 10 <matmul_vec>
  d4:	000007b7          	lui	a5,0x0
  d8:	2ec78613          	addi	a2,a5,748 # 2ec <c>
  dc:	00c12083          	lw	ra,12(sp)
  e0:	00000513          	li	a0,0
  e4:	01010113          	addi	sp,sp,16
  e8:	00008067          	ret
