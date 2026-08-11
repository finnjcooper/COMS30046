// TypeScript bindings for emscripten-generated code.  Automatically generated at compile time.
interface WasmModule {
}

type EmbindString = ArrayBuffer|Uint8Array|Uint8ClampedArray|Int8Array|string;
export interface ClassHandle {
  isAliasOf(other: ClassHandle): boolean;
  delete(): void;
  deleteLater(): this;
  isDeleted(): boolean;
  // @ts-ignore - If targeting lower than ESNext, this symbol might not exist.
  [Symbol.dispose](): void;
  clone(): this;
}
export interface FetchQueue extends ClassHandle, Iterable<FetchEntry> {
  size(): number;
  get(_0: number): FetchEntry | undefined;
  push_back(_0: FetchEntry): void;
  resize(_0: number, _1: FetchEntry): void;
  set(_0: number, _1: FetchEntry): boolean;
}

export interface DecodeQueue extends ClassHandle, Iterable<DecodeEntry> {
  size(): number;
  get(_0: number): DecodeEntry | undefined;
  push_back(_0: DecodeEntry): void;
  resize(_0: number, _1: DecodeEntry): void;
  set(_0: number, _1: DecodeEntry): boolean;
}

export interface Regs extends ClassHandle, Iterable<Value> {
  size(): number;
  get(_0: number): Value | undefined;
  push_back(_0: Value): void;
  resize(_0: number, _1: Value): void;
  set(_0: number, _1: Value): boolean;
}

export interface RAT extends ClassHandle, Iterable<number> {
  push_back(_0: number): void;
  resize(_0: number, _1: number): void;
  size(): number;
  get(_0: number): number | undefined;
  set(_0: number, _1: number): boolean;
}

export interface ROB extends ClassHandle, Iterable<ROBEntry> {
  size(): number;
  get(_0: number): ROBEntry | undefined;
  push_back(_0: ROBEntry): void;
  resize(_0: number, _1: ROBEntry): void;
  set(_0: number, _1: ROBEntry): boolean;
}

export interface LSQ extends ClassHandle, Iterable<LSQEntry> {
  size(): number;
  get(_0: number): LSQEntry | undefined;
  push_back(_0: LSQEntry): void;
  resize(_0: number, _1: LSQEntry): void;
  set(_0: number, _1: LSQEntry): boolean;
}

export interface ReservationStations extends ClassHandle, Iterable<RSEntry> {
  size(): number;
  get(_0: number): RSEntry | undefined;
  push_back(_0: RSEntry): void;
  resize(_0: number, _1: RSEntry): void;
  set(_0: number, _1: RSEntry): boolean;
}

export interface ExecUnits extends ClassHandle, Iterable<ExecSnapshot> {
  size(): number;
  get(_0: number): ExecSnapshot | undefined;
  push_back(_0: ExecSnapshot): void;
  resize(_0: number, _1: ExecSnapshot): void;
  set(_0: number, _1: ExecSnapshot): boolean;
}

export interface LSUs extends ClassHandle, Iterable<LSUSnapshot> {
  size(): number;
  get(_0: number): LSUSnapshot | undefined;
  push_back(_0: LSUSnapshot): void;
  resize(_0: number, _1: LSUSnapshot): void;
  set(_0: number, _1: LSUSnapshot): boolean;
}

export interface Value extends ClassHandle {
  vector_value: boolean;
  is_vector(): boolean;
  as_scalar(): number;
  lane(_0: number): number;
  set_lane(_0: number, _1: number): void;
}

