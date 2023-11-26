#include "OcrLite.h"
#include "tools.h"
#include <napi.h>
#include <opencv2/imgcodecs.hpp>

class RapidOcrOnnx : public Napi::ObjectWrap<RapidOcrOnnx> {
public:
    static Napi::Object Init(Napi::Env env);
    RapidOcrOnnx(const Napi::CallbackInfo& info);
    ~RapidOcrOnnx();

    OcrResult Detect(std::string imgFile);
    OcrResult Detect(void* buf, size_t sz);

private:
    OcrLite* ocrLite;

    Property(int, padding, 50);
    Property(int, maxSideLen, 1024);
    Property(float, boxScoreThresh, 0.6);
    Property(float, boxThresh, 0.6);
    Property(float, unClipRatio, 0.6);
    Property(bool, doAngle, true);
    Property(bool, mostAngle, true);

    Napi::Value detect(const Napi::CallbackInfo& info);
    Napi::Value detectSync(const Napi::CallbackInfo& info);
};

Napi::Object RapidOcrOnnx::Init(Napi::Env env)
{
    return DefineClass(env, "RapidOcrOnnx",
        {
            Instance_Method(detect),
            Instance_Method(detectSync),
            Instance_Property(padding),
            Instance_Property(maxSideLen),
            Instance_Property(boxScoreThresh),
            Instance_Property(boxThresh),
            Instance_Property(unClipRatio),
            Instance_Property(doAngle),
            Instance_Property(mostAngle),
        });
}

OcrResult RapidOcrOnnx::Detect(std::string imgFile)
{
    cv::Mat originSrc = cv::imread(imgFile, cv::IMREAD_COLOR);

    OcrResult result
        = ocrLite->detect(originSrc, padding, maxSideLen, boxScoreThresh, boxThresh, unClipRatio, doAngle, mostAngle);
    result.strRes[result.strRes.length() - 1] = 0;

    return result;
}
OcrResult RapidOcrOnnx::Detect(void* buf, size_t sz)
{
    cv::_InputArray arr((char*)buf, sz);
    cv::Mat originSrc = cv::imdecode(arr, cv::IMREAD_COLOR);

    OcrResult result
        = ocrLite->detect(originSrc, padding, maxSideLen, boxScoreThresh, boxThresh, unClipRatio, doAngle, mostAngle);
    result.strRes[result.strRes.length() - 1] = 0;

    return result;
}

RapidOcrOnnx::RapidOcrOnnx(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<RapidOcrOnnx>(info)
{
    ocrLite = new OcrLite;

    std::string szDetModel = info[0].As<Napi::String>().Utf8Value();
    std::string szClsModel = info[1].As<Napi::String>().Utf8Value();
    std::string szRecModel = info[2].As<Napi::String>().Utf8Value();
    std::string szKeyPath = info[3].As<Napi::String>().Utf8Value();
    int nThreads = info[4].ToNumber().Int32Value();

    ocrLite->setNumThread(nThreads);
    ocrLite->initModels(szDetModel, szClsModel, szRecModel, szKeyPath);
}
RapidOcrOnnx::~RapidOcrOnnx()
{
    if (ocrLite)
        delete ocrLite;
}

class DetectWorker : public Napi::AsyncWorker {
public:
    DetectWorker(Napi::Env& env, RapidOcrOnnx* obj, std::string& imgFile, Napi::Promise::Deferred deferred)
        : AsyncWorker(env)
        , obj(obj)
        , imgFile(imgFile)
        , deferred(deferred)
    {
    }
    ~DetectWorker() { }
    void Execute() override { result = obj->Detect(imgFile); }
    void OnOK() override { deferred.Resolve(Napi::String::New(Env(), result.strRes)); }
    void OnError(const Napi::Error& e) override { deferred.Reject(e.Value()); }

private:
    RapidOcrOnnx* obj;
    std::string imgFile;
    OcrResult result;
    Napi::Promise::Deferred deferred;
};

Napi::Value RapidOcrOnnx::detect(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    std::string imgFile = info[0].As<Napi::String>().Utf8Value();

    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
    DetectWorker* worker = new DetectWorker(env, this, imgFile, deferred);
    worker->Queue();
    return deferred.Promise();
}
Napi::Value RapidOcrOnnx::detectSync(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    std::string imgFile = info[0].As<Napi::String>().Utf8Value();

    OcrResult result = Detect(imgFile);
    return Napi::String::New(env, result.strRes);
}

Napi::Object Init(Napi::Env env, Napi::Object exports) { return RapidOcrOnnx::Init(env); }

NODE_API_MODULE(RapidOcrOnnx, Init);
