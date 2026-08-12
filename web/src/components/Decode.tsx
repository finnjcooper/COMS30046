import { useSimulator } from '@hooks/useSimulator';
import { opName } from '@hooks/utils';
import type { DecodeState } from '@wasm/sim';

export default function Decode() {
	const { snapshot } = useSimulator();
	const decode = snapshot.pipeline.decode_q;

	const render = (s: DecodeState) => {
		return (
			<div key={s.pc} className="card card-border grid grid-cols-2 border-secondary bg-base-200 justify-center items-center p-2">
				<p>PC:</p><p>0x{s.pc.toString(16).padStart(8, '0')}</p>
				<p>Op:</p><p>{opName(s.op)}</p>
				<p>Predicted:</p><p>{s.predTaken ? 'Taken' : 'Not Taken'}</p>
				<p>Target:</p><p>0x{s.predTarget.toString(16).padStart(8, '0')}</p>
			</div>
		)
	}

	return (
		<div className="space-y-4">
			<h2>Decode</h2>
			<div className="grid grid-cols-2 gap-2">
				{Array.from(decode).map(render)}
			</div>
		</div>
	);
}