export interface OpValue<T extends number> {
  value: T;
}
export type Op = OpValue<0>|OpValue<1>|OpValue<2>|OpValue<3>|OpValue<4>|OpValue<5>|OpValue<6>|OpValue<7>|OpValue<8>|OpValue<9>|OpValue<10>|OpValue<11>|OpValue<12>|OpValue<13>|OpValue<14>|OpValue<15>|OpValue<16>|OpValue<17>|OpValue<18>|OpValue<19>|OpValue<20>|OpValue<21>|OpValue<22>|OpValue<23>|OpValue<24>|OpValue<25>|OpValue<26>|OpValue<27>|OpValue<28>|OpValue<29>|OpValue<30>|OpValue<31>|OpValue<32>|OpValue<33>|OpValue<34>|OpValue<35>|OpValue<36>|OpValue<37>|OpValue<38>|OpValue<39>|OpValue<40>|OpValue<41>|OpValue<42>|OpValue<43>|OpValue<44>|OpValue<45>|OpValue<46>|OpValue<47>|OpValue<48>|OpValue<49>|OpValue<50>|OpValue<51>|OpValue<52>|OpValue<53>|OpValue<54>|OpValue<55>|OpValue<56>|OpValue<57>|OpValue<58>|OpValue<59>|OpValue<60>|OpValue<61>|OpValue<62>|OpValue<63>|OpValue<64>|OpValue<65>|OpValue<66>|OpValue<67>|OpValue<68>|OpValue<69>|OpValue<70>|OpValue<71>|OpValue<72>|OpValue<73>|OpValue<74>|OpValue<75>|OpValue<76>|OpValue<77>|OpValue<78>|OpValue<79>|OpValue<80>|OpValue<81>|OpValue<82>|OpValue<83>|OpValue<84>|OpValue<85>|OpValue<86>|OpValue<87>|OpValue<88>|OpValue<89>|OpValue<90>|OpValue<91>;

export type Instruction = {
  op: Op,
  rd: number,
  rs1: number,
  rs2: number,
  imm: number,
  rs3: number,
  rm: number,
  sew: number
};

export type VectorState = {
  tag: number,
  vector_bits: number,
  vsew_bits: number,
  vl: number
};

export type Stats = {
  instructionCount: number,
  cycleCount: number,
  branchCount: number,
  mispredCount: number
};

export type ROBEntry = {
  ready: boolean,
  jumped: boolean,
  ctrl_handled: boolean,
  pred_taken: boolean,
  pred_target: number,
  op: Op,
  rd: number,
  value: Value,
  target: number,
  pc: number,
  vl: number,
  sew: number,
  tag: number
};

export type RSEntry = {
  busy: boolean,
  op: Op,
  Vj: Value,
  Vk: Value,
  Vl: Value,
  Qj: number,
  Qk: number,
  Ql: number,
  Qv: number,
  pc: number,
  imm: number,
  rm: number,
  vl: number,
  sew: number,
  tag: number
};

export type LSQEntry = {
  op: Op,
  tag: number,
  V: Value,
  Va: number,
  Q: number,
  Qa: number,
  vl: number,
  sew: number,
  issued: boolean,
  done: boolean
};

export type ExecEntry = {
  op: Op,
  value: Value,
  target: number,
  jumped: boolean,
  tag: number,
  vl: number,
  sew: number
};

export type ExecSnapshot = {
  busy: boolean,
  cycles_remaining: number,
  current: RSEntry
};

export type LSUSnapshot = {
  busy: boolean,
  cycles_remaining: number,
  current_tag: number
};

export type ExecPathSnapshot = {
  units: ExecUnits,
  stations: ReservationStations
};

export type LoadStorePathSnapshot = {
  agus: ExecPathSnapshot,
  lsus: LSUs,
  lsq: LSQ
};

export type RATSnapshot = {
  table: RAT
};

export type ROBSnapshot = {
  next_tag: number,
  entries: ROB
};

export type RegisterFileSnapshot = {
  regs: Regs
};

export type FetchEntry = {
  pc: number,
  instr: number
};

export type DecodeEntry = {
  pc: number,
  instr: Instruction,
  predTaken: boolean,
  predTarget: number
};

export type Snapshot = {
  program: EmbindString,
  config: EmbindString,
  halted: boolean,
  stalled: boolean,
  jumped: boolean,
  msg: EmbindString,
  stats: Stats,
  pc: number,
  vecState: VectorState,
  fetchQ: FetchQueue,
  decodeQ: DecodeQueue,
  rob: ROBSnapshot,
  rat: RATSnapshot,
  frat: RATSnapshot,
  vrat: RATSnapshot,
  alus: ExecPathSnapshot,
  muls: ExecPathSnapshot,
  ctrls: ExecPathSnapshot,
  fpus: ExecPathSnapshot,
  vecs: ExecPathSnapshot,
  lsus: LoadStorePathSnapshot,
  regs: RegisterFileSnapshot,
  fregs: RegisterFileSnapshot,
  vregs: RegisterFileSnapshot
};

