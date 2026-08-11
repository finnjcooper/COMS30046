import useSimulator from '@hooks/useSimulator';
import Pipeline from './Pipeline';
import Controls from './Controls';
import Config from './Config';
import Loader from './Loader';

export default function Simulator() {
	const { loading } = useSimulator();

	if (loading) return <div className="loading loading-spinner loading-xl"/>;

	return (
		<div className="flex gap-4 w-full px-32">
			<div className="flex-1 space-y-4">
				<Config/>
				<Loader/>
			</div>
			<div className="flex-1 space-y-4">
				<Controls/>
				<Pipeline/>
			</div>
		</div>
	);
}
