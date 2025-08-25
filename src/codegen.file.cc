#include <iostream>
#include <array>
#include <cstdint>
#include "codegen.amd64.cc"
#include "codegen.arm64.cc"
class FileWriter
{
public:
    FileWriter(FILE *fp) : m_fp(fp) {}
    template <std::size_t N>
    void inline operator()(const std::array<uint8_t, N> &data)
    {
        fwrite(data.data(), sizeof(uint8_t), N, m_fp);
    }

private:
    FILE *m_fp;
};

template <template <class> class Platform>
struct CodeGenerator
{

public:
    CodeGenerator(bool is_light, bool is_debug)
        : m_is_debug(is_debug), m_is_light(is_light)
    {
    }

    int generate(const char *filename)
    {
        if (m_is_debug)
        {
            printf("Generating file %s...\n", filename);
        }

        FILE *fp = fopen(filename, "wb");
        if (!fp)
        {
            perror("fopen");
            return -1;
        }

        auto writer = FileWriter(fp);
        auto platform = Platform{writer};

        platform.writePreamble();

        long int max_i = m_is_light ? 0x2FFFF : 0xFFFFFFFF;

        for (long int i = 0; i <= max_i; i++)
        {
            if (i % 0x100000 == 0 && m_is_debug)
            {
                printf("Processing: %lx, %li%%\n", i, (i * 100) / max_i);
            }

            platform.writeIteration(static_cast<uint32_t>(i));
        }
        if (m_is_light)
        {
            platform.writeIteration(0xFFFFFFFF);
        }
        // platform.writeIteration(0x15432);
        // platform.writeIteration(0x12345);

        platform.writeEpilogue();
        fclose(fp);

        if (m_is_debug)
        {
            printf("Done\n");
        }

        return 0;
    }

private:
    bool m_is_debug;
    bool m_is_light;
};

auto create_code_generator(bool is_light, bool is_debug)
{
#if defined(__x86_64__) || defined(_M_X64)

#if defined(__linux__) || defined(__APPLE__)
    return CodeGenerator<PlatformPosix64>(is_light, is_debug);
#elif defined(_WIN64)
#error "Windows is not supported yet"
#else
#error "Unknown platform"
#endif

#elif defined(__aarch64__) || defined(_M_ARM64)
    return CodeGenerator<PlatformArm64>(is_light, is_debug);
#else
#error "Unsupported architecture"
#endif
}