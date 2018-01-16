/*const o = require('./other');

o.test();

const fs = require('fs');

const buffer = fs.readFileSync('../test.js');

fs.writeFileSync("rapper.json", JSON.stringify({
    Hello: 'world'
}));

try {
    fs.writeFileSync("test", new Buffer(JSON.stringify({
        Hello: 'world'
    })));
} catch (e) {
    console.error(e);
}


fs.mkdirSync("test_rapper", 0755)*/

const crypto = require('crypto');
console.log(crypto);
const hash = crypto.createHash('sha').update("Hello, World").digest();

console.log(Duktape.enc('hex', hash));