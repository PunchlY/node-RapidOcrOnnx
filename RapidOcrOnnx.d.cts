
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

    detect(buf: NodeJS.TypedArray): Promise<any>;
    detect(path: string): Promise<any>;

    detectSync(buf: NodeJS.TypedArray): string;
    detectSync(path: string): string;
}

export = RapidOcrOnnx;
