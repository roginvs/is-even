#include <iostream>
#include <concepts>

template <typename Platform>
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

        auto platform = Platform(fp);

        platform.writePreamble();

        long int max_i = m_is_light ? 0x2FFFF : 0xFFFFFFFF;

        for (long int i = 0; i <= max_i; i++)
        {
            if (i % 0x100000 == 0 && m_is_debug)
            {
                printf("Processing: %lx, %li%%\n", i, (i * 100) / max_i);
            }
            platform.writeIteration(i);
        }
        platform.writeEpilogue();
        fclose(fp);

        if (m_is_debug)
        {
            printf("Done\n");
        }

        return 0;
    }

    virtual ~CodeGenerator() = default;

private:
    bool m_is_debug;
    bool m_is_light;
};

struct PlatformPosix64
{
    PlatformPosix64(FILE *fp) : m_fp(fp) {}

    // https://defuse.ca/online-x86-assembler.htm#disassembly

    void writePreamble()
    {
        /*
        mov rax, 0
        */
        static unsigned char preamble[] = {0x48, 0xC7, 0xC0, 0x00, 0x00, 0x00, 0x00};

        fwrite(preamble, sizeof(preamble), 1, m_fp);
    }

    void writeIteration(long int i)
    {
        /*
        cmp edi, 0xaabbccdd
        jnz L1
        mov al, 0xab
        ret
        L1:
        */
        static unsigned char iteration[] = {0x81, 0xFF, 0xDD, 0xCC, 0xBB, 0xAA,
                                            0x75, 0x03, 0xB0, 0xAB, 0xC3};

        __uint32_t val = i;
        (*(__uint32_t *)&iteration[2]) = val;
        iteration[9] = i % 2 == 0 ? 1 : 0;

        fwrite(iteration, sizeof(iteration), 1, m_fp);
    }

    void writeEpilogue()
    {
        /*
        mov rax, 0xffffffff
        ret
        */
        static unsigned char epilogue[] = {0x48, 0xB8, 0xFF, 0xFF, 0xFF, 0xFF,
                                           0x00, 0x00, 0x00, 0x00, 0xC3};

        fwrite(epilogue, sizeof(epilogue), 1, m_fp);
    }

private:
    FILE *m_fp;
};

auto create_code_generator(bool is_light, bool is_debug)
{
    return CodeGenerator<PlatformPosix64>(is_light, is_debug);
}