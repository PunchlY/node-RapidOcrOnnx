#ifndef __TOOL_H__
#define __TOOL_H__

#define Getter(name, property, type)                   \
    Napi::Value name(const Napi::CallbackInfo& info) { \
        Napi::Env env = info.Env();                    \
        return Napi::type::New(env, property);       \
    }
#define Setter(name, property, type, transform)                           \
    void name(const Napi::CallbackInfo& info, const Napi::Value &value) { \
        property = value.To##type().##transform();                        \
    }

#define Property_int(property)                            \
    Getter(Get_##property, property, Number);             \
    Setter(Set_##property, property, Number, Int32Value);

#define Property_float(property)                          \
    Getter(Get_##property, property, Number);             \
    Setter(Set_##property, property, Number, FloatValue);

#define Property_bool(property)                       \
    Getter(Get_##property, property, Boolean);        \
    Setter(Set_##property, property, Boolean, Value);

#define Property(type, property, value) \
    type property = value;              \
    Property_##type(property);

#define Method(method) Napi::Value method(const Napi::CallbackInfo& info)

#define Instance_Property(property) InstanceAccessor<&Get_##property, &Set_##property>(#property)
#define Instance_Method(method) InstanceMethod<&method>(#method, static_cast< napi_property_attributes >(napi_writable | napi_configurable))

#endif //__TOOL_H__
