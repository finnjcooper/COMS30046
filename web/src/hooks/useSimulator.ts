import { useEffect, useState, useSyncExternalStore } from 'react';
import { simulator } from '@hooks/simulator';

export default function useSimulator() {
	const [loading, setLoading] = useState(true);

	useEffect(() => {
		simulator.init().finally(() => setLoading(false));
	}, []);

	const snapshot = useSyncExternalStore(simulator.subscribe, simulator.snapshot);

	return {
		loading,
		snapshot,
		simulator: simulator
	};
}

export const url = (path: string) => `${import.meta.env.BASE_URL}${path.replace(/^\//, '')}`;
