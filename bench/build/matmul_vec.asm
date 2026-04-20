
G:/Finn/Stuff/Bristol/COMS30046/src/../bench/build/matmul_vec.elf:     file format elf32-littleriscv


Disassembly of section .text:

00000000 <_start>:
   0:	00000097          	auipc	ra,0x0
   4:	2e0080e7          	jalr	736(ra) # 2e0 <main>
   8:	00000073          	ecall
   c:	0000006f          	j	c <_start+0xc>

00000010 <dot_row>:
  10:	00050713          	mv	a4,a0
  14:	00000513          	li	a0,0
  18:	02c05e63          	blez	a2,54 <dot_row+0x44>
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
  4c:	fcc048e3          	bgtz	a2,1c <dot_row+0xc>
  50:	00008067          	ret
  54:	00008067          	ret

00000058 <mat_mul>:
  58:	fd010113          	addi	sp,sp,-48
  5c:	02812423          	sw	s0,40(sp)
  60:	00060413          	mv	s0,a2
  64:	00500613          	li	a2,5
  68:	02112623          	sw	ra,44(sp)
  6c:	02912223          	sw	s1,36(sp)
  70:	03212023          	sw	s2,32(sp)
  74:	00050493          	mv	s1,a0
  78:	00058913          	mv	s2,a1
  7c:	00000097          	auipc	ra,0x0
  80:	f94080e7          	jalr	-108(ra) # 10 <dot_row>
  84:	00a42023          	sw	a0,0(s0)
  88:	01490593          	addi	a1,s2,20
  8c:	00048513          	mv	a0,s1
  90:	00500613          	li	a2,5
  94:	00000097          	auipc	ra,0x0
  98:	f7c080e7          	jalr	-132(ra) # 10 <dot_row>
  9c:	00a42223          	sw	a0,4(s0)
  a0:	02890593          	addi	a1,s2,40
  a4:	00048513          	mv	a0,s1
  a8:	00500613          	li	a2,5
  ac:	00000097          	auipc	ra,0x0
  b0:	f64080e7          	jalr	-156(ra) # 10 <dot_row>
  b4:	00a42423          	sw	a0,8(s0)
  b8:	03c90593          	addi	a1,s2,60
  bc:	00048513          	mv	a0,s1
  c0:	00500613          	li	a2,5
  c4:	00000097          	auipc	ra,0x0
  c8:	f4c080e7          	jalr	-180(ra) # 10 <dot_row>
  cc:	00a42623          	sw	a0,12(s0)
  d0:	05090593          	addi	a1,s2,80
  d4:	00048513          	mv	a0,s1
  d8:	00500613          	li	a2,5
  dc:	00000097          	auipc	ra,0x0
  e0:	f34080e7          	jalr	-204(ra) # 10 <dot_row>
  e4:	00a42823          	sw	a0,16(s0)
  e8:	00090593          	mv	a1,s2
  ec:	01448513          	addi	a0,s1,20
  f0:	00500613          	li	a2,5
  f4:	00000097          	auipc	ra,0x0
  f8:	f1c080e7          	jalr	-228(ra) # 10 <dot_row>
  fc:	00a42a23          	sw	a0,20(s0)
 100:	01490593          	addi	a1,s2,20
 104:	01448513          	addi	a0,s1,20
 108:	00500613          	li	a2,5
 10c:	00000097          	auipc	ra,0x0
 110:	f04080e7          	jalr	-252(ra) # 10 <dot_row>
 114:	00a42c23          	sw	a0,24(s0)
 118:	02890593          	addi	a1,s2,40
 11c:	01448513          	addi	a0,s1,20
 120:	00500613          	li	a2,5
 124:	00000097          	auipc	ra,0x0
 128:	eec080e7          	jalr	-276(ra) # 10 <dot_row>
 12c:	00a42e23          	sw	a0,28(s0)
 130:	03c90593          	addi	a1,s2,60
 134:	01448513          	addi	a0,s1,20
 138:	00500613          	li	a2,5
 13c:	00000097          	auipc	ra,0x0
 140:	ed4080e7          	jalr	-300(ra) # 10 <dot_row>
 144:	02a42023          	sw	a0,32(s0)
 148:	05090593          	addi	a1,s2,80
 14c:	01448513          	addi	a0,s1,20
 150:	00500613          	li	a2,5
 154:	00000097          	auipc	ra,0x0
 158:	ebc080e7          	jalr	-324(ra) # 10 <dot_row>
 15c:	02a42223          	sw	a0,36(s0)
 160:	00090593          	mv	a1,s2
 164:	02848513          	addi	a0,s1,40
 168:	00500613          	li	a2,5
 16c:	00000097          	auipc	ra,0x0
 170:	ea4080e7          	jalr	-348(ra) # 10 <dot_row>
 174:	02a42423          	sw	a0,40(s0)
 178:	01490593          	addi	a1,s2,20
 17c:	02848513          	addi	a0,s1,40
 180:	00500613          	li	a2,5
 184:	00000097          	auipc	ra,0x0
 188:	e8c080e7          	jalr	-372(ra) # 10 <dot_row>
 18c:	02a42623          	sw	a0,44(s0)
 190:	02890593          	addi	a1,s2,40
 194:	02848513          	addi	a0,s1,40
 198:	00500613          	li	a2,5
 19c:	00000097          	auipc	ra,0x0
 1a0:	e74080e7          	jalr	-396(ra) # 10 <dot_row>
 1a4:	02a42823          	sw	a0,48(s0)
 1a8:	03c90593          	addi	a1,s2,60
 1ac:	02848513          	addi	a0,s1,40
 1b0:	00500613          	li	a2,5
 1b4:	00000097          	auipc	ra,0x0
 1b8:	e5c080e7          	jalr	-420(ra) # 10 <dot_row>
 1bc:	02a42a23          	sw	a0,52(s0)
 1c0:	05090593          	addi	a1,s2,80
 1c4:	02848513          	addi	a0,s1,40
 1c8:	00500613          	li	a2,5
 1cc:	00000097          	auipc	ra,0x0
 1d0:	e44080e7          	jalr	-444(ra) # 10 <dot_row>
 1d4:	02a42c23          	sw	a0,56(s0)
 1d8:	00090593          	mv	a1,s2
 1dc:	03c48513          	addi	a0,s1,60
 1e0:	00500613          	li	a2,5
 1e4:	00000097          	auipc	ra,0x0
 1e8:	e2c080e7          	jalr	-468(ra) # 10 <dot_row>
 1ec:	02a42e23          	sw	a0,60(s0)
 1f0:	01490593          	addi	a1,s2,20
 1f4:	03c48513          	addi	a0,s1,60
 1f8:	00500613          	li	a2,5
 1fc:	00000097          	auipc	ra,0x0
 200:	e14080e7          	jalr	-492(ra) # 10 <dot_row>
 204:	04a42023          	sw	a0,64(s0)
 208:	02890593          	addi	a1,s2,40
 20c:	03c48513          	addi	a0,s1,60
 210:	00500613          	li	a2,5
 214:	00000097          	auipc	ra,0x0
 218:	dfc080e7          	jalr	-516(ra) # 10 <dot_row>
 21c:	04a42223          	sw	a0,68(s0)
 220:	03c90593          	addi	a1,s2,60
 224:	03c48513          	addi	a0,s1,60
 228:	00500613          	li	a2,5
 22c:	00000097          	auipc	ra,0x0
 230:	de4080e7          	jalr	-540(ra) # 10 <dot_row>
 234:	04a42423          	sw	a0,72(s0)
 238:	05090593          	addi	a1,s2,80
 23c:	03c48513          	addi	a0,s1,60
 240:	00500613          	li	a2,5
 244:	00000097          	auipc	ra,0x0
 248:	dcc080e7          	jalr	-564(ra) # 10 <dot_row>
 24c:	04a42623          	sw	a0,76(s0)
 250:	00090593          	mv	a1,s2
 254:	05048513          	addi	a0,s1,80
 258:	00500613          	li	a2,5
 25c:	00000097          	auipc	ra,0x0
 260:	db4080e7          	jalr	-588(ra) # 10 <dot_row>
 264:	04a42823          	sw	a0,80(s0)
 268:	01490593          	addi	a1,s2,20
 26c:	05048513          	addi	a0,s1,80
 270:	00500613          	li	a2,5
 274:	00000097          	auipc	ra,0x0
 278:	d9c080e7          	jalr	-612(ra) # 10 <dot_row>
 27c:	04a42a23          	sw	a0,84(s0)
 280:	02890593          	addi	a1,s2,40
 284:	05048513          	addi	a0,s1,80
 288:	00500613          	li	a2,5
 28c:	00000097          	auipc	ra,0x0
 290:	d84080e7          	jalr	-636(ra) # 10 <dot_row>
 294:	04a42c23          	sw	a0,88(s0)
 298:	03c90593          	addi	a1,s2,60
 29c:	05048513          	addi	a0,s1,80
 2a0:	00500613          	li	a2,5
 2a4:	00000097          	auipc	ra,0x0
 2a8:	d6c080e7          	jalr	-660(ra) # 10 <dot_row>
 2ac:	04a42e23          	sw	a0,92(s0)
 2b0:	05090593          	addi	a1,s2,80
 2b4:	05048513          	addi	a0,s1,80
 2b8:	00500613          	li	a2,5
 2bc:	00000097          	auipc	ra,0x0
 2c0:	d54080e7          	jalr	-684(ra) # 10 <dot_row>
 2c4:	06a42023          	sw	a0,96(s0)
 2c8:	02c12083          	lw	ra,44(sp)
 2cc:	02812403          	lw	s0,40(sp)
 2d0:	02412483          	lw	s1,36(sp)
 2d4:	02012903          	lw	s2,32(sp)
 2d8:	03010113          	addi	sp,sp,48
 2dc:	00008067          	ret

000002e0 <main>:
 2e0:	000005b7          	lui	a1,0x0
 2e4:	31c58593          	addi	a1,a1,796 # 31c <bt>
 2e8:	00000637          	lui	a2,0x0
 2ec:	ff010113          	addi	sp,sp,-16
 2f0:	3e460613          	addi	a2,a2,996 # 3e4 <c>
 2f4:	06458513          	addi	a0,a1,100
 2f8:	00112623          	sw	ra,12(sp)
 2fc:	00000097          	auipc	ra,0x0
 300:	d5c080e7          	jalr	-676(ra) # 58 <mat_mul>
 304:	000007b7          	lui	a5,0x0
 308:	3e478613          	addi	a2,a5,996 # 3e4 <c>
 30c:	00c12083          	lw	ra,12(sp)
 310:	00000513          	li	a0,0
 314:	01010113          	addi	sp,sp,16
 318:	00008067          	ret
