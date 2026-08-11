import { useEffect, useState } from "react";
import useSimulator, { url } from "@hooks/useSimulator";



export default function Loader() {
	const { snapshot, simulator } = useSimulator();
	const [programs, setPrograms] = useState<Record<string, Uint8Array>>({});
	const [programName, setProgramName] = useState<string | null>(null);

	async function getProgram(name: string): Promise<Record<string, Uint8Array>> {
		const binary = await fetch(url(name)).then(res => res.arrayBuffer()).then(buf => new Uint8Array(buf))
		return { [name]: binary };
	}

	useEffect(() => {
		async function fetchPrograms() {
			const programs = await Promise.all([
				getProgram("fibonacci.elf"),
				getProgram("dot.elf")
			]);
			setPrograms(programs.reduce((acc, curr) => ({ ...acc, ...curr }), {}));
		}

		void fetchPrograms();
	}, [])

	return (
		<div>
			<p>Loaded: {snapshot.program.toString()}</p>
			<div className="join w-full">
				<select className="select join-item" defaultValue={""} onChange={(e) => {setProgramName(e.target.value);}}>
					<option value={""} disabled>Select a program</option>
					<option value={"fibonacci.elf"}>Fibonacci</option>
					<option value={"dot.elf"}>Dot Product</option>
				</select>
				<button className="btn btn-info join-item" onClick={() => simulator.load(programs[programName!], programName!)} disabled={!programName}>
					Load
				</button>
			</div>
		</div>
	)
}
