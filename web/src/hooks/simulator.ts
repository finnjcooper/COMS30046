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
		console.log("Simulator initialized");
	}

	subscribe = (listener: () => void) => {
		this.listeners.add(listener);
		return () => this.listeners.delete(listener);
	}

	snapshot = (): Snapshot => {
		return this.ss!;
	}

	load = (program: Uint8Array, name: string) => {
		this.simulator().load(program, name);
		this.update();
	}

	configure = (config: string) => {
		this.simulator().configure(config);
		this.update();
	}

	reset = () => {
		this.simulator().reset();
		this.update();
	}

	step = () => {
		this.simulator().step();
		this.update();
	}

	run = () => {
		this.simulator().run();
		this.update();
	}
}

export const simulator = new SimulatorService();
