import { useEffect, useState } from 'react';

async function createSimulator() {
	const moduleUrl = new URL('/sim.mjs', import.meta.url).href;
	const { default: createModule } = await import(/* @vite-ignore */ moduleUrl);
	const Module = await createModule();
	const simulator = new Module.Simulator();

	return {
		loadElf: simulator.loadElf.bind(simulator),
		loadConfig: simulator.loadConfig.bind(simulator),
		build: simulator.build.bind(simulator),
		step: simulator.step.bind(simulator),
		run: simulator.run.bind(simulator),
		reset: simulator.reset.bind(simulator),
		setStepCallback: simulator.setStepCallback.bind(simulator),
	};
}

type SimulatorInstance = Awaited<ReturnType<typeof createSimulator>>;

export default function useSimulator() {
	const [simulator, setSimulator] = useState<SimulatorInstance | null>(null);
	const [loading, setLoading] = useState(true);
	const [error, setError] = useState<unknown>(null);

	useEffect(() => {
		let cancelled = false;

		async function loadSimulator() {
			try {
				const instance = await createSimulator();
				if (!cancelled) setSimulator(instance);
			} catch (err) {
				if (!cancelled) setError(err);
			} finally {
				if (!cancelled) setLoading(false);
			}
		}

		void loadSimulator();

		return () => {
			cancelled = true;
		};
	}, []);

	return { simulator, loading, error };
}
