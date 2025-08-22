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

After installation it will generate a ~45 GB file of machine code which will be placed inside node_modules.
(Yes, you read that right. See the meme: Heaviest objects in the universe → neutron stars, black holes, node_modules.)

Inspired by [4 billion if statements](https://andreasjhkarlsson.github.io/jekyll/update/2023/12/27/4-billion-if-statements.html)

## Limitations

- For now works only for numbers from 0 to 0xFFFFFFFF. Passing anything else is **undefined behavior**.
- Only Linux x64 and Intel Mac are supported
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

## API

### `isEven(number)`

- **number**: `Number` — The number to check.
- **Returns**: `Boolean` — `true` if the number is even, `false` otherwise.

## TODO

- Add support for arm64 platform
- Add support for Windows

## License

MIT
