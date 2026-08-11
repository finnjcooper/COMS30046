import { useEffect } from 'react';
import useSimulator from '@hooks/useSimulator';

export default function Config() {
	const { simulator } = useSimulator();

	const defaultConfig = {
		"name": "Width 2",
		"pipe_width": 2,
		"rs_size": 8,
		"lsq_size": 8,
		"alu_count": 2,
		"mul_count": 2,
		"ctrl_count": 2,
		"fpu_count": 2,
		"vec_count": 2,
		"lsu_count": 2,
		"branch_pred": "two_bit"
	};

	const handleChange = (event: React.ChangeEvent<HTMLTextAreaElement>) => {
		simulator.configure(event.target.value);
	};

	useEffect(() => {
		simulator.configure(JSON.stringify(defaultConfig, null, 2));
	}, [simulator]);

	return (
		<div>
			<textarea
				className="textarea textarea-bordered w-full h-64 font-mono"
				defaultValue={JSON.stringify(defaultConfig, null, 2)}
				onChange={handleChange}
			/>
		</div>
	);
}
