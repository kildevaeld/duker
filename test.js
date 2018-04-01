const out = require('./build/example/libplugin.dylib');

const EventEmitter = require('events').EventEmitter;

console.log(out)

var emitter = new EventEmitter();

emitter.on('test', function (message) {
    console.log(message);
});


console.log(emitter);

emitter.emit('test', 'Hello, World 2')