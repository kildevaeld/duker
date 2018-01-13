console.log('Hello, World', process.cwd());

const path = require('path'),
    fs = require('fs');


const buffer = fs.readFileSync("../test.js");
//console.log(buffer);
console.log(fs.readdirSync('.'))
console.log(path.basename("test/mig"));
console.log(path.join('test', 'rapper', '/hopper'), process.env.HOME);

console.log(Object.getOwnPropertyNames(process.env));



Object.keys(process.env)
    .forEach(function (m) {
        console.log('raprap', m)
    })

const mod = require('./other.js');

//mod.test();
console.log(mod.test);