import { useEffect } from 'react';
import { useSimulatorControls } from '@hooks/useSimulator';

export default function Config() {
	const { configure } = useSimulatorControls();

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
		configure(event.target.value);
	};

	useEffect(() => {
		configure(JSON.stringify(defaultConfig, null, 2));
	}, [configure]);

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
