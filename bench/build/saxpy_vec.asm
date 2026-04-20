
G:/Finn/Stuff/Bristol/COMS30046/src/../bench/build/saxpy_vec.elf:     file format elf32-littleriscv


Disassembly of section .text:

00000000 <_start>:
   0:	00000097          	auipc	ra,0x0
   4:	044080e7          	jalr	68(ra) # 44 <main>
   8:	00000073          	ecall
   c:	0000006f          	j	c <_start+0xc>

00000010 <saxpy>:
  10:	02c05863          	blez	a2,40 <saxpy+0x30>
  14:	0d0677d7          	vsetvli	a5,a2,e32,m1,ta,ma
  18:	02056187          	vle32.v	v3,(a0)
  1c:	0205e087          	vle32.v	v1,(a1)
  20:	5e055157          	vfmv.v.f	v2,fa0
  24:	00279713          	slli	a4,a5,0x2
  28:	40f60633          	sub	a2,a2,a5
  2c:	00e50533          	add	a0,a0,a4
  30:	b22190d7          	vfmacc.vv	v1,v3,v2
  34:	0205e0a7          	vse32.v	v1,(a1)
  38:	00e585b3          	add	a1,a1,a4
  3c:	fcc04ce3          	bgtz	a2,14 <saxpy+0x4>
  40:	00008067          	ret

00000044 <main>:
  44:	000007b7          	lui	a5,0x0
  48:	0847a507          	flw	fa0,132(a5) # 84 <main+0x40>
  4c:	000005b7          	lui	a1,0x0
  50:	08858593          	addi	a1,a1,136 # 88 <y>
  54:	ff010113          	addi	sp,sp,-16
  58:	10058513          	addi	a0,a1,256
  5c:	04000613          	li	a2,64
  60:	00112623          	sw	ra,12(sp)
  64:	00000097          	auipc	ra,0x0
  68:	fac080e7          	jalr	-84(ra) # 10 <saxpy>
  6c:	000007b7          	lui	a5,0x0
  70:	08878593          	addi	a1,a5,136 # 88 <y>
  74:	00c12083          	lw	ra,12(sp)
  78:	00000513          	li	a0,0
  7c:	01010113          	addi	sp,sp,16
  80:	00008067          	ret
