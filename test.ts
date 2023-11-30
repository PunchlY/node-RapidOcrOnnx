import OCR from './ocr.cjs';
import fs from 'fs/promises';

// setInterval(Function.prototype, -1 >>> 1);

const ocr = new OCR();
await ocr.initModels({
    rootDir: 'models',
    detModel: 'ch_PP-OCRv3_det_infer.onnx',
    clsModel: 'ch_ppocr_mobile_v2.0_cls_infer.onnx',
    recModel: 'ch_PP-OCRv3_rec_infer.onnx',
    keyPath: 'dict_chinese.txt',
});
// await ocr.initModels({
//     rootDir: 'models',
//     detModel: 'ch_PP-OCRv4_det_infer.onnx',
//     clsModel: 'ch_ppocr_mobile_v2.0_cls_infer.onnx',
//     recModel: 'rec_ch_PP-OCRv4_infer.onnx',
//     keyPath: 'dict_chinese.txt',
// });
console.log(ocr.version);
// console.log(ocr.detectSync('images/1.jpg'));

ocr.detect('images/1.jpg').then(JSON.stringify).then(console.log).catch(console.error);

// ocr.detect(await fs.readFile('images/1.jpg')).then(console.log).catch(console.error);
