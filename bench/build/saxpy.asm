
build/saxpy.elf:     file format elf32-littleriscv


Disassembly of section .text:

00000000 <saxpy>:
   0:	02c05463          	blez	a2,28 <saxpy+0x28>
   4:	00261613          	slli	a2,a2,0x2
   8:	00c58633          	add	a2,a1,a2
   c:	00052787          	flw	fa5,0(a0)
  10:	0005a707          	flw	fa4,0(a1)
  14:	00458593          	addi	a1,a1,4
  18:	00450513          	addi	a0,a0,4
  1c:	70a7f7c3          	fmadd.s	fa5,fa5,fa0,fa4
  20:	fef5ae27          	fsw	fa5,-4(a1)
  24:	fec594e3          	bne	a1,a2,c <saxpy+0xc>
  28:	00008067          	ret

0000002c <main>:
  2c:	000007b7          	lui	a5,0x0
  30:	ff010113          	addi	sp,sp,-16
  34:	0747a507          	flw	fa0,116(a5) # 74 <main+0x48>
  38:	00812423          	sw	s0,8(sp)
  3c:	00001537          	lui	a0,0x1
  40:	00000437          	lui	s0,0x0
  44:	07840593          	addi	a1,s0,120 # 78 <y>
  48:	07850513          	addi	a0,a0,120 # 1078 <x>
  4c:	40000613          	li	a2,1024
  50:	00112623          	sw	ra,12(sp)
  54:	07840413          	addi	s0,s0,120
  58:	00000097          	auipc	ra,0x0
  5c:	fa8080e7          	jalr	-88(ra) # 0 <saxpy>
  60:	00c12083          	lw	ra,12(sp)
  64:	00812403          	lw	s0,8(sp)
  68:	00000513          	li	a0,0
  6c:	01010113          	addi	sp,sp,16
  70:	00008067          	ret
