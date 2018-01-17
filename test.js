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
/*
const crypto = require('crypto'),
    zlib = require('zlib'),
    fs = require('fs');
const hash = crypto.createHash('sha').update("Hello, World").digest();

console.log(Duktape.enc('hex', hash));

var out = zlib.gzip("Hello, World, You Too");

console.log(new TextDecoder("utf-8").decode(zlib.unzip(out)));

fs.writeFileSync('test2.gz', out);*/

const http = require('http');

var buf = http.get('http://google.com');

//console.log(new TextDecoder("utf-8").decode(buf));