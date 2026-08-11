import useSimulator from '@hooks/useSimulator';

export default function Controls() {
	const { snapshot, simulator } = useSimulator();

	return (
		<div className="space-x-4">
			<button className="btn btn-info" onClick={simulator.run} disabled={snapshot.halted}>Run</button>
			<button className="btn btn-primary" onClick={simulator.step} disabled={snapshot.halted}>Step</button>
			<button className="btn btn-error" onClick={simulator.reset}>Reset</button>
		</div>
	)
}
