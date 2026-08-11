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

export type Stats = {
  instructionCount: number,
  cycleCount: number,
  branchCount: number,
  mispredCount: number
};

export type FetchEntry = {
  pc: number,
  instr: number
};

export type DecodeEntry = {
  pc: number,
  predTaken: boolean,
  predTarget: number
};

export type Snapshot = {
  program: EmbindString,
  config: EmbindString,
  msg: EmbindString,
  stats: Stats,
  halted: boolean,
  stalled: boolean,
  jumped: boolean,
  pc: number,
  fetchQ: FetchQueue,
  decodeQ: DecodeQueue
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
  Simulator: {
    new(): Simulator;
  };
}

export type MainModule = WasmModule & EmbindModule;
export default function MainModuleFactory (options?: unknown): Promise<MainModule>;
