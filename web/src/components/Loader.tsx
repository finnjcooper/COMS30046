import { useEffect, useState } from "react";
import { useSimulator, useSimulatorControls } from "@simulator/useSimulator";

const url = (path: string) => `${import.meta.env.BASE_URL}${path.replace(/^\//, '')}`;

export default function Loader() {
	const { snapshot } = useSimulator();
	const { load } = useSimulatorControls();
	const [programs, setPrograms] = useState<Record<string, Uint8Array>>({});
	const [programName, setProgramName] = useState<string | null>(null);

	const disabled = !programName || snapshot.program === programName;

	async function getProgram(name: string): Promise<Record<string, Uint8Array>> {
		const binary = await fetch(url(name + ".elf")).then(res => res.arrayBuffer()).then(buf => new Uint8Array(buf))
		return { [name]: binary };
	}

	useEffect(() => {
		async function fetchPrograms() {
			const programs = await Promise.all([
				getProgram("dot"),
				getProgram("fibonacci"),
				getProgram("gaussian"),
				getProgram("matmul"),
				getProgram("quicksort"),
				getProgram("saxpy")
			]);
			setPrograms(programs.reduce((acc, curr) => ({ ...acc, ...curr }), {}));
		}

		void fetchPrograms();
	}, [])

	return (
		<div className="join w-full">
			<select className="select join-item w-full" defaultValue={""} onChange={(e) => setProgramName(e.target.value)}>
				<option value={""} disabled>Select a program</option>
				<option value={"dot"}>Dot Product</option>
				<option value={"fibonacci"}>Fibonacci</option>
				<option value={"gaussian"}>Gaussian Blur</option>
				<option value={"matmul"}>Matrix Multiplication</option>
				<option value={"quicksort"}>Quicksort</option>
				<option value={"saxpy"}>SAXPY</option>
			</select>
			<button className="btn btn-info join-item" onClick={() => load(programs[programName!], programName!)} disabled={disabled}>
				Load
			</button>
		</div>
	)
}
