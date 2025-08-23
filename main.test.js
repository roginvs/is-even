const isEven = require("./build/Release/is_even");

describe("isEven", () => {
  test("Value is 0", () => {
    expect(isEven(0)).toBe(true);
  });
  test("Value is 1", () => {
    expect(isEven(1)).toBe(false);
  });
  test("Value is 2", () => {
    expect(isEven(2)).toBe(true);
  });
  test("Value is 3", () => {
    expect(isEven(3)).toBe(false);
  });
  test("Value is 0x10000", () => {
    expect(isEven(0x10000)).toBe(true);
  });
  test("Value is 0x10001", () => {
    expect(isEven(0x10001)).toBe(false);
  });
  test("Value is 0x15432", () => {
    expect(isEven(0x15432)).toBe(true);
  });
  test("Value is 0x12345", () => {
    expect(isEven(0x12345)).toBe(false);
  });
  test("Value is 0x3FFFF", () => {
    expect(isEven(0x3ffff)).toBe(process.env.IS_EVEN_LIGHT ? -1 : false);
  });
  test("Value is 0xaabbccdd", () => {
    expect(isEven(0xaabbccdd)).toBe(process.env.IS_EVEN_LIGHT ? -1 : false);
  });
  test("Value is 0xddccbbaa", () => {
    expect(isEven(0xddccbbaa)).toBe(process.env.IS_EVEN_LIGHT ? -1 : true);
  });
  test("Throws on non-number", () => {
    expect(() => isEven("string")).toThrow();
  });
  test("Throws on no arguments", () => {
    expect(() => isEven()).toThrow();
  });
});
