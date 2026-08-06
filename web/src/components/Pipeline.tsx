import useSimulator from '@hooks/useSimulator';

export default function Pipeline() {
	const { snapshot } = useSimulator();

	return (
		<div className="font-mono grid grid-cols-2 gap-x-4 text-right">
			<p className="font-bold text-primary text-left">Program Counter:</p><p>0x{snapshot.pc.toString(16).padStart(8, '0')}</p>
			<p className="font-bold text-primary text-left">Instructions:</p><p>{snapshot.stats.instructionCount}</p>
			<p className="font-bold text-primary text-left">Cycles:</p><p>{snapshot.stats.cycleCount}</p>
			<p className="font-bold text-primary text-left">Branches:</p><p>{snapshot.stats.branchCount}</p>
			<p className="font-bold text-primary text-left">Mispredictions:</p><p>{snapshot.stats.mispredCount}</p>
		</div>
	);
}
