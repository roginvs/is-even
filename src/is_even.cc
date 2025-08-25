#include <node.h>

#include "./codegen.h"
#include "./module.init.h"

using v8::Boolean;
using v8::Exception;
using v8::Function;
using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::NewStringType;
using v8::Number;
using v8::Object;
using v8::String;
using v8::Value;

void IsEvenMethod(const FunctionCallbackInfo<Value> &args)
{
    Isolate *isolate = args.GetIsolate();

    if (init_module_if_needed() != 0)
    {
        isolate->ThrowException(Exception::Error(
            String::NewFromUtf8(isolate,
                                "Failed to initialize module")
                .ToLocalChecked()));
        return;
    }

    if (args.Length() < 1)
    {
        // Throw an Error that is passed back to JavaScript
        isolate->ThrowException(Exception::Error(
            String::NewFromUtf8(isolate,
                                "Wrong number of arguments")
                .ToLocalChecked()));
        return;
    }

    if (!args[0]->IsNumber())
    {
        isolate->ThrowException(Exception::Error(
            String::NewFromUtf8(isolate,
                                "Wrong arguments")
                .ToLocalChecked()));
        return;
    }

    long int argumentValue = args[0].As<Number>()->Value();

    int returnValue = is_even(argumentValue);

    if (std::getenv("IS_EVEN_DEBUG"))
    {
        printf("asm is_even(%u = 0x%x) = %u = 0x%x\n\n", argumentValue, argumentValue, returnValue, returnValue);
    }

    if (returnValue != 0xFFFFFFFF)
    {
        args.GetReturnValue().Set(Boolean::New(isolate, returnValue));
    }
    else
    {
        args.GetReturnValue().Set(Number::New(isolate, -1));
    }
}

void RebuildCodeMethod(const FunctionCallbackInfo<Value> &args)
{

    if (generate_code_file() != 0)
    {
        Isolate *isolate = args.GetIsolate();
        isolate->ThrowException(Exception::Error(
            String::NewFromUtf8(isolate,
                                "Failed to build code")
                .ToLocalChecked()));
        return;
    }

    args.GetReturnValue().Set(0);
}

void Initialize(Local<Object> exports, Local<Object> module)
{
    Isolate *isolate = exports->GetIsolate();
    auto ctx = isolate->GetCurrentContext();

    Local<Function> isEvenFn =
        v8::Function::New(ctx, IsEvenMethod).ToLocalChecked();

    // Attach the extra method onto the function
    isEvenFn->Set(ctx,
                  v8::String::NewFromUtf8(isolate, "_build_code").ToLocalChecked(),
                  v8::Function::New(ctx, RebuildCodeMethod).ToLocalChecked())
        .Check();

    // module.exports = isEvenFn
    module->Set(ctx,
                v8::String::NewFromUtf8(isolate, "exports").ToLocalChecked(),
                isEvenFn)
        .Check();
}

NODE_MODULE(NODE_GYP_MODULE_NAME, Initialize)
