import { StrictMode } from 'react'
import { createRoot } from 'react-dom/client'
import '@styles/global.css'
import Simulator from '@components/Simulator.tsx';

createRoot(document.getElementById('root')!).render(
	<StrictMode>
		<div className="w-screen h-screen flex flex-col justify-center items-center">
			<Simulator />
		</div>
	</StrictMode>,
)
