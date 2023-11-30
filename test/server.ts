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
    if (request.body instanceof Buffer)
        return await ocr.detect(request.body);
    reply.code(400);
});

fastify.addContentTypeParser('*', { parseAs: 'buffer' }, async (request: any, payload: Buffer) => {
    return payload;
});

await fastify.listen({ port: 3000 });
