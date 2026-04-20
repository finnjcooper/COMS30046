
G:/Finn/Stuff/Bristol/COMS30046/src/../bench/build/dotproduct.elf:     file format elf32-littleriscv


Disassembly of section .text:

00000000 <_start>:
   0:	00000097          	auipc	ra,0x0
   4:	04c080e7          	jalr	76(ra) # 4c <main>
   8:	00000073          	ecall
   c:	0000006f          	j	c <_start+0xc>

00000010 <dot_product>:
  10:	02c05a63          	blez	a2,44 <dot_product+0x34>
  14:	00261613          	slli	a2,a2,0x2
  18:	00050793          	mv	a5,a0
  1c:	00c50633          	add	a2,a0,a2
  20:	00000513          	li	a0,0
  24:	0007a703          	lw	a4,0(a5)
  28:	0005a683          	lw	a3,0(a1)
  2c:	00478793          	addi	a5,a5,4
  30:	00458593          	addi	a1,a1,4
  34:	02d70733          	mul	a4,a4,a3
  38:	00e50533          	add	a0,a0,a4
  3c:	fec794e3          	bne	a5,a2,24 <dot_product+0x14>
  40:	00008067          	ret
  44:	00000513          	li	a0,0
  48:	00008067          	ret

0000004c <main>:
  4c:	de010113          	addi	sp,sp,-544
  50:	20112e23          	sw	ra,540(sp)
  54:	01010713          	addi	a4,sp,16
  58:	11010793          	addi	a5,sp,272
  5c:	00000613          	li	a2,0
  60:	04100513          	li	a0,65
  64:	04000593          	li	a1,64
  68:	00160613          	addi	a2,a2,1
  6c:	40c506b3          	sub	a3,a0,a2
  70:	00c72023          	sw	a2,0(a4)
  74:	00d7a023          	sw	a3,0(a5)
  78:	00470713          	addi	a4,a4,4
  7c:	00478793          	addi	a5,a5,4
  80:	feb614e3          	bne	a2,a1,68 <main+0x1c>
  84:	11010593          	addi	a1,sp,272
  88:	01010513          	addi	a0,sp,16
  8c:	00000097          	auipc	ra,0x0
  90:	f84080e7          	jalr	-124(ra) # 10 <dot_product>
  94:	00a12623          	sw	a0,12(sp)
  98:	21c12083          	lw	ra,540(sp)
  9c:	00000513          	li	a0,0
  a0:	22010113          	addi	sp,sp,544
  a4:	00008067          	ret
