import ROB from './ROB';
import Fetch from './Fetch';
import Decode from './Decode';

export default function Pipeline() {
	return (
		<div className="flex flex-col flex-1 gap-4 py-4">
			<div className="grid grid-cols-2 gap-4">
				<Fetch />
				<Decode />
			</div>
			<ROB />
		</div>
	);
}
