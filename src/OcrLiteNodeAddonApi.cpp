#include "OcrLite.h"
#include "OcrResultUtils.h"
#include "OcrUtils.h"
#include "version.h"
#include <fstream>
#include <napi.h>
#include <opencv2/imgcodecs.hpp>

class RapidOcrOnnx : public Napi::ObjectWrap<RapidOcrOnnx> {
public:
    static Napi::Function Init(Napi::Env env);
    RapidOcrOnnx(const Napi::CallbackInfo& info);
    ~RapidOcrOnnx();

    OcrResult Detect(std::string& imgFile);
    OcrResult Detect(char* buffer, size_t sz);
    static Napi::Value OcrResultToJSON(Napi::Env env, OcrResult& ocrResult);
    static Napi::Value OcrResultToString(Napi::Env env, OcrResult& result);

private:
    OcrLite* ocrLite;

    Napi::Value version(const Napi::CallbackInfo& info)
    {
        return Napi::String::New(info.Env(), VERSION);
    }
    Property(int, padding, 50);
    Property(int, maxSideLen, 1024);
    Property(float, boxScoreThresh, 0.6);
    Property(float, boxThresh, 0.6);
    Property(float, unClipRatio, 0.6);
    Property(bool, doAngle, true);
    Property(bool, mostAngle, true);

    void setNumThread(const Napi::CallbackInfo& info);
    void setGpuIndex(const Napi::CallbackInfo& info);
    Napi::Value initModels(const Napi::CallbackInfo& info);
    Napi::Value initModelsSync(const Napi::CallbackInfo& info);
    Napi::Value detect(const Napi::CallbackInfo& info);
    Napi::Value detectSync(const Napi::CallbackInfo& info);
};

Napi::Function RapidOcrOnnx::Init(Napi::Env env)
{
    return DefineClass(env, "RapidOcrOnnx",
        {
            InstanceAccessor<&RapidOcrOnnx::version>("version"),
            Instance_Method(RapidOcrOnnx, setNumThread),
            Instance_Method(RapidOcrOnnx, setGpuIndex),
            Instance_Method(RapidOcrOnnx, initModels),
            Instance_Method(RapidOcrOnnx, initModelsSync),
            Instance_Method(RapidOcrOnnx, detect),
            Instance_Method(RapidOcrOnnx, detectSync),
            Instance_Property(RapidOcrOnnx, padding),
            Instance_Property(RapidOcrOnnx, maxSideLen),
            Instance_Property(RapidOcrOnnx, boxScoreThresh),
            Instance_Property(RapidOcrOnnx, boxThresh),
            Instance_Property(RapidOcrOnnx, unClipRatio),
            Instance_Property(RapidOcrOnnx, doAngle),
            Instance_Property(RapidOcrOnnx, mostAngle),
        });
}

