
build/saxpyv.elf:     file format elf32-littleriscv


Disassembly of section .text:

00000000 <saxpy>:
   0:	02c05663          	blez	a2,2c <saxpy+0x2c>
   4:	0d0677d7          	vsetvli	a5,a2,e32,m1,ta,ma
   8:	02056107          	vle32.v	v2,(a0)
   c:	0205e087          	vle32.v	v1,(a1)
  10:	00279713          	slli	a4,a5,0x2
  14:	40f60633          	sub	a2,a2,a5
  18:	00e50533          	add	a0,a0,a4
  1c:	b22550d7          	vfmacc.vf	v1,fa0,v2
  20:	0205e0a7          	vse32.v	v1,(a1)
  24:	00e585b3          	add	a1,a1,a4
  28:	fcc04ee3          	bgtz	a2,4 <saxpy+0x4>
  2c:	00008067          	ret

00000030 <main>:
  30:	000007b7          	lui	a5,0x0
  34:	ff010113          	addi	sp,sp,-16
  38:	0787a507          	flw	fa0,120(a5) # 78 <main+0x48>
  3c:	00812423          	sw	s0,8(sp)
  40:	00001537          	lui	a0,0x1
  44:	00000437          	lui	s0,0x0
  48:	07c40593          	addi	a1,s0,124 # 7c <y>
  4c:	07c50513          	addi	a0,a0,124 # 107c <x>
  50:	40000613          	li	a2,1024
  54:	00112623          	sw	ra,12(sp)
  58:	07c40413          	addi	s0,s0,124
  5c:	00000097          	auipc	ra,0x0
  60:	fa4080e7          	jalr	-92(ra) # 0 <saxpy>
  64:	00c12083          	lw	ra,12(sp)
  68:	00812403          	lw	s0,8(sp)
  6c:	00000513          	li	a0,0
  70:	01010113          	addi	sp,sp,16
  74:	00008067          	ret
