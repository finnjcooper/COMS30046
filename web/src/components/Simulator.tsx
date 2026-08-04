import { useEffect, useState } from 'react';
import useSimulator from '@hooks/useSimulator';

type StepSnapshot = {
	jumped: boolean;
	stalled: boolean;
	pc: number;
	stats: any;
	// log: any;
	readout: string;
};

export default function Simulator() {
	const url = (path: string) => `${import.meta.env.BASE_URL}${path.replace(/^\//, '')}`;
	const { simulator, loading, error } = useSimulator();
	const [binary, setBinary] = useState<Uint8Array | null>(null);
	const [config, setConfig] = useState<string | null>(null);
	const [snapshot, setSnapshot] = useState<StepSnapshot | null>(null);

	useEffect(() => {
		async function fetchStuff() {
			const [binary, config] = await Promise.all([
				fetch(url('fibonacci.elf')).then(res => res.arrayBuffer()).then(buf => new Uint8Array(buf)),
				fetch(url('config.json')).then(res => res.text()),
			]);

			setBinary(binary);
			setConfig(config);
		}

		void fetchStuff();
	}, []);

	useEffect(() => {
		if (!simulator || !binary || !config) return;

		simulator.loadElf(binary);
		simulator.loadConfig(config);
		simulator.build();
		simulator.setStepCallback((nextSnapshot: StepSnapshot) => {
			setSnapshot(nextSnapshot);
		});
	}, [simulator, binary, config]);

	if (loading || !binary || !config) return <div className="loading loading-spinner loading-xl"></div>;
	if (error) return <h3 className="text-error">Failed to load simulator.</h3>;
	if (!simulator) return <div className="loading loading-spinner loading-xl"></div>;

	return (
		<div className="space-y-4">
			<div className="space-x-4">
				<button className="btn btn-info" onClick={() => { simulator.run(1000); }}>Run</button>
				<button className="btn btn-primary" onClick={() => { simulator.step(); }}>Step</button>
				<button className="btn btn-error" onClick={() => { simulator.reset(); }}>Reset</button>
			</div>
		
			{snapshot && (
				<div>
					<h2>Simulator Stats</h2>
					<p>{snapshot.stats.instructionCount} instructions</p>
					<p>{snapshot.stats.cycleCount} cycles</p>
					<p>{snapshot.stats.branchCount} branches</p>
					<p>{snapshot.stats.mispredCount} mispredictions</p>
					<p>{snapshot.readout}</p>
				</div>
			)}
		</div>
	);
}
