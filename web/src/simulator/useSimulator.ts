import { useCallback, useEffect, useState, useSyncExternalStore } from 'react';
import { simulator } from './simulator';
import type { Config } from '@wasm/sim';

export function useSimulator() {
	const [loading, setLoading] = useState(true);
	const [error, setError] = useState<unknown>(null);

	useEffect(() => {
		simulator.init()
		.catch(setError)
		.finally(() => setLoading(false));
	}, []);

	const snapshot = useSyncExternalStore(simulator.subscribe, () => simulator.snapshot()!, () => simulator.snapshot()!);

	return {
		loading,
		error,
		snapshot
	};
}

export function useSimulatorControls() {
	const load = useCallback(
		(program: Uint8Array, name: string) => simulator.load(program, name),
	[]);

	const configure = useCallback(
		(config: Config) => simulator.configure(config),
	[]);

	const reset = useCallback(
		() => simulator.reset(),
	[]);

	const step = useCallback(
		() => simulator.step(),
	[]);

	const run = useCallback(
		() => simulator.run(),
	[]);

	return {
		load,
		configure,
		reset,
		step,
		run
	}
}
