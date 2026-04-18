
G:/Finn/Stuff/Bristol/COMS30046/src/bench/build/saxpy.elf:     file format elf32-littleriscv


Disassembly of section .text:

00000000 <saxpy>:
   0:	f00506d3          	fmv.w.x	fa3,a0
   4:	02d05463          	blez	a3,2c <saxpy+0x2c>
   8:	00269693          	slli	a3,a3,0x2
   c:	00d606b3          	add	a3,a2,a3
  10:	0005a787          	flw	fa5,0(a1)
  14:	00062707          	flw	fa4,0(a2)
  18:	00460613          	addi	a2,a2,4
  1c:	00458593          	addi	a1,a1,4
  20:	70d7f7c3          	fmadd.s	fa5,fa5,fa3,fa4
  24:	fef62e27          	fsw	fa5,-4(a2)
  28:	fed614e3          	bne	a2,a3,10 <saxpy+0x10>
  2c:	00008067          	ret

00000030 <main>:
  30:	00000513          	li	a0,0
  34:	00008067          	ret
