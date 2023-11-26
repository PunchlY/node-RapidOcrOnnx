import Path from 'path';

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
    detect(imgFile: string): Promise<string>;
    detectSync(imgFile: string): string;
}

export = class OCR extends (require('./build/Release/RapidOcrOnnx.node') as typeof RapidOcrOnnx) {
    constructor() {
        super(Path.resolve('models/ch_PP-OCRv3_det_infer.onnx'), Path.resolve('models/ch_ppocr_mobile_v2.0_cls_infer.onnx'), Path.resolve('models/ch_PP-OCRv3_rec_infer.onnx'), Path.resolve('models/ppocr_keys_v1.txt'), 3);
    }
    detect(imgFile: string) {
        return super.detect(Path.resolve(imgFile));
    }
    detectSync(imgFile: string) {
        return super.detectSync(Path.resolve(imgFile));
    }
};
