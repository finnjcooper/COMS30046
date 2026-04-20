
G:/Finn/Stuff/Bristol/COMS30046/src/../bench/build/conv2d_vec.elf:     file format elf32-littleriscv


Disassembly of section .text:

00000000 <_start>:
   0:	00000097          	auipc	ra,0x0
   4:	1c4080e7          	jalr	452(ra) # 1c4 <main>
   8:	00000073          	ecall
   c:	0000006f          	j	c <_start+0xc>

00000010 <dot3>:
  10:	cd01f057          	vsetivli	zero,3,e32,m1,ta,ma
  14:	02056087          	vle32.v	v1,(a0)
  18:	0205e187          	vle32.v	v3,(a1)
  1c:	5e003157          	vmv.v.i	v2,0
  20:	9611a0d7          	vmul.vv	v1,v1,v3
  24:	021120d7          	vredsum.vs	v1,v1,v2
  28:	42102557          	vmv.x.s	a0,v1
  2c:	00008067          	ret

00000030 <conv2d>:
  30:	fd010113          	addi	sp,sp,-48
  34:	02812423          	sw	s0,40(sp)
  38:	01312e23          	sw	s3,28(sp)
  3c:	01612823          	sw	s6,16(sp)
  40:	01712623          	sw	s7,12(sp)
  44:	02112623          	sw	ra,44(sp)
  48:	02912223          	sw	s1,36(sp)
  4c:	03212023          	sw	s2,32(sp)
  50:	00050b93          	mv	s7,a0
  54:	00058993          	mv	s3,a1
  58:	00060413          	mv	s0,a2
  5c:	06050b13          	addi	s6,a0,96
  60:	00042023          	sw	zero,0(s0)
  64:	00098593          	mv	a1,s3
  68:	000b8513          	mv	a0,s7
  6c:	00000097          	auipc	ra,0x0
  70:	fa4080e7          	jalr	-92(ra) # 10 <dot3>
  74:	000b8493          	mv	s1,s7
  78:	018b8b93          	addi	s7,s7,24
  7c:	00a42023          	sw	a0,0(s0)
  80:	00050913          	mv	s2,a0
  84:	00c98593          	addi	a1,s3,12
  88:	000b8513          	mv	a0,s7
  8c:	00000097          	auipc	ra,0x0
  90:	f84080e7          	jalr	-124(ra) # 10 <dot3>
  94:	00a90933          	add	s2,s2,a0
  98:	01242023          	sw	s2,0(s0)
  9c:	01898593          	addi	a1,s3,24
  a0:	03048513          	addi	a0,s1,48
  a4:	00000097          	auipc	ra,0x0
  a8:	f6c080e7          	jalr	-148(ra) # 10 <dot3>
  ac:	00a90933          	add	s2,s2,a0
  b0:	01242023          	sw	s2,0(s0)
  b4:	00042223          	sw	zero,4(s0)
  b8:	00098593          	mv	a1,s3
  bc:	00448513          	addi	a0,s1,4
  c0:	00000097          	auipc	ra,0x0
  c4:	f50080e7          	jalr	-176(ra) # 10 <dot3>
  c8:	00a42223          	sw	a0,4(s0)
  cc:	00050913          	mv	s2,a0
  d0:	00c98593          	addi	a1,s3,12
  d4:	01c48513          	addi	a0,s1,28
  d8:	00000097          	auipc	ra,0x0
  dc:	f38080e7          	jalr	-200(ra) # 10 <dot3>
  e0:	00a90933          	add	s2,s2,a0
  e4:	01242223          	sw	s2,4(s0)
  e8:	01898593          	addi	a1,s3,24
  ec:	03448513          	addi	a0,s1,52
  f0:	00000097          	auipc	ra,0x0
  f4:	f20080e7          	jalr	-224(ra) # 10 <dot3>
  f8:	00a90933          	add	s2,s2,a0
  fc:	01242223          	sw	s2,4(s0)
 100:	00042423          	sw	zero,8(s0)
 104:	00098593          	mv	a1,s3
 108:	00848513          	addi	a0,s1,8
 10c:	00000097          	auipc	ra,0x0
 110:	f04080e7          	jalr	-252(ra) # 10 <dot3>
 114:	00a42423          	sw	a0,8(s0)
 118:	00050913          	mv	s2,a0
 11c:	00c98593          	addi	a1,s3,12
 120:	02048513          	addi	a0,s1,32
 124:	00000097          	auipc	ra,0x0
 128:	eec080e7          	jalr	-276(ra) # 10 <dot3>
 12c:	00a90933          	add	s2,s2,a0
 130:	01242423          	sw	s2,8(s0)
 134:	01898593          	addi	a1,s3,24
 138:	03848513          	addi	a0,s1,56
 13c:	00000097          	auipc	ra,0x0
 140:	ed4080e7          	jalr	-300(ra) # 10 <dot3>
 144:	00a90933          	add	s2,s2,a0
 148:	01242423          	sw	s2,8(s0)
 14c:	00042623          	sw	zero,12(s0)
 150:	00098593          	mv	a1,s3
 154:	00c48513          	addi	a0,s1,12
 158:	00000097          	auipc	ra,0x0
 15c:	eb8080e7          	jalr	-328(ra) # 10 <dot3>
 160:	00a42623          	sw	a0,12(s0)
 164:	00050913          	mv	s2,a0
 168:	00c98593          	addi	a1,s3,12
 16c:	02448513          	addi	a0,s1,36
 170:	00000097          	auipc	ra,0x0
 174:	ea0080e7          	jalr	-352(ra) # 10 <dot3>
 178:	00a90933          	add	s2,s2,a0
 17c:	01242623          	sw	s2,12(s0)
 180:	03c48513          	addi	a0,s1,60
 184:	01898593          	addi	a1,s3,24
 188:	00000097          	auipc	ra,0x0
 18c:	e88080e7          	jalr	-376(ra) # 10 <dot3>
 190:	00a90933          	add	s2,s2,a0
 194:	01242623          	sw	s2,12(s0)
 198:	01040413          	addi	s0,s0,16
 19c:	ed6b92e3          	bne	s7,s6,60 <conv2d+0x30>
 1a0:	02c12083          	lw	ra,44(sp)
 1a4:	02812403          	lw	s0,40(sp)
 1a8:	02412483          	lw	s1,36(sp)
 1ac:	02012903          	lw	s2,32(sp)
 1b0:	01c12983          	lw	s3,28(sp)
 1b4:	01012b03          	lw	s6,16(sp)
 1b8:	00c12b83          	lw	s7,12(sp)
 1bc:	03010113          	addi	sp,sp,48
 1c0:	00008067          	ret

000001c4 <main>:
 1c4:	000005b7          	lui	a1,0x0
 1c8:	20058593          	addi	a1,a1,512 # 200 <kernel>
 1cc:	00000637          	lui	a2,0x0
 1d0:	ff010113          	addi	sp,sp,-16
 1d4:	2b460613          	addi	a2,a2,692 # 2b4 <output>
 1d8:	02458513          	addi	a0,a1,36
 1dc:	00112623          	sw	ra,12(sp)
 1e0:	00000097          	auipc	ra,0x0
 1e4:	e50080e7          	jalr	-432(ra) # 30 <conv2d>
 1e8:	000007b7          	lui	a5,0x0
 1ec:	2b478613          	addi	a2,a5,692 # 2b4 <output>
 1f0:	00c12083          	lw	ra,12(sp)
 1f4:	00000513          	li	a0,0
 1f8:	01010113          	addi	sp,sp,16
 1fc:	00008067          	ret
