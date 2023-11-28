#ifndef __UTILS__
#define __UTILS__

#include <napi.h>

#define Getter(name, property, type)                  \
    Napi::Value name(const Napi::CallbackInfo& info)  \
    {                                                 \
        return Napi::type::New(info.Env(), property); \
    }
#define Setter(name, property, type, transform)                         \
    void name(const Napi::CallbackInfo& info, const Napi::Value& value) \
    {                                                                   \
        property = value.To##type().transform();                        \
    }

#define Property_int(property)                \
    Getter(Get_##property, property, Number); \
    Setter(Set_##property, property, Number, Int32Value);

#define Property_float(property)              \
    Getter(Get_##property, property, Number); \
    Setter(Set_##property, property, Number, FloatValue);

#define Property_bool(property)                \
    Getter(Get_##property, property, Boolean); \
    Setter(Set_##property, property, Boolean, Value);

#define Property(type, property, value) \
    type property = value;              \
    Property_##type(property);

#define Method(method) Napi::Value method(const Napi::CallbackInfo& info)

#define Instance_Property(classname, property) InstanceAccessor<&classname::Get_##property, &classname::Set_##property>(#property)
#define Instance_Method(classname, method) InstanceMethod<&classname::method>(#method, static_cast<napi_property_attributes>(napi_writable | napi_configurable))

class PromiseWorker : public Napi::AsyncWorker {
public:
    Napi::Promise Promise();
    void OnError(const Napi::Error& e) override;

protected:
    explicit PromiseWorker(Napi::Promise::Deferred& deferred);
    explicit PromiseWorker(Napi::Env& env);

    Napi::Promise::Deferred Deferred();

private:
    Napi::Promise::Deferred deferred;
};

#endif //__UTILS__
