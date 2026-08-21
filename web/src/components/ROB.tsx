import { useSimulator } from '@simulator/useSimulator';
import type { ROBState } from '@wasm/sim';
import { name } from '@simulator/utils';

export default function ROB() {
	const { snapshot } = useSimulator();
	const rob = Array.from(snapshot.pipeline.rob);


	const render = (s: ROBState) => {
		return (
			<div key={s.tag} className="card card-border border-secondary bg-base-200 justify-center items-center p-2">
				<p className="font-bold">#{s.tag}</p>
				<p>{name(s.op)}</p>
				{/* <p>0x{s.pc.toString(16).padStart(8, '0')}</p> */}
			</div>
		)
	}

	return (
		<div className="space-y-4 font-mono">
			<h2>Re-order Buffer</h2>
			<div className="grid grid-cols-8 gap-2">
				{!rob.length ? <p>Empty</p> : rob.map(render)}
			</div>
		</div>
	);
}
