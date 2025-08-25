#include <array>
#include <cstdint>

class Arm64
{
public:
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
        return {0x61, 0x00, 0x00, 0x54};
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
};

template <class T, std::size_t N>
constexpr bool arr_eq(const std::array<T, N> &a, const std::array<T, N> &b)
{
    for (std::size_t i = 0; i < N; ++i)
        if (a[i] != b[i])
            return false;
    return true;
}
struct Arm64Test
{
    static_assert(arr_eq(
        Arm64::movz_x(Arm64::XReg::X1, 0xccdd),
        std::array<std::uint8_t, 4>{0xA1, 0x9B, 0x99, 0xD2}));

    static_assert(arr_eq(
        Arm64::movz_x(Arm64::XReg::X1, 0xaabb),
        std::array<std::uint8_t, 4>{0x61, 0x57, 0x95, 0xD2}));

    static_assert(arr_eq(
        Arm64::movz_x(Arm64::XReg::X0, 0),
        std::array<std::uint8_t, 4>{0x00, 0x00, 0x80, 0xD2}));
    static_assert(arr_eq(
        Arm64::movz_x(Arm64::XReg::X0, 1),
        std::array<std::uint8_t, 4>{0x20, 0x00, 0x80, 0xD2}));

    static_assert(arr_eq(
        Arm64::movk_w1_lsl16(0xaabb),
        std::array<std::uint8_t, 4>{0x61, 0x57, 0xb5, 0x72}));

    static_assert(arr_eq(
        Arm64::movk_w1_lsl16(0x0000),
        std::array<std::uint8_t, 4>{0x01, 0x00, 0xA0, 0x72}));

    static_assert(arr_eq(
        Arm64::movk_w1_lsl16(0x0001),
        std::array<std::uint8_t, 4>{0x21, 0x00, 0xA0, 0x72}));

    static_assert(arr_eq(
        Arm64::mov_w0(0),
        std::array<std::uint8_t, 4>{0x00, 0x00, 0x80, 0x52}));

    static_assert(arr_eq(
        Arm64::mov_w0(1),
        std::array<std::uint8_t, 4>{0x20, 0x00, 0x80, 0x52}));
};

template <class Writer>
struct PlatformArm64
{
    explicit PlatformArm64(Writer &w) : write(w) {}

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
        write(Arm64::movz_x(Arm64::XReg::X1, i & 0xFFFF));
        write(Arm64::movk_w1_lsl16((i >> 16) & 0xFFFF));
        write(Arm64::cmp_w0_w1());
        write(Arm64::b_ne_8());
        write(Arm64::movz_x(Arm64::XReg::X0, i % 2 == 0 ? 1 : 0));
        write(Arm64::ret());
    }

    void writeEpilogue()
    {
        // TODO: Use funcs

        /*
        mov x0, 0
        mov     w0, #0xffffffff
        ret
        */
        std::array<std::uint8_t, 12> epilogue = {0x00, 0x00, 0x80, 0xD2,
                                                 0x00, 0x00, 0x80, 0x12,
                                                 0xC0, 0x03, 0x5F, 0xD6};
        write(epilogue);
    }

private:
    Writer &write;

    friend struct PlatformArm64Test;
};
