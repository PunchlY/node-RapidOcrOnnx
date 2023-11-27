
declare interface RapidOcrOnnx {
    padding: number;
    maxSideLen: number;
    boxScoreThresh: number;
    boxThresh: number;
    unClipRatio: number;
    doAngle: boolean;
    mostAngle: boolean;
}
declare class RapidOcrOnnx {
    constructor(detModel: string, clsModel: string, recModel: string, keyPath: string, threads: number);

    detect(buf: NodeJS.TypedArray): Promise<string>;
    detect(path: string): Promise<string>;

    detectSync(buf: NodeJS.TypedArray): string;
    detectSync(path: string): string;
}

export { RapidOcrOnnx };
