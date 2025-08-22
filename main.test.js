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
  test("Throws on non-number", () => {
    expect(() => isEven("string")).toThrow();
  });
  test("Throws on no arguments", () => {
    expect(() => isEven()).toThrow();
  });
});
