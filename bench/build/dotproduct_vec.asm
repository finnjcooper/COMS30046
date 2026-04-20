
G:/Finn/Stuff/Bristol/COMS30046/src/../bench/build/dotproduct_vec.elf:     file format elf32-littleriscv


Disassembly of section .text:

00000000 <_start>:
   0:	00000097          	auipc	ra,0x0
   4:	05c080e7          	jalr	92(ra) # 5c <main>
   8:	00000073          	ecall
   c:	0000006f          	j	c <_start+0xc>

00000010 <dot_product_vec>:
  10:	00050713          	mv	a4,a0
  14:	00000513          	li	a0,0
  18:	04c05063          	blez	a2,58 <dot_product_vec+0x48>
  1c:	0d0677d7          	vsetvli	a5,a2,e32,m1,ta,ma
  20:	02076087          	vle32.v	v1,(a4)
  24:	0205e107          	vle32.v	v2,(a1)
  28:	5e003057          	vmv.v.i	v0,0
  2c:	5e0031d7          	vmv.v.i	v3,0
  30:	b620a1d7          	vmacc.vv	v3,v1,v2
  34:	023021d7          	vredsum.vs	v3,v3,v0
  38:	423026d7          	vmv.x.s	a3,v3
  3c:	40f60633          	sub	a2,a2,a5
  40:	00279793          	slli	a5,a5,0x2
  44:	00f70733          	add	a4,a4,a5
  48:	00f585b3          	add	a1,a1,a5
  4c:	00d50533          	add	a0,a0,a3
  50:	fcc046e3          	bgtz	a2,1c <dot_product_vec+0xc>
  54:	00008067          	ret
  58:	00008067          	ret

0000005c <main>:
  5c:	000005b7          	lui	a1,0x0
  60:	09458593          	addi	a1,a1,148 # 94 <b>
  64:	ff010113          	addi	sp,sp,-16
  68:	10058513          	addi	a0,a1,256
  6c:	04000613          	li	a2,64
  70:	00112623          	sw	ra,12(sp)
  74:	00000097          	auipc	ra,0x0
  78:	f9c080e7          	jalr	-100(ra) # 10 <dot_product_vec>
  7c:	000007b7          	lui	a5,0x0
  80:	28a7aa23          	sw	a0,660(a5) # 294 <result>
  84:	00c12083          	lw	ra,12(sp)
  88:	00000513          	li	a0,0
  8c:	01010113          	addi	sp,sp,16
  90:	00008067          	ret
