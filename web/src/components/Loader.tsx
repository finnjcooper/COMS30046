import { useEffect, useState } from "react";
import { useSimulatorControls } from "@hooks/useSimulator";

const url = (path: string) => `${import.meta.env.BASE_URL}${path.replace(/^\//, '')}`;

export default function Loader() {
	const { load } = useSimulatorControls();
	const [programs, setPrograms] = useState<Record<string, Uint8Array>>({});
	const [programName, setProgramName] = useState<string | null>(null);

	async function getProgram(name: string): Promise<Record<string, Uint8Array>> {
		const binary = await fetch(url(name)).then(res => res.arrayBuffer()).then(buf => new Uint8Array(buf))
		return { [name]: binary };
	}

	useEffect(() => {
		async function fetchPrograms() {
			const programs = await Promise.all([
				getProgram("dot.elf"),
				getProgram("fibonacci.elf"),
				getProgram("gaussian.elf"),
				getProgram("matmul.elf"),
				getProgram("quicksort.elf"),
				getProgram("saxpy.elf")
			]);
			setPrograms(programs.reduce((acc, curr) => ({ ...acc, ...curr }), {}));
		}

		void fetchPrograms();
	}, [])

	return (
		<div className="join w-full">
			<select className="select join-item w-full" defaultValue={""} onChange={(e) => {setProgramName(e.target.value);}}>
				<option value={""} disabled>Select a program</option>
				<option value={"dot.elf"}>Dot Product</option>
				<option value={"fibonacci.elf"}>Fibonacci</option>
				<option value={"gaussian.elf"}>Gaussian Blur</option>
				<option value={"matmul.elf"}>Matrix Multiplication</option>
				<option value={"quicksort.elf"}>Quicksort</option>
				<option value={"saxpy.elf"}>SAXPY</option>
			</select>
			<button className="btn btn-info join-item" onClick={() => load(programs[programName!], programName!)} disabled={!programName}>
				Load
			</button>
		</div>
	)
}
