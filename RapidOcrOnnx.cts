import { resolve } from 'path';
import { isTypedArray } from 'util/types';

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

    detect(path: string): Promise<string>;
    detect(buf: NodeJS.TypedArray): Promise<string>;

    detectSync(path: string): string;
    detectSync(buf: NodeJS.TypedArray): string;
}

declare interface RapidOcrOnnxOption {
    detModel: string;
    clsModel: string;
    recModel: string;
    keyPath: string;
    threads?: number;
}

class OCR extends (require('./build/Release/RapidOcrOnnx.node') as typeof RapidOcrOnnx) {
    constructor(option: RapidOcrOnnxOption) {
        super(
            resolve(option.detModel),
            resolve(option.clsModel),
            resolve(option.recModel),
            resolve(option.keyPath),
            option.threads ?? 3,
        );
    }
    detect(path: string): Promise<string>;
    detect(buf: NodeJS.TypedArray): Promise<string>;
    async detect(src: string | NodeJS.TypedArray) {
        if (isTypedArray(src))
            return await super.detect(src);
        return await super.detect(resolve(src));
    }
    detectSync(path: string): string;
    detectSync(buf: NodeJS.TypedArray): string;
    detectSync(src: string | NodeJS.TypedArray) {
        if (isTypedArray(src))
            return super.detectSync(src);
        return super.detectSync(resolve(src));
    }
}

export = OCR;
