
G:\Finn\Stuff\Bristol\COMS30046\bench\build\saxpy.elf:     file format elf32-littleriscv


Disassembly of section .text:

00000000 <_start>:
   0:	00000097          	auipc	ra,0x0
   4:	040080e7          	jalr	64(ra) # 40 <main>
   8:	00000073          	ecall
   c:	0000006f          	j	c <_start+0xc>

00000010 <saxpy>:
  10:	f00506d3          	fmv.w.x	fa3,a0
  14:	02d05463          	blez	a3,3c <saxpy+0x2c>
  18:	00269693          	slli	a3,a3,0x2
  1c:	00d606b3          	add	a3,a2,a3
  20:	0005a787          	flw	fa5,0(a1)
  24:	00062707          	flw	fa4,0(a2)
  28:	00460613          	addi	a2,a2,4
  2c:	00458593          	addi	a1,a1,4
  30:	70d7f7c3          	fmadd.s	fa5,fa5,fa3,fa4
  34:	fef62e27          	fsw	fa5,-4(a2)
  38:	fed614e3          	bne	a2,a3,20 <saxpy+0x10>
  3c:	00008067          	ret

00000040 <main>:
  40:	df010113          	addi	sp,sp,-528
  44:	000007b7          	lui	a5,0x0
  48:	0a47a707          	flw	fa4,164(a5) # a4 <main+0x64>
  4c:	20112623          	sw	ra,524(sp)
  50:	00010713          	mv	a4,sp
  54:	10010793          	addi	a5,sp,256
  58:	00000693          	li	a3,0
  5c:	04000613          	li	a2,64
  60:	d006f7d3          	fcvt.s.w	fa5,a3
  64:	00e7a027          	fsw	fa4,0(a5)
  68:	00168693          	addi	a3,a3,1
  6c:	00f72027          	fsw	fa5,0(a4)
  70:	00478793          	addi	a5,a5,4
  74:	00470713          	addi	a4,a4,4
  78:	fec694e3          	bne	a3,a2,60 <main+0x20>
  7c:	000007b7          	lui	a5,0x0
  80:	0a87a503          	lw	a0,168(a5) # a8 <main+0x68>
  84:	10010613          	addi	a2,sp,256
  88:	00010593          	mv	a1,sp
  8c:	00000097          	auipc	ra,0x0
  90:	f84080e7          	jalr	-124(ra) # 10 <saxpy>
  94:	20c12083          	lw	ra,524(sp)
  98:	00000513          	li	a0,0
  9c:	21010113          	addi	sp,sp,528
  a0:	00008067          	ret
