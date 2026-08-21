import { useSimulator } from '@simulator/useSimulator';

import Controls from './Controls';
import Config from './Config';
import Loader from './Loader';
import Pipeline from './Pipeline';
import Stats from './Stats';

export default function Simulator() {
	const { loading, error } = useSimulator();

	if (error) return <div className="alert alert-error">{String(error)}</div>;
	if (loading) return <div className="loading loading-spinner loading-xl"/>;

	return (
		<div className="grid grid-cols-4 flex-1 h-full w-full gap-4 p-16">
			<div className="col-span-3">
				<h1 className="font-mono">RISC-V Simulator</h1>
				<Pipeline />
			</div>
			<div className="flex flex-col gap-4 col-span-1">
				<div className="flex gap-4">
					<Stats />
					<Controls />
				</div>
				<Loader />
				<Config />
			</div>
		</div>
	);
}
