import { useSimulator } from '@simulator/useSimulator';
import type { FetchState } from '@wasm/sim';

export default function Fetch() {
	const { snapshot } = useSimulator();
	const fetch = Array.from(snapshot.pipeline.fetch_q);

	const render = (s: FetchState) => {
		return (
			<div key={s.pc} className="card card-border grid grid-cols-2 border-secondary bg-base-200 justify-center items-center p-2">
				<p>PC:</p><p>0x{s.pc.toString(16).padStart(8, '0')}</p>
				<p>Instruction:</p><p>0x{s.instr.toString(16).padStart(8, '0')}</p>
			</div>
		)
	}

	return (
		<div className="space-y-4 font-mono">
			<h2>Fetch Queue</h2>
			<div className="grid grid-cols-2 gap-2">
				{!fetch.length ? <p className="h-14">Empty</p> : fetch.map(render)}
			</div>
		</div>
	);
}
