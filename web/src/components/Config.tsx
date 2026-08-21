import { useEffect, useState } from 'react';
import { useSimulatorControls } from '@simulator/useSimulator';
import { type Config } from '@wasm/sim';

export default function Config() {
	const { configure } = useSimulatorControls();

	const defaultConfig: Config = {
		name: "Default",
		pipe_width: 2,
		rs_size: 8,
		lsq_size: 8,
		alu_count: 2,
		mul_count: 2,
		ctrl_count: 2,
		fpu_count: 2,
		vec_count: 2,
		lsu_count: 2,
		vector_bits: 128,
		branch_pred: { value: 3 } // two_bit
	};

	const [config, setConfig] = useState<Config>(defaultConfig);
	useEffect(handleSubmit, [configure])

	function handleSubmit() {
		configure(config);
	}

	function toString({ value }: { value: number}): string {
		switch(value) {
			case 0: return "Always Taken";
			case 1: return "Always Not Taken";
			case 2: return "One Bit Counter";
			case 3: return "Two Bit Counter";
			case 4: return "BTFNT";
			default: return "Unknown";
		}
	}

	return (
		<div className="flex flex-col gap-2">
			{Object.keys(config).map((key, index) => {
				if (index === 0) return null;

				let value: string | number | { value: number } = (config as any)[key];
				const isNumber = typeof value === 'number';
				const isString = typeof value === 'string';

				return (
					<label className="input w-full" key={index}>
						<span className="label">{key.toLocaleUpperCase()}</span>
						{isNumber &&
							<input
								className="text-right"
								type="number"
								value={value}
								onChange={(e) => setConfig({...config, [key]: parseInt(e.target.value)})}
							/>
						}
						{isString &&
							<input
								className="text-right"
								type="text"
								value={value}
								onChange={(e) => setConfig({...config, [key]: e.target.value})}
							/>
						}
						{!isNumber && !isString &&
							<select className="text-right w-full" defaultValue={value.value} onChange={(e) => setConfig({...config, [key]: {value: e.target.value}})}>
								<option value={0}>{toString({value: 0})}</option>
								<option value={1}>{toString({value: 1})}</option>
								<option value={2}>{toString({value: 2})}</option>
								<option value={3}>{toString({value: 3})}</option>
								<option value={4}>{toString({value: 4})}</option>
							</select>
						}
					</label>
				)
			})}
			<button type="button" className="btn btn-info" onClick={handleSubmit}>Submit</button>
		</div>
	);
}
