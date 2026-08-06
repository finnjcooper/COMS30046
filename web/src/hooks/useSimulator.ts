import { useEffect, useState, useSyncExternalStore } from 'react';
import { simulator } from '@hooks/simulator';

export default function useSimulator() {
	const [loading, setLoading] = useState(true);

	useEffect(() => {
		simulator.init().finally(() => setLoading(false));
	}, []);

	const snapshot = useSyncExternalStore(
		(callback) => simulator.subscribe(callback),
		() => simulator.snapshot()
	);

	return {
		loading,
		snapshot,
		running: simulator.running.bind(simulator),
		readout: simulator.readout.bind(simulator),
		load: simulator.load.bind(simulator),
		configure: simulator.configure.bind(simulator),
		step: simulator.step.bind(simulator),
		run: simulator.run.bind(simulator),
	};
}
