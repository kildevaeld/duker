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

//const http = require('http');
/*
var js
js = setInterval(function () {
    console.log('interfaval');
}, 1000);


console.log('function', js);
setTimeout(function () {
    console.log('out')
    clearInterval(js);
}, 10000);

console.log(Promise)*/

const _slice = Array.prototype.slice;

const wrap = function (obj) {

    for (var n in obj) {
        if (typeof obj[n] !== 'function') continue;

        obj[n] = (function (fn) {
            return function () {

                const args = _slice.call(arguments);

                return new Promise(function (res, rej) {
                    console.log('raprprapraprap', args)
                    args.push(function (err, result) {
                        if (err) return rej(err);
                        res(result);
                    })
                    console.log('raprap')
                    fn.apply(obj, args);
                })
            }
        })(obj[n]);
    }

    return obj;
}

var fs = require('fs');

fs = wrap(fs);
console.log(Promise)
fs.readFile('../test.js').then(function (result) {
    console.log('success');
    //console.log(new TextDecoder("utf-8").decode(result));
}).catch(function (e) {
    console.log(e.message);
})