RapidOcrOnnx::RapidOcrOnnx(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<RapidOcrOnnx>(info)
{
    ocrLite = new OcrLite;
}
RapidOcrOnnx::~RapidOcrOnnx()
{
    if (ocrLite)
        delete ocrLite;
}

void RapidOcrOnnx::setNumThread(const Napi::CallbackInfo& info)
{
    int nThreads = info[0].ToNumber().Int32Value();
    ocrLite->setNumThread(nThreads);
}
void RapidOcrOnnx::setGpuIndex(const Napi::CallbackInfo& info)
{
    int gpuIndex = info[0].ToNumber().Int32Value();
    ocrLite->setGpuIndex(gpuIndex);
}

class InitModelsWorker : public PromiseWorker {
public:
    InitModelsWorker(Napi::Env& env, OcrLite* ocrLite, const std::string& detPath, const std::string& clsPath, const std::string& recPath, const std::string& keysPath)
        : PromiseWorker(env)
        , ocrLite(ocrLite)
        , szDetModel(detPath)
        , szClsModel(clsPath)
        , szRecModel(recPath)
        , szKeyPath(keysPath)
    {
    }
    void Execute() override
    {
        result = ocrLite->initModels(szDetModel, szClsModel, szRecModel, szKeyPath);
    }
    void OnOK() override
    {
        Deferred().Resolve(Napi::Boolean::New(Env(), result));
    }

private:
    OcrLite* ocrLite;
    std::string szDetModel;
    std::string szClsModel;
    std::string szRecModel;
    std::string szKeyPath;

    bool result;
};

Napi::Value RapidOcrOnnx::initModels(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();

    std::string szDetModel = info[0].As<Napi::String>().Utf8Value();
    std::string szClsModel = info[1].As<Napi::String>().Utf8Value();
    std::string szRecModel = info[2].As<Napi::String>().Utf8Value();
    std::string szKeyPath = info[3].As<Napi::String>().Utf8Value();

    InitModelsWorker* worker = new InitModelsWorker(env, ocrLite, szDetModel, szClsModel, szRecModel, szKeyPath);
    worker->Queue();
    return worker->Promise();
}
Napi::Value RapidOcrOnnx::initModelsSync(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();

    std::string szDetModel = info[0].As<Napi::String>().Utf8Value();
    std::string szClsModel = info[1].As<Napi::String>().Utf8Value();
    std::string szRecModel = info[2].As<Napi::String>().Utf8Value();
    std::string szKeyPath = info[3].As<Napi::String>().Utf8Value();

    bool result = ocrLite->initModels(szDetModel, szClsModel, szRecModel, szKeyPath);
    return Napi::Boolean::New(env, result);
}

OcrResult RapidOcrOnnx::Detect(std::string& imgFile)
{
#ifdef _WIN32
    FILE* fp = _wfopen(strToWstr(imgFile).c_str(), L"rb");
#else
    FILE* fp = fopen(imgFile.c_str(), "rb");
#endif
    fseek(fp, 0, SEEK_END);
    size_t size = ftell(fp);
    char* buffer = new char[size];
    fseek(fp, 0, SEEK_SET);
    fread(buffer, 1, size, fp);

    OcrResult result = Detect(buffer, size);

    delete[] buffer;
    fclose(fp);

    return result;
}
OcrResult RapidOcrOnnx::Detect(char* buffer, size_t size)
{
    cv::_InputArray arr(buffer, size);
    cv::Mat originSrc = cv::imdecode(arr, cv::IMREAD_COLOR);

    OcrResult result
        = ocrLite->detect(originSrc, padding, maxSideLen, boxScoreThresh, boxThresh, unClipRatio, doAngle, mostAngle);

    return result;
}

Napi::Value RapidOcrOnnx::OcrResultToJSON(Napi::Env env, OcrResult& result)
{
    Napi::Array arr = Napi::Array::New(env);
    unsigned int i = 0;
    for (auto& textBlock : result.textBlocks) {
        Napi::Object block = Napi::Object::New(env);
        Napi::Array boxes = Napi::Array::New(env);
        block["text"] = textBlock.text;
        block["box"] = boxes;
        block["score"] = textBlock.boxScore;
        for (int i_box = 0; i_box < 4; i_box++) {
            Napi::Array box = Napi::Array::New(env);
            boxes[i_box] = box;
            box[(unsigned int)0] = textBlock.boxPoint[i_box].x;
            box[1] = textBlock.boxPoint[i_box].y;
        }
        arr[i++] = block;
    }
    return arr;
}
Napi::Value RapidOcrOnnx::OcrResultToString(Napi::Env env, OcrResult& result)
{
    std::string strRes;
    for (auto& textBlock : result.textBlocks) {
        strRes.append(textBlock.text);
        strRes.append("\n");
    }
    return Napi::String::New(env, strRes);
}

class DetectWorker : public PromiseWorker {
public:
    DetectWorker(Napi::Env& env, RapidOcrOnnx* obj, const std::string& imgFile)
        : PromiseWorker(env)
        , obj(obj)
        , imgFile(imgFile)
    {
    }
    DetectWorker(Napi::Env& env, RapidOcrOnnx* obj, char* data, size_t sz)
        : PromiseWorker(env)
        , obj(obj)
        , data(data)
        , sz(sz)
    {
    }
    void Execute() override
    {
        if (data)
            result = obj->Detect(data, sz);
        else
            result = obj->Detect(imgFile);
    }
    void OnOK() override
    {
        Deferred().Resolve(RapidOcrOnnx::OcrResultToJSON(Env(), result));
    }

private:
    RapidOcrOnnx* obj;
    std::string imgFile;
    char* data = nullptr;
    size_t sz;

    OcrResult result;
};

Napi::Value RapidOcrOnnx::detect(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    DetectWorker* worker;
    if (info[0].IsTypedArray()) {
        Napi::Uint8Array arr = info[0].As<Napi::Uint8Array>();
        worker = new DetectWorker(env, this, (char*)arr.ArrayBuffer().Data() + arr.ByteOffset(), arr.ByteLength());
    } else {
        std::string imgFile = info[0].As<Napi::String>().Utf8Value();
        worker = new DetectWorker(env, this, imgFile);
    }
    worker->Queue();
    return worker->Promise();
}
Napi::Value RapidOcrOnnx::detectSync(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    OcrResult result;
    if (info[0].IsTypedArray()) {
        Napi::Uint8Array arr = info[0].As<Napi::Uint8Array>();
        result = Detect((char*)arr.ArrayBuffer().Data() + arr.ByteOffset(), arr.ByteLength());
    } else {
        std::string imgFile = info[0].As<Napi::String>().Utf8Value();
        result = Detect(imgFile);
    }

    return OcrResultToJSON(env, result);
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    return RapidOcrOnnx::Init(env);
}

NODE_API_MODULE(RapidOcrOnnx, Init);
