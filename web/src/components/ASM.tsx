import { useEffect, useState } from "react";
import { useSimulator } from "@simulator/useSimulator";

const url = (path: string) => `${import.meta.env.BASE_URL}${path.replace(/^\//, '')}`;

export default function ASM() {
	const { snapshot: { program, pc } } = useSimulator();
	const [asm, setAsm] = useState<string | null>(null);

	useEffect(() => {
		if (!program) return;

		async function getAsm() {
			const res = await fetch(url(program + ".asm")).then(res => res.text());
			setAsm(res);
		}

		void getAsm();
	}, [program])


	function render(asm: string) {
		const disasm = new Map<number, string>();

		for (const line of asm.split(/\r?\n/)) {
			const m = line.match(/^\s*([0-9a-fA-F]+):\s+\S+\s+(.+)$/);
			if (m) disasm.set(parseInt(m[1], 16), m[2]);
		}

		return (<>{Array.from(disasm).map(([key, val]) => {
			if (key === pc) return <p className="text-red-500">{key.toString(16)}: {val}</p>
			return <p>{key.toString(16)}: {val}</p>
		})}</>)
	}

	return (
		<div className="card card-border border-secondary bg-base-200 p-4 flex-1">
			{asm && render(asm)}
		</div>
	)
}