export interface Simulator extends ClassHandle {
  load(_0: any, _1: EmbindString): void;
  configure(_0: EmbindString): void;
  reset(): void;
  step(): void;
  run(): void;
  snapshot(): Snapshot;
}

interface EmbindModule {
  FetchQueue: {
    new(): FetchQueue;
  };
  DecodeQueue: {
    new(): DecodeQueue;
  };
  Regs: {
    new(): Regs;
  };
  RAT: {
    new(): RAT;
  };
  ROB: {
    new(): ROB;
  };
  LSQ: {
    new(): LSQ;
  };
  ReservationStations: {
    new(): ReservationStations;
  };
  ExecUnits: {
    new(): ExecUnits;
  };
  LSUs: {
    new(): LSUs;
  };
  Value: {
    new(_0: number): Value;
  };
  Op: {INVALID: OpValue<0>, ADD: OpValue<1>, SUB: OpValue<2>, SLL: OpValue<3>, SLT: OpValue<4>, SLTU: OpValue<5>, XOR: OpValue<6>, SRL: OpValue<7>, SRA: OpValue<8>, OR: OpValue<9>, AND: OpValue<10>, ADDI: OpValue<11>, SLTI: OpValue<12>, SLTIU: OpValue<13>, XORI: OpValue<14>, ORI: OpValue<15>, ANDI: OpValue<16>, SLLI: OpValue<17>, SRLI: OpValue<18>, SRAI: OpValue<19>, LB: OpValue<20>, LH: OpValue<21>, LW: OpValue<22>, LBU: OpValue<23>, LHU: OpValue<24>, SB: OpValue<25>, SH: OpValue<26>, SW: OpValue<27>, BEQ: OpValue<28>, BNE: OpValue<29>, BLT: OpValue<30>, BGE: OpValue<31>, BLTU: OpValue<32>, BGEU: OpValue<33>, JAL: OpValue<34>, JALR: OpValue<35>, LUI: OpValue<36>, AUIPC: OpValue<37>, MUL: OpValue<38>, MULH: OpValue<39>, MULHSU: OpValue<40>, MULHU: OpValue<41>, DIV: OpValue<42>, DIVU: OpValue<43>, REM: OpValue<44>, REMU: OpValue<45>, FADD_S: OpValue<46>, FSUB_S: OpValue<47>, FMUL_S: OpValue<48>, FDIV_S: OpValue<49>, FSQRT_S: OpValue<50>, FMADD_S: OpValue<51>, FMSUB_S: OpValue<52>, FNMSUB_S: OpValue<53>, FNMADD_S: OpValue<54>, FSGNJ_S: OpValue<55>, FSGNJN_S: OpValue<56>, FSGNJX_S: OpValue<57>, FMIN_S: OpValue<58>, FMAX_S: OpValue<59>, FEQ_S: OpValue<60>, FLT_S: OpValue<61>, FLE_S: OpValue<62>, FLW: OpValue<63>, FSW: OpValue<64>, FMV_X_W: OpValue<65>, FMV_W_X: OpValue<66>, FCVT_W_S: OpValue<67>, FCVT_WU_S: OpValue<68>, FCVT_S_W: OpValue<69>, FCVT_S_WU: OpValue<70>, FCLASS_S: OpValue<71>, VSETVLI: OpValue<72>, VSETIVLI: OpValue<73>, VLE8_V: OpValue<74>, VLE16_V: OpValue<75>, VLE32_V: OpValue<76>, VSE8_V: OpValue<77>, VSE16_V: OpValue<78>, VSE32_V: OpValue<79>, VMUL_VV: OpValue<80>, VMACC_VV: OpValue<81>, VMACC_VX: OpValue<82>, VMV_V_I: OpValue<83>, VMV_V_X: OpValue<84>, VMV_S_X: OpValue<85>, VMV_X_S: OpValue<86>, VMV1R_V: OpValue<87>, VREDSUM_VS: OpValue<88>, VFMV_V_F: OpValue<89>, VFMACC_VV: OpValue<90>, VFMACC_VF: OpValue<91>};
  Simulator: {
    new(): Simulator;
  };
}

export type MainModule = WasmModule & EmbindModule;
export default function MainModuleFactory (options?: unknown): Promise<MainModule>;
