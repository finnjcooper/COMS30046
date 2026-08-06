import useSimulator from '@hooks/useSimulator';

export default function Controls() {
	const { running, step, run } = useSimulator();

	return (
		<div className="space-x-4">
			<button className="btn btn-info" onClick={run} disabled={!running()}>Run</button>
			<button className="btn btn-primary" onClick={step} disabled={!running()}>Step</button>
			<button className="btn btn-error" onClick={() => window.location.reload()}>Reset</button>
		</div>
	)
}
