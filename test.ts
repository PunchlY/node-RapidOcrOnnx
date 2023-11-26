import OCR from './RapidOcrOnnx.cjs';
import fs from 'fs/promises';

setInterval(Function.prototype, -1 >>> 1);

const ocr = new OCR({
    detModel: 'models/ch_PP-OCRv3_det_infer.onnx',
    clsModel: 'models/ch_ppocr_mobile_v2.0_cls_infer.onnx',
    recModel: 'models/ch_PP-OCRv3_rec_infer.onnx',
    keyPath: 'models/ppocr_keys_v1.txt',
});
console.log('---');

// console.log(ocr.detectSync('images/1.jpg'));

ocr.detect('images/1.jpg').then(console.log).catch(console.error);

// ocr.detect(await fs.readFile('images/1.jpg')).then(console.log).catch(console.error);
