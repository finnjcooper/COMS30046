
G:/Finn/Stuff/Bristol/COMS30046/src/../bench/build/dotproduct_vec.elf:     file format elf32-littleriscv


Disassembly of section .text:

00000000 <_start>:
   0:	00000097          	auipc	ra,0x0
   4:	058080e7          	jalr	88(ra) # 58 <main>
   8:	00000073          	ecall
   c:	0000006f          	j	c <_start+0xc>

00000010 <dot_product>:
  10:	00050713          	mv	a4,a0
  14:	00000513          	li	a0,0
  18:	02c05e63          	blez	a2,54 <dot_product+0x44>
  1c:	0d0677d7          	vsetvli	a5,a2,e32,m1,ta,ma
  20:	02076087          	vle32.v	v1,(a4)
  24:	0205e187          	vle32.v	v3,(a1)
  28:	5e003157          	vmv.v.i	v2,0
  2c:	00279693          	slli	a3,a5,0x2
  30:	40f60633          	sub	a2,a2,a5
  34:	00d70733          	add	a4,a4,a3
  38:	00d585b3          	add	a1,a1,a3
  3c:	9611a0d7          	vmul.vv	v1,v1,v3
  40:	021120d7          	vredsum.vs	v1,v1,v2
  44:	421027d7          	vmv.x.s	a5,v1
  48:	00f50533          	add	a0,a0,a5
  4c:	fcc048e3          	bgtz	a2,1c <dot_product+0xc>
  50:	00008067          	ret
  54:	00008067          	ret

00000058 <main>:
  58:	000005b7          	lui	a1,0x0
  5c:	09058593          	addi	a1,a1,144 # 90 <b>
  60:	ff010113          	addi	sp,sp,-16
  64:	10058513          	addi	a0,a1,256
  68:	04000613          	li	a2,64
  6c:	00112623          	sw	ra,12(sp)
  70:	00000097          	auipc	ra,0x0
  74:	fa0080e7          	jalr	-96(ra) # 10 <dot_product>
  78:	000007b7          	lui	a5,0x0
  7c:	28a7a823          	sw	a0,656(a5) # 290 <result>
  80:	00c12083          	lw	ra,12(sp)
  84:	00000513          	li	a0,0
  88:	01010113          	addi	sp,sp,16
  8c:	00008067          	ret
