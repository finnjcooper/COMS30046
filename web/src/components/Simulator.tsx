import { useEffect, useState } from 'react';
import useSimulator, { type SimulatorInstance } from '@hooks/useSimulator';

export default function Simulator() {
	const url = (path: string) => `${import.meta.env.BASE_URL}${path.replace(/^\//, '')}`;
	const [simulator, setSimulator] = useState<SimulatorInstance | null>(null);
	const [binary, setBinary] = useState<Uint8Array | null>(null);
	const [config, setConfig] = useState<string | null>(null);

	useEffect(() => {
		let cancelled = false;

		async function loadSimulator() {
			try {
				const instance = await useSimulator();
				if (!cancelled) setSimulator(instance);
			} catch (err) {
				console.error(err);
			}
		}

		async function fetchStuff() {
			const [binary, config] = await Promise.all([
				fetch(url('fibonacci.elf')).then(res => res.arrayBuffer()).then(buf => new Uint8Array(buf)),
				fetch(url('config.json')).then(res => res.text()),
			]);

			if (cancelled) return;
			setBinary(binary);
			setConfig(config);
		}

		void loadSimulator();
		void fetchStuff();

		return () => { cancelled = true; };
	}, []);

	if (!simulator) return <div className="loading loading-spinner loading-xl"></div>;

	simulator.loadElf(binary);
	simulator.loadConfig(config);
	simulator.build();


	return (
		<div className="space-x-4">
			<button className="btn btn-info" onClick={() => simulator.run(10000)}>Run</button>
			<button className="btn btn-primary" onClick={() => simulator.step()}>Step</button>
			<button className="btn btn-error" onClick={() => simulator.reset()}>Reset</button>
		</div>
	);
}
