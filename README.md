# is-even

A simple utility to check if a number is even.

Works by checking every number one-by-one, i.e. like

```javascript
function isEven(n)) {
    if (n == 0) {
        return true
    } else if (n == 1) {
        return false
    } else if (n == 2) {
        return true
    } else if (n == 3) {
        ...
    } else if (n === 0xFFFFFFFF) {
        return false
    };
    return -1;
}
```

Implemented in assembly code so native nodejs bindings are used.

After installation (and first time require) it will generate a ~45 GB file (about 100 GB on arm64) of machine code which will be placed inside node_modules.
(Yes, you read that right. See the meme: Heaviest objects in the universe → neutron stars, black holes, node_modules.)

Supports basic platforms: Linux/Mac/Windows on amd64/arm64 cpu.

Inspired by [4 billion if statements](https://andreasjhkarlsson.github.io/jekyll/update/2023/12/27/4-billion-if-statements.html)

## Limitations

- For now works only for numbers from 0 to 0xFFFFFFFF. Passing anything else is **undefined behavior**.
- Might be slow (dozens of seconds) on big numbers

## Installation

```bash
npm install https://github.com/roginvs/is-even
```

## Usage

```js
const isEven = require("is-even");

console.log(isEven(4)); // true
console.log(isEven(7)); // false
```

## TODO

- Add support for risc-v

## License

MIT
