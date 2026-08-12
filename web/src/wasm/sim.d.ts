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
export interface FetchQueue extends ClassHandle, Iterable<FetchState> {
  size(): number;
  get(_0: number): FetchState | undefined;
  push_back(_0: FetchState): void;
  resize(_0: number, _1: FetchState): void;
  set(_0: number, _1: FetchState): boolean;
}

export interface DecodeQueue extends ClassHandle, Iterable<DecodeState> {
  size(): number;
  get(_0: number): DecodeState | undefined;
  push_back(_0: DecodeState): void;
  resize(_0: number, _1: DecodeState): void;
  set(_0: number, _1: DecodeState): boolean;
}

export interface Registers extends ClassHandle, Iterable<number> {
  push_back(_0: number): void;
  resize(_0: number, _1: number): void;
  size(): number;
  get(_0: number): number | undefined;
  set(_0: number, _1: number): boolean;
}

export interface VectorRegisters extends ClassHandle, Iterable<VectorRegister> {
  size(): number;
  get(_0: number): VectorRegister | undefined;
  push_back(_0: VectorRegister): void;
  resize(_0: number, _1: VectorRegister): void;
  set(_0: number, _1: VectorRegister): boolean;
}

export interface ROB extends ClassHandle, Iterable<ROBState> {
  size(): number;
  get(_0: number): ROBState | undefined;
  push_back(_0: ROBState): void;
  resize(_0: number, _1: ROBState): void;
  set(_0: number, _1: ROBState): boolean;
}

export interface LSQ extends ClassHandle, Iterable<LSQState> {
  size(): number;
  get(_0: number): LSQState | undefined;
  push_back(_0: LSQState): void;
  resize(_0: number, _1: LSQState): void;
  set(_0: number, _1: LSQState): boolean;
}

export interface ReservationStations extends ClassHandle, Iterable<RSState> {
  size(): number;
  get(_0: number): RSState | undefined;
  push_back(_0: RSState): void;
  resize(_0: number, _1: RSState): void;
  set(_0: number, _1: RSState): boolean;
}

export interface ExecUnits extends ClassHandle, Iterable<ExecState> {
  size(): number;
  get(_0: number): ExecState | undefined;
  push_back(_0: ExecState): void;
  resize(_0: number, _1: ExecState): void;
  set(_0: number, _1: ExecState): boolean;
}

export interface OpValue<T extends number> {
  value: T;
}
export type Op = OpValue<0>|OpValue<1>|OpValue<2>|OpValue<3>|OpValue<4>|OpValue<5>|OpValue<6>|OpValue<7>|OpValue<8>|OpValue<9>|OpValue<10>|OpValue<11>|OpValue<12>|OpValue<13>|OpValue<14>|OpValue<15>|OpValue<16>|OpValue<17>|OpValue<18>|OpValue<19>|OpValue<20>|OpValue<21>|OpValue<22>|OpValue<23>|OpValue<24>|OpValue<25>|OpValue<26>|OpValue<27>|OpValue<28>|OpValue<29>|OpValue<30>|OpValue<31>|OpValue<32>|OpValue<33>|OpValue<34>|OpValue<35>|OpValue<36>|OpValue<37>|OpValue<38>|OpValue<39>|OpValue<40>|OpValue<41>|OpValue<42>|OpValue<43>|OpValue<44>|OpValue<45>|OpValue<46>|OpValue<47>|OpValue<48>|OpValue<49>|OpValue<50>|OpValue<51>|OpValue<52>|OpValue<53>|OpValue<54>|OpValue<55>|OpValue<56>|OpValue<57>|OpValue<58>|OpValue<59>|OpValue<60>|OpValue<61>|OpValue<62>|OpValue<63>|OpValue<64>|OpValue<65>|OpValue<66>|OpValue<67>|OpValue<68>|OpValue<69>|OpValue<70>|OpValue<71>|OpValue<72>|OpValue<73>|OpValue<74>|OpValue<75>|OpValue<76>|OpValue<77>|OpValue<78>|OpValue<79>|OpValue<80>|OpValue<81>|OpValue<82>|OpValue<83>|OpValue<84>|OpValue<85>|OpValue<86>|OpValue<87>|OpValue<88>|OpValue<89>|OpValue<90>|OpValue<91>;

export type Stats = {
  instructionCount: number,
  cycleCount: number,
  branchCount: number,
  mispredCount: number
};

export type VectorState = {
  tag: number,
  vector_bits: number,
  vsew_bits: number,
  vl: number
};

export type VectorRegister = {
  lanes: Registers
};

export type RSState = {
  op: Op,
  Vj: number,
  Vk: number,
  Vl: number,
  Qj: number,
  Qk: number,
  Ql: number,
  Qv: number,
  pc: number,
  tag: number
};

export type LSQState = {
  op: Op,
  tag: number,
  V: number,
  Va: number,
  Q: number,
  Qa: number
};

export type ExecState = {
  busy: boolean,
  op: Op,
  tag: number,
  cycles_remaining: number
};

