import createModule, { type Simulator, type Snapshot } from '@wasm/sim.js';

class SimulatorService {
	private sim: Simulator | null = null;
	private ss: Snapshot | null = null;

	private listeners = new Set<() => void>;

	private simulator(): Simulator {
		if (!this.sim) throw new Error("Simulator not loaded");
		return this.sim;
	}

	private notify() {
		for (const listener of this.listeners) listener();
	}

	private update() {
		this.ss = this.simulator().snapshot();
		this.notify();
	}

	async init() {
		if (this.sim) return;

		const Module = await createModule();
		this.sim = new Module.Simulator();
		this.update();
	}

	subscribe(listener: () => void) {
		this.listeners.add(listener);
		return () => this.listeners.delete(listener);
	}

	snapshot(): Snapshot {
		return this.ss!;
	}

	running(): boolean {
		return this.simulator().running();
	}

	readout(): string {
		return this.simulator().readout();
	}

	load(program: Uint8Array) {
		this.simulator().load(program);
		this.update();
	}

	configure(config: string) {
		this.simulator().configure(config);
		this.update();
	}

	step() {
		this.simulator().step();
		this.update();
	}

	run() {
		while (this.running()) {
			this.simulator().step();
		}

		this.update();
	}
}

export const simulator = new SimulatorService();
