#include <iostream>
#include <array>
#include <cstdint>

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

            platform.writeIteration(static_cast<uint32_t>(i));
        }
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

    void writeIteration(uint32_t i)
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

        (*(__uint32_t *)&iteration[2]) = i;
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

struct PlatformArm64
{
    PlatformArm64(FILE *fp) : m_fp(fp) {}

    void writePreamble()
    {
        // No prologue needed
    }

    void writeIteration(uint32_t i)
    {
        /*
        0000:  A1 9B 99 52    // movz  w1, #0xccdd
        0004:  61 57 B5 72    // movk  w1, #0xaabb, lsl #16
        0008:  1F 00 01 6B    // cmp   w0, w1        (subs wzr,w0,w1)
        000C:  41 00 00 54    // b.ne  L1            (to 0x18; imm19 = 2)
        0010:  20 00 80 52    // movz   w0, #1        (alias of movz w0,#1)
        0014:  C0 03 5F D6    // ret
        0018:  ...            // L1:
        L1:
        */
        write(PlatformArm64::movz_x(PlatformArm64::XReg::X1, i & 0xFFFF));
        write(PlatformArm64::movk_w1_lsl16((i >> 16) & 0xFFFF));
        write(PlatformArm64::cmp_w0_w1());
        write(PlatformArm64::b_ne_8());
        write(PlatformArm64::movz_x(PlatformArm64::XReg::X0, i % 2));
        write(PlatformArm64::ret());
    }

    void writeEpilogue()
    {
        /*
        mov     w0, #0xffffffff
        ret
        */
        static unsigned char epilogue[] = {0x00, 0x00, 0x80, 0x12,
                                           0xC0, 0x03, 0x5F, 0xD6};
        fwrite(epilogue, sizeof(epilogue), 1, m_fp);
    }

private:
    FILE *m_fp;

    template <std::size_t N>
    void write(const std::array<std::uint8_t, N> &data)
    {
        fwrite(data.data(), sizeof(std::uint8_t), N, m_fp);
    }

    enum class XReg : std::uint8_t
    {
        X0 = 0,
        X1 = 1,
        X2 = 2,

    };

    static constexpr std::array<std::uint8_t, 4>
    ret()
    {
        return {0xC0, 0x03, 0x5F, 0xD6};
    }

    static constexpr std::array<std::uint8_t, 4> mov_w0(uint16_t imm16)
    {
        const std::uint32_t word =
            0x52800000u | (std::uint32_t(imm16) << 5) | 0u; // Rd = 0

        return {
            static_cast<std::uint8_t>(word & 0xFF),
            static_cast<std::uint8_t>((word >> 8) & 0xFF),
            static_cast<std::uint8_t>((word >> 16) & 0xFF),
            static_cast<std::uint8_t>((word >> 24) & 0xFF),
        };
    }

    static constexpr std::array<std::uint8_t, 4> b_ne_8()
    {
        return {0x41, 0x00, 0x00, 0x54};
    }

    static constexpr std::array<std::uint8_t, 4> cmp_w0_w1()
    {
        return {0x1F, 0x00, 0x01, 0x6B};
    }

    static constexpr std::array<std::uint8_t, 4>
    movz_x(XReg rd, std::uint16_t imm16)
    {
        const std::uint32_t word =
            0xD2800000u | (std::uint32_t(imm16) << 5) | (static_cast<std::uint8_t>(rd) & 0x1F);

        return {
            std::uint8_t(word & 0xFF),
            std::uint8_t((word >> 8) & 0xFF),
            std::uint8_t((word >> 16) & 0xFF),
            std::uint8_t((word >> 24) & 0xFF),
        };
    }

    //  movk w1, #0xaabb, lsl #16
    static constexpr std::array<std::uint8_t, 4> movk_w1_lsl16(uint16_t imm16)
    {
        const std::uint32_t word =
            0x72A00000u |                 // MOVK base (W-form)
            (1u << 21) |                  // hw = 1 → LSL #16
            (std::uint32_t(imm16) << 5) | // imm16 into [20:5]
            0x1u;                         // Rd = w1

        return {
            static_cast<std::uint8_t>(word & 0xFF),
            static_cast<std::uint8_t>((word >> 8) & 0xFF),
            static_cast<std::uint8_t>((word >> 16) & 0xFF),
            static_cast<std::uint8_t>((word >> 24) & 0xFF),
        };
    }

    friend struct PlatformArm64Test;
};

template <class T, std::size_t N>
constexpr bool arr_eq(const std::array<T, N> &a, const std::array<T, N> &b)
{
    for (std::size_t i = 0; i < N; ++i)
        if (a[i] != b[i])
            return false;
    return true;
}
struct PlatformArm64Test
{
    static_assert(arr_eq(
        PlatformArm64::movz_x(PlatformArm64::XReg::X1, 0xccdd),
        std::array<std::uint8_t, 4>{0xA1, 0x9B, 0x99, 0xD2}));

    static_assert(arr_eq(
        PlatformArm64::movz_x(PlatformArm64::XReg::X1, 0xaabb),
        std::array<std::uint8_t, 4>{0x61, 0x57, 0x95, 0xD2}));

    static_assert(arr_eq(
        PlatformArm64::movz_x(PlatformArm64::XReg::X0, 0),
        std::array<std::uint8_t, 4>{0x00, 0x00, 0x80, 0xD2}));
    static_assert(arr_eq(
        PlatformArm64::movz_x(PlatformArm64::XReg::X0, 1),
        std::array<std::uint8_t, 4>{0x20, 0x00, 0x80, 0xD2}));

    static_assert(arr_eq(
        PlatformArm64::movk_w1_lsl16(0xaabb),
        std::array<std::uint8_t, 4>{0x61, 0x57, 0xb5, 0x72}));

    static_assert(arr_eq(
        PlatformArm64::mov_w0(0),
        std::array<std::uint8_t, 4>{0x00, 0x00, 0x80, 0x52}));

    static_assert(arr_eq(
        PlatformArm64::mov_w0(1),
        std::array<std::uint8_t, 4>{0x20, 0x00, 0x80, 0x52}));
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