export type ExecPathState = {
  units: ExecUnits,
  stations: ReservationStations
};

export type LoadStoreState = {
  agus: ExecUnits,
  lsus: ExecUnits,
  stations: ReservationStations,
  lsq: LSQ
};

export type ROBState = {
  jumped: boolean,
  target: number,
  pred_taken: boolean,
  pred_target: number,
  op: Op,
  pc: number,
  tag: number
};

export type FetchState = {
  pc: number,
  instr: number
};

export type DecodeState = {
  pc: number,
  op: Op,
  predTaken: boolean,
  predTarget: number
};

export type RegisterState = {
  vec_state: VectorState,
  rat: Registers,
  frat: Registers,
  vrat: Registers,
  regs: Registers,
  fregs: Registers,
  vregs: VectorRegisters
};

export type PipelineState = {
  fetch_q: FetchQueue,
  decode_q: DecodeQueue,
  rob: ROB,
  alus: ExecPathState,
  muls: ExecPathState,
  ctrls: ExecPathState,
  fpus: ExecPathState,
  vecs: ExecPathState,
  lsus: LoadStoreState
};

export type Snapshot = {
  program: EmbindString,
  config: EmbindString,
  halted: boolean,
  stalled: boolean,
  jumped: boolean,
  pc: number,
  stats: Stats,
  registers: RegisterState,
  pipeline: PipelineState
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
  Registers: {
    new(): Registers;
  };
  VectorRegisters: {
    new(): VectorRegisters;
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
  Op: {INVALID: OpValue<0>, ADD: OpValue<1>, SUB: OpValue<2>, SLL: OpValue<3>, SLT: OpValue<4>, SLTU: OpValue<5>, XOR: OpValue<6>, SRL: OpValue<7>, SRA: OpValue<8>, OR: OpValue<9>, AND: OpValue<10>, ADDI: OpValue<11>, SLTI: OpValue<12>, SLTIU: OpValue<13>, XORI: OpValue<14>, ORI: OpValue<15>, ANDI: OpValue<16>, SLLI: OpValue<17>, SRLI: OpValue<18>, SRAI: OpValue<19>, LB: OpValue<20>, LH: OpValue<21>, LW: OpValue<22>, LBU: OpValue<23>, LHU: OpValue<24>, SB: OpValue<25>, SH: OpValue<26>, SW: OpValue<27>, BEQ: OpValue<28>, BNE: OpValue<29>, BLT: OpValue<30>, BGE: OpValue<31>, BLTU: OpValue<32>, BGEU: OpValue<33>, JAL: OpValue<34>, JALR: OpValue<35>, LUI: OpValue<36>, AUIPC: OpValue<37>, MUL: OpValue<38>, MULH: OpValue<39>, MULHSU: OpValue<40>, MULHU: OpValue<41>, DIV: OpValue<42>, DIVU: OpValue<43>, REM: OpValue<44>, REMU: OpValue<45>, FADD_S: OpValue<46>, FSUB_S: OpValue<47>, FMUL_S: OpValue<48>, FDIV_S: OpValue<49>, FSQRT_S: OpValue<50>, FMADD_S: OpValue<51>, FMSUB_S: OpValue<52>, FNMSUB_S: OpValue<53>, FNMADD_S: OpValue<54>, FSGNJ_S: OpValue<55>, FSGNJN_S: OpValue<56>, FSGNJX_S: OpValue<57>, FMIN_S: OpValue<58>, FMAX_S: OpValue<59>, FEQ_S: OpValue<60>, FLT_S: OpValue<61>, FLE_S: OpValue<62>, FLW: OpValue<63>, FSW: OpValue<64>, FMV_X_W: OpValue<65>, FMV_W_X: OpValue<66>, FCVT_W_S: OpValue<67>, FCVT_WU_S: OpValue<68>, FCVT_S_W: OpValue<69>, FCVT_S_WU: OpValue<70>, FCLASS_S: OpValue<71>, VSETVLI: OpValue<72>, VSETIVLI: OpValue<73>, VLE8_V: OpValue<74>, VLE16_V: OpValue<75>, VLE32_V: OpValue<76>, VSE8_V: OpValue<77>, VSE16_V: OpValue<78>, VSE32_V: OpValue<79>, VMUL_VV: OpValue<80>, VMACC_VV: OpValue<81>, VMACC_VX: OpValue<82>, VMV_V_I: OpValue<83>, VMV_V_X: OpValue<84>, VMV_S_X: OpValue<85>, VMV_X_S: OpValue<86>, VMV1R_V: OpValue<87>, VREDSUM_VS: OpValue<88>, VFMV_V_F: OpValue<89>, VFMACC_VV: OpValue<90>, VFMACC_VF: OpValue<91>};
  Simulator: {
    new(): Simulator;
  };
}

export type MainModule = WasmModule & EmbindModule;
export default function MainModuleFactory (options?: unknown): Promise<MainModule>;
