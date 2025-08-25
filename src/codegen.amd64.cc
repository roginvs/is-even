#include <array>
#include <cstdint>

enum class Kind
{
    Windows,
    Posix
};

template <Kind K, class Writer>
struct PlatformCommon64
{
    explicit PlatformCommon64(Writer &w) : write(w) {}

    // https://defuse.ca/online-x86-assembler.htm#disassembly

    void writePreamble()
    {
        /*
        mov rax, 0
        */
        static constexpr uint8_t preamble[] = {0x48, 0xC7, 0xC0, 0x00, 0x00, 0x00, 0x00};

        write(preamble);
    }

    void writeIteration(uint32_t i)
    {
        static auto iteration = PlatformCommon64::make_iteration();

        (*(uint32_t *)&iteration[2]) = i;
        iteration[9] = i % 2 == 0 ? 1 : 0;

        write(iteration);
    }

    void writeEpilogue()
    {
        /*
        mov rax, 0xffffffff
        ret
        */
        static constexpr uint8_t epilogue[] = {0x48, 0xB8, 0xFF, 0xFF, 0xFF, 0xFF,
                                               0x00, 0x00, 0x00, 0x00, 0xC3};

        write(epilogue);
    }

private:
    Writer &write;

    static constexpr std::array<std::uint8_t, 11> make_iteration()
    {
        if constexpr (K == Kind::Posix)
        {
            // cmp edi, 0xaabbccdd ; jnz +3 ; mov al,0xab ; ret
            return {0x81, 0xFF, 0xDD, 0xCC, 0xBB, 0xAA,
                    0x75, 0x03, 0xB0, 0xAB, 0xC3};
        }
        else if constexpr (K == Kind::Windows)
        {
            // cmp ecx, 0xaabbccdd ; jnz +3 ; mov al,0xab ; ret
            return {0x81, 0xF9, 0xDD, 0xCC, 0xBB, 0xAA,
                    0x75, 0x03, 0xB0, 0xAB, 0xC3};
        }
        else
        {
            static_assert(K == Kind::Windows || K == Kind::Posix,
                          "Unhandled Kind in make_iteration()");
        }
    }
};

template <class Writer>
using PlatformWindows64 = PlatformCommon64<Kind::Windows, Writer>;

template <class Writer>
using PlatformPosix64 = PlatformCommon64<Kind::Posix, Writer>;