#include "OcrResultUtils.h"
#include <napi.h>

PromiseWorker::PromiseWorker(Napi::Promise::Deferred& deferred)
    : AsyncWorker(deferred.Env())
    , deferred(deferred)
{
}

PromiseWorker::PromiseWorker(Napi::Env& env)
    : AsyncWorker(env)
    , deferred(Napi::Promise::Deferred::New(env))
{
}

void PromiseWorker::OnError(const Napi::Error& e)
{
    deferred.Reject(e.Value());
}

Napi::Promise::Deferred PromiseWorker::Deferred()
{
    return deferred;
}

Napi::Promise PromiseWorker::Promise()
{
    return deferred.Promise();
}
