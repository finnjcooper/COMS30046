import { useSimulator, useSimulatorControls } from '@simulator/useSimulator';

export default function Controls() {
	const { run, step, reset } = useSimulatorControls();
	const disabled = useSimulator().snapshot.halted;

	return (
		<div className="flex flex-col flex-wrap gap-2 justify-end">
			<button className="btn btn-info" onClick={run} disabled={disabled}>Run</button>
			<button className="btn btn-primary" onClick={step} disabled={disabled}>Step</button>
			<button className="btn btn-error" onClick={reset}>Reset</button>
		</div>
	)
}
