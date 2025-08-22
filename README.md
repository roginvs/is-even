# is-even

A simple utility to check if a number is even.

Works by checking every number, i.e. like 

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

Implemented in assembly code so native nodejs bindings are used. Thus only Linux is supported yet.


After install it will create a ~45GB file with machine code, a picture with "heaviest objects in the universe" is here.

For now works only for numbers from 0 to 0xFFFFFFFF, other numbers is undefined behavior.

Inspired by https://andreasjhkarlsson.github.io/jekyll/update/2023/12/27/4-billion-if-statements.html


## Installation

```bash
npm install https://github.com/roginvs/is-even
```

## Usage

```js
const isEven = require('is-even');

console.log(isEven(4)); // true
console.log(isEven(7)); // false
```

## API

### `isEven(number)`

- **number**: `Number` — The number to check.
- **Returns**: `Boolean` — `true` if the number is even, `false` otherwise.


## License

MIT