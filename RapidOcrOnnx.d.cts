
declare interface RapidOcrOnnx {
    readonly version: string;

    padding: number;
    maxSideLen: number;
    boxScoreThresh: number;
    boxThresh: number;
    unClipRatio: number;
    doAngle: boolean;
    mostAngle: boolean;
}
declare class RapidOcrOnnx {
    constructor();

    initModels(detModel: string, clsModel: string, recModel: string, keyPath: string): Promise<boolean>;
    setNumThread(threads: number): void;
    setGpuIndex(gpuIndex: number): void;

    detect(buf: NodeJS.TypedArray): Promise<RapidOcrOnnx.Results>;
    detect(path: string): Promise<RapidOcrOnnx.Results>;

    detectSync(buf: NodeJS.TypedArray): RapidOcrOnnx.Results;
    detectSync(path: string): RapidOcrOnnx.Results;
}
declare namespace RapidOcrOnnx {
    interface Result {
        text: string;
        block: [[number, number], [number, number], [number, number], [number, number]];
        score: number;
    }
    interface Results extends Array<Result> {
    }
}

export = RapidOcrOnnx;
