import { useSimulator } from "@simulator/useSimulator";

export default function Stats() {
	const { snapshot } = useSimulator();
	return (
		<div className="card card-border grid grid-cols-2 border-secondary bg-base-200 justify-center items-center font-mono text-right flex-1 p-4">
			<p className="font-extrabold text-left">Cycles:</p><p>{snapshot.stats.cycleCount}</p>
			<p className="font-extrabold text-left">Instructions:</p><p>{snapshot.stats.instructionCount}</p>
			<p className="font-extrabold text-left">IPC:</p><p>{(snapshot.stats.instructionCount / snapshot.stats.cycleCount).toFixed(2)}</p>
		</div>
	)
}
