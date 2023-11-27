import { resolve } from 'path';
import { isTypedArray } from 'util/types';

declare interface RapidOcrOnnxOption {
    detModel: string;
    clsModel: string;
    recModel: string;
    keyPath: string;
    threads?: number;
}

class OCR extends require('./build/Release/RapidOcrOnnx.node').RapidOcrOnnx {
    constructor(option: RapidOcrOnnxOption) {
        super(
            resolve(option.detModel),
            resolve(option.clsModel),
            resolve(option.recModel),
            resolve(option.keyPath),
            option.threads ?? 3,
        );
    }
    detect(buf: NodeJS.TypedArray): Promise<string>;
    detect(path: string): Promise<string>;
    async detect(src: string | NodeJS.TypedArray) {
        if (isTypedArray(src))
            return await super.detect(src);
        return await super.detect(resolve(src));
    }
    detectSync(buf: NodeJS.TypedArray): string;
    detectSync(path: string): string;
    detectSync(src: string | NodeJS.TypedArray) {
        if (isTypedArray(src))
            return super.detectSync(src);
        return super.detectSync(resolve(src));
    }
}

export = OCR;
