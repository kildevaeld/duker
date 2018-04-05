const out = require('./other2');
const out2 = require('./build/example/plugin');
//const test = require('test');
console.log(out(), out2);
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