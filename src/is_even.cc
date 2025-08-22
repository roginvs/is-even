#include <node.h>

#include <iostream>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "./build_code.h"
#include "./get_module_filename.h"

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

typedef int IsEven(int);

IsEven *is_even = nullptr;

int init_module_if_needed()
{
    if (is_even != nullptr)
    {
        return 0;
    }

    auto fileName = get_module_filename();

    if (std::getenv("IS_EVEN_DEBUG"))
    {
        printf("Binary file is %s\n", fileName.c_str());
    }

    int fd = open(fileName.c_str(), O_RDONLY);
    if (fd == -1)
    {
        perror("fopen");
        return -1;
    }

    struct stat statbuf;

    if (fstat(fd, &statbuf) < 0)
    {
        perror("fstat error");
        return -1;
    }

    void *addr = mmap((void *)0x10000000000ULL, statbuf.st_size,
                      PROT_EXEC | PROT_READ, MAP_PRIVATE, fd, 0);
    if (addr == MAP_FAILED)
    {
        perror("mmap");
        close(fd);
        return 1;
    }
    close(fd);

    if (std::getenv("IS_EVEN_DEBUG"))
    {
        printf("Memory mapped at address: %p\n", addr);
    }

    if (std::getenv("IS_EVEN_DEBUG"))
    {
        unsigned char *p = (unsigned char *)addr;

        size_t len = 16 * 16;

        for (size_t i = 0; i < len; i += 16)
        {
            // Print offset
            printf("%08zx  ", i);

            // Print hex bytes
            for (size_t j = 0; j < 16; j++)
            {
                if (i + j < len)
                    printf("%02x ", p[i + j]);
                else
                    printf("   "); // padding
            }

            // Print ASCII
            printf(" |");
            for (size_t j = 0; j < 16 && i + j < len; j++)
            {
                unsigned char c = p[i + j];
                printf("%c", isprint(c) ? c : '.');
            }
            printf("|\n");
        }
    }

    is_even = (IsEven *)addr;

    return 0;
}

void IsEvenMethod(const FunctionCallbackInfo<Value> &args)
{

    // getchar();
    // raise(SIGTRAP);

    if (init_module_if_needed() != 0)
    {
        Isolate *isolate = args.GetIsolate();
        isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate,
                                "Failed to initialize module")
                .ToLocalChecked()));
        return;
    }

    Isolate *isolate = args.GetIsolate();

    if (args.Length() < 1)
    {
        // Throw an Error that is passed back to JavaScript
        isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate,
                                "Wrong number of arguments")
                .ToLocalChecked()));
        return;
    }

    if (!args[0]->IsNumber())
    {
        isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate,
                                "Wrong arguments")
                .ToLocalChecked()));
        return;
    }

    int argumentValue = args[0].As<Number>()->Value();

    int returnValue = is_even(argumentValue);

    args.GetReturnValue().Set(Boolean::New(isolate, returnValue));
}

void RebuildCodeMethod(const FunctionCallbackInfo<Value> &args)
{

    if (build_code() != 0)
    {
        Isolate *isolate = args.GetIsolate();
        isolate->ThrowException(Exception::TypeError(
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
