import { resolve } from 'path';
import { isTypedArray } from 'util/types';
import RapidOcrOnnx from 'RapidOcrOnnx';

interface Models {
    rootDir?: string;
    detModel: string;
    clsModel: string;
    recModel: string;
    keyPath: string;
}

class OCR extends RapidOcrOnnx {
    initModels(models: Models): Promise<boolean>;
    initModels(detModel: string, clsModel: string, recModel: string, keyPath: string): Promise<boolean>;
    async initModels() {
        if (arguments.length >= 4) return await super.initModels(
            resolve(arguments[0]),
            resolve(arguments[1]),
            resolve(arguments[2]),
            resolve(arguments[3])
        );

        const models: Models = arguments[0];
        const rootDir = models.rootDir ?? '';
        return await super.initModels(
            resolve(rootDir, models.detModel),
            resolve(rootDir, models.clsModel),
            resolve(rootDir, models.recModel),
            resolve(rootDir, models.keyPath)
        );
    }

    initModelsSync(models: Models): boolean;
    initModelsSync(detModel: string, clsModel: string, recModel: string, keyPath: string): boolean;
    initModelsSync() {
        if (arguments.length >= 4) return super.initModelsSync(
            resolve(arguments[0]),
            resolve(arguments[1]),
            resolve(arguments[2]),
            resolve(arguments[3])
        );

        const models: Models = arguments[0];
        const rootDir = models.rootDir ?? '';
        return super.initModelsSync(
            resolve(rootDir, models.detModel),
            resolve(rootDir, models.clsModel),
            resolve(rootDir, models.recModel),
            resolve(rootDir, models.keyPath)
        );
    }

    detect(buf: NodeJS.TypedArray): Promise<RapidOcrOnnx.Results>;
    detect(path: string): Promise<RapidOcrOnnx.Results>;
    async detect(src: string | NodeJS.TypedArray) {
        if (isTypedArray(src))
            return await super.detect(src);
        return await super.detect(resolve(src));
    }

    detectSync(buf: NodeJS.TypedArray): RapidOcrOnnx.Results;
    detectSync(path: string): RapidOcrOnnx.Results;
    detectSync(src: string | NodeJS.TypedArray) {
        if (isTypedArray(src))
            return super.detectSync(src);
        return super.detectSync(resolve(src));
    }
}

namespace OCR {
    export type Result = RapidOcrOnnx.Result;
    export type Results = RapidOcrOnnx.Results;
}

export = OCR;
