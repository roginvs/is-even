const is_even = require("./build/Release/is_even");

describe("isEven", () => {
  test("Value is 0", () => {
    expect(is_even.isEven(0)).toBe(true);
  });
  test("Value is 1", () => {
    expect(is_even.isEven(1)).toBe(false);
  });
  test("Value is 2", () => {
    expect(is_even.isEven(2)).toBe(true);
  });
  test("Value is 3", () => {
    expect(is_even.isEven(3)).toBe(false);
  });
});
