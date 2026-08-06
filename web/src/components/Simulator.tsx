import { useEffect } from 'react';
import useSimulator from '@hooks/useSimulator';
import Pipeline from './Pipeline';
import Controls from './Controls';
import Config from './Config';

export default function Simulator() {
	const url = (path: string) => `${import.meta.env.BASE_URL}${path.replace(/^\//, '')}`;
	const { loading, load, configure } = useSimulator();

	useEffect(() => {
		async function fetchStuff() {
			const [binary, config] = await Promise.all([
				fetch(url('fibonacci.elf')).then(res => res.arrayBuffer()).then(buf => new Uint8Array(buf)),
				fetch(url('config.json')).then(res => res.text()),
			]);

			configure(config);
			load(binary);
		}

		if (loading) return;
		void fetchStuff();
	}, [loading]);

	if (loading) return <div className="loading loading-spinner loading-xl"/>;

	return (
		<div className="flex gap-4 w-full px-32">
			<div className="flex-1">
				<Config/>
			</div>
			<div className="space-y-4">
				<Controls/>
				<Pipeline/>
			</div>
		</div>
	);
}
