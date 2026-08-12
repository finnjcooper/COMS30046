import createModule, { type Simulator, type Snapshot } from '@wasm/sim.js';

class SimulatorService {
	private simulator_: Simulator | null = null;
	private snapshot_: Snapshot | null = null;
	private promise: Promise<void> | null = null;
	private listeners = new Set<() => void>;

	private simulator(): Simulator {
		if (!this.simulator_) throw new Error("Simulator not loaded");
		return this.simulator_;
	}

	private notify() {
		for (const listener of this.listeners) listener();
	}

	private update() {
		this.snapshot_ = this.simulator().snapshot();
		this.notify();
	}

	async init() {
		if (this.simulator_) return;
		if (!this.promise) {
			this.promise = createModule()
			.then(Module => {
				this.simulator_ = new Module.Simulator();
				this.update();
			});
		}

		return this.promise;
	}

	subscribe = (listener: () => void) => {
		this.listeners.add(listener);
		return () => this.listeners.delete(listener);
	}

	snapshot = (): Snapshot | null => {
		return this.snapshot_;
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
