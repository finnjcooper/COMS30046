import { useSimulator } from '@simulator/useSimulator';
import { name } from '@simulator/utils';
import type { DecodeState } from '@wasm/sim';

export default function Decode() {
	const { snapshot } = useSimulator();
	const decode = Array.from(snapshot.pipeline.decode_q);

	const render = (s: DecodeState) => {
		return (
			<div key={s.pc} className="card card-border grid grid-cols-2 border-secondary bg-base-200 justify-center items-center p-2">
				<p>PC:</p><p>0x{s.pc.toString(16).padStart(8, '0')}</p>
				<p>Op:</p><p>{name(s.op)}</p>
				<p>Predicted:</p><p>{s.predTaken ? 'Taken' : 'Not Taken'}</p>
				<p>Target:</p><p>0x{s.predTarget.toString(16).padStart(8, '0')}</p>
			</div>
		)
	}

	return (
		<div className="space-y-4 font-mono">
			<h2>Decode Queue</h2>
			<div className="grid grid-cols-2 gap-2">
				{!decode.length ? <p className="h-[114px]">Empty</p> : decode.map(render)}
			</div>
		</div>
	);
}
