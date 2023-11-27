#include "OcrLite.h"
#include "tools.h"
#include <fstream>
#include <napi.h>
#include <opencv2/imgcodecs.hpp>

class RapidOcrOnnx : public Napi::ObjectWrap<RapidOcrOnnx> {
public:
    static Napi::Function Init(Napi::Env env);
    RapidOcrOnnx(const Napi::CallbackInfo& info);
    ~RapidOcrOnnx();

#ifdef _WIN32
    OcrResult Detect(std::u16string& imgFile);
#else
    OcrResult Detect(std::string& imgFile);
#endif
    OcrResult Detect(char* buffer, size_t sz);

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

Napi::Function RapidOcrOnnx::Init(Napi::Env env)
{
    return DefineClass(env, "RapidOcrOnnx",
        {
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

#ifdef _WIN32
OcrResult RapidOcrOnnx::Detect(std::u16string& imgFile)
{
    FILE* fp = _wfopen((wchar_t*)imgFile.c_str(), L"rb");
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
#else
OcrResult RapidOcrOnnx::Detect(std::string& imgFile)
{
    cv::Mat originSrc = cv::imread(imgFile, cv::IMREAD_COLOR);

    OcrResult result
        = ocrLite->detect(originSrc, padding, maxSideLen, boxScoreThresh, boxThresh, unClipRatio, doAngle, mostAngle);
    result.strRes[result.strRes.length() - 1] = 0;

    return result;
}
#endif
OcrResult RapidOcrOnnx::Detect(char* buffer, size_t size)
{
    cv::_InputArray arr(buffer, size);
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
#ifdef _WIN32
    DetectWorker(Napi::Env& env, Napi::Promise::Deferred deferred, RapidOcrOnnx* obj, std::u16string& imgFile)
        : AsyncWorker(env)
        , deferred(deferred)
        , obj(obj)
        , imgFile(imgFile)
    {
    }
#else
    DetectWorker(Napi::Env& env, Napi::Promise::Deferred deferred, RapidOcrOnnx* obj, std::string& imgFile)
        : AsyncWorker(env)
        , deferred(deferred)
        , obj(obj)
        , imgFile(imgFile)
    {
    }
#endif
    DetectWorker(Napi::Env& env, Napi::Promise::Deferred deferred, RapidOcrOnnx* obj, char* data, size_t sz)
        : AsyncWorker(env)
        , deferred(deferred)
        , obj(obj)
        , data(data)
        , sz(sz)
    {
    }
    ~DetectWorker() { }
    void Execute() override
    {
        if (sz)
            result = obj->Detect(data, sz);
        else
            result = obj->Detect(imgFile);
    }
    void OnOK() override
    {
        deferred.Resolve(Napi::String::New(Env(), result.strRes));
    }
    void OnError(const Napi::Error& e) override
    {
        deferred.Reject(e.Value());
    }

private:
    RapidOcrOnnx* obj;
#ifdef _WIN32
    std::u16string imgFile;
#else
    std::string imgFile;
#endif
    char* data;
    size_t sz = 0;

    OcrResult result;
    Napi::Promise::Deferred deferred;
};

Napi::Value RapidOcrOnnx::detect(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
    DetectWorker* worker;
    if (info[0].IsTypedArray()) {
        Napi::Uint8Array arr = info[0].As<Napi::Uint8Array>();
        char* buffer = (char*)arr.ArrayBuffer().Data();
        buffer += arr.ByteOffset();
        size_t size = arr.ByteLength();
        worker = new DetectWorker(env, deferred, this, buffer, size);
    } else {
#ifdef _WIN32
        std::u16string imgFile = info[0].As<Napi::String>().Utf16Value();
#else
        std::string imgFile = info[0].As<Napi::String>().Utf8Value();
#endif
        worker = new DetectWorker(env, deferred, this, imgFile);
    }
    worker->Queue();
    return deferred.Promise();
}
Napi::Value RapidOcrOnnx::detectSync(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    OcrResult result;
    if (info[0].IsTypedArray()) {
        Napi::Uint8Array arr = info[0].As<Napi::Uint8Array>();
        char* buffer = (char*)arr.ArrayBuffer().Data();
        buffer += arr.ByteOffset();
        size_t size = arr.ByteLength();
        result = Detect(buffer, size);
    } else {
#ifdef _WIN32
        std::u16string imgFile = info[0].As<Napi::String>().Utf16Value();
#else
        std::string imgFile = info[0].As<Napi::String>().Utf8Value();
#endif
        result = Detect(imgFile);
    }
    return Napi::String::New(env, result.strRes);
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    exports.Set("RapidOcrOnnx", RapidOcrOnnx::Init(env));
    return exports;
}

NODE_API_MODULE(RapidOcrOnnx, Init);
