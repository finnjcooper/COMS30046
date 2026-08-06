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
export type Stats = {
  instructionCount: number,
  cycleCount: number,
  branchCount: number,
  mispredCount: number
};

export type Snapshot = {
  pc: number,
  stats: Stats
};

export interface Simulator extends ClassHandle {
  load(_0: any): boolean;
  configure(_0: EmbindString): boolean;
  step(): boolean;
  running(): boolean;
  readout(): string;
  snapshot(): Snapshot;
}

interface EmbindModule {
  Simulator: {
    new(): Simulator;
  };
}

export type MainModule = WasmModule & EmbindModule;
export default function MainModuleFactory (options?: unknown): Promise<MainModule>;
