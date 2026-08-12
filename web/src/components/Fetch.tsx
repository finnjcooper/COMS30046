import { useSimulator } from '@hooks/useSimulator';
import type { FetchState } from '@wasm/sim';

export default function Fetch() {
	const { snapshot } = useSimulator();
	const fetch = snapshot.pipeline.fetch_q;

	const render = (s: FetchState) => {
		return (
			<div key={s.pc} className="card card-border grid grid-cols-2 border-secondary bg-base-200 justify-center items-center p-2">
				<p>PC:</p><p>0x{s.pc.toString(16).padStart(8, '0')}</p>
				<p>Instruction:</p><p>0x{s.instr.toString(16).padStart(8, '0')}</p>
			</div>
		)
	}

	return (
		<div className="space-y-4">
			<h2>Fetch</h2>
			<div className="grid grid-cols-2 gap-2">
				{Array.from(fetch).map(render)}
			</div>
		</div>
	);
}
