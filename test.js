const out = require('./other2');
const out2 = require('./build/example/plugin');
const test = require('test');
console.log(out(), process.cwd());
console.log(out2, test, require('./test-data.json'));

var idx = setInterval(function () {
    console.log('interval');
}, 200)

setTimeout(function () {
    console.log('timeout', idx);
    clearInterval(idx);
}, 5000);

var test = [];
for (var i = 0; i < 100000; i++) {
    test.push({
        test: 'test',
        test2: 'test2'
    });
}
setTimeout(function () {
    test = [];
}, 5000);

/*const EventEmitter = require('events').EventEmitter;

console.log(out)

var emitter = new EventEmitter();

emitter.on('test', function (message) {
    console.log(message, this);
    emitter.off('test', doit);
});

function doit(mes) {
    console.log('message', mes);
}

emitter.on('test', doit);

emitter.emit('test', 'Hello, World 2')*/
//console.log('rap')