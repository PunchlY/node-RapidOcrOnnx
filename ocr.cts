import { resolve } from 'path';
import { isTypedArray } from 'util/types';
import { RapidOcrOnnx, toString, toJSON } from 'RapidOcrOnnx';

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
    detect(buf: NodeJS.TypedArray): Promise<string>;
    detect(path: string): Promise<string>;
    async detect(src: string | NodeJS.TypedArray) {
        if (isTypedArray(src))
            return toString(await super.detect(src));
        return toString(await super.detect(resolve(src)));
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
