import { useSimulatorControls } from '@hooks/useSimulator';

export default function Controls() {
	const { run, step, reset } = useSimulatorControls();

	return (
		<div className="flex flex-wrap gap-2 justify-end">
			<button className="btn btn-info" onClick={run}>Run</button>
			<button className="btn btn-primary" onClick={step}>Step</button>
			<button className="btn btn-error" onClick={reset}>Reset</button>
		</div>
	)
}
