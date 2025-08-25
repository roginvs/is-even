#include <array>
#include <cstdint>

template <class Writer>
struct PlatformPosix64
{
    explicit PlatformPosix64(Writer &w) : write(w) {}

    // https://defuse.ca/online-x86-assembler.htm#disassembly

    void writePreamble()
    {
        /*
        mov rax, 0
        */
        static std::array<uint8_t, 7> preamble = {0x48, 0xC7, 0xC0, 0x00, 0x00, 0x00, 0x00};

        write(preamble);
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
        static std::array<uint8_t, 11> iteration = {0x81, 0xFF, 0xDD, 0xCC, 0xBB, 0xAA,
                                                    0x75, 0x03, 0xB0, 0xAB, 0xC3};

        (*(__uint32_t *)&iteration[2]) = i;
        iteration[9] = i % 2 == 0 ? 1 : 0;

        write(iteration);
    }

    void writeEpilogue()
    {
        /*
        mov rax, 0xffffffff
        ret
        */
        static std::array<uint8_t, 11> epilogue = {0x48, 0xB8, 0xFF, 0xFF, 0xFF, 0xFF,
                                                   0x00, 0x00, 0x00, 0x00, 0xC3};

        write(epilogue);
    }

private:
    Writer &write;
};
