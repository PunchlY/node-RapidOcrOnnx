import Fastify from 'fastify';
import OCR from '../ocr.cjs';

const ocr = new OCR();

await ocr.initModels({
    rootDir: 'models',
    detModel: 'ch_PP-OCRv3_det_infer.onnx',
    clsModel: 'ch_ppocr_mobile_v2.0_cls_infer.onnx',
    recModel: 'ch_PP-OCRv3_rec_infer.onnx',
    keyPath: 'dict_chinese.txt',
});

const fastify = Fastify({
    // logger: true,
});

fastify.post('/', async (request, reply) => {
    console.log(request.body);
    // return await ocr.detect('images/1.jpg');
});

await fastify.listen({ port: 3000 });
