
build/dot.elf:     file format elf32-littleriscv


Disassembly of section .text:

00000000 <dot_product>:
   0:	02c05a63          	blez	a2,34 <dot_product+0x34>
   4:	00261613          	slli	a2,a2,0x2
   8:	00050793          	mv	a5,a0
   c:	00c50633          	add	a2,a0,a2
  10:	00000513          	li	a0,0
  14:	0007a703          	lw	a4,0(a5)
  18:	0005a683          	lw	a3,0(a1)
  1c:	00478793          	addi	a5,a5,4
  20:	00458593          	addi	a1,a1,4
  24:	02d70733          	mul	a4,a4,a3
  28:	00e50533          	add	a0,a0,a4
  2c:	fec794e3          	bne	a5,a2,14 <dot_product+0x14>
  30:	00008067          	ret
  34:	00000513          	li	a0,0
  38:	00008067          	ret

0000003c <main>:
  3c:	000005b7          	lui	a1,0x0
  40:	00001537          	lui	a0,0x1
  44:	ff010113          	addi	sp,sp,-16
  48:	07858593          	addi	a1,a1,120 # 78 <b>
  4c:	40000613          	li	a2,1024
  50:	07850513          	addi	a0,a0,120 # 1078 <a>
  54:	00112623          	sw	ra,12(sp)
  58:	00000097          	auipc	ra,0x0
  5c:	fa8080e7          	jalr	-88(ra) # 0 <dot_product>
  60:	000027b7          	lui	a5,0x2
  64:	06a7ac23          	sw	a0,120(a5) # 2078 <c>
  68:	00c12083          	lw	ra,12(sp)
  6c:	00000513          	li	a0,0
  70:	01010113          	addi	sp,sp,16
  74:	00008067          	ret
