export type SimulatorInstance = Awaited<ReturnType<typeof useSimulator>>;

export default async function useSimulator() {
	const moduleUrl = new URL('/sim.mjs', import.meta.url).href;
	const { default: createModule } = await import(/* @vite-ignore */ moduleUrl);
	const Module = await createModule();
	return new Module.Simulator();
}
