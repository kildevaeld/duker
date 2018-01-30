var fs = require('fs');


fs = Promise.wrap(fs);
console.log(Promise)
fs.readFile('../test.js').then(function (result) {
    console.log('success', process.cwd());
    //console.log(new TextDecoder("utf-8").decode(result));
}).catch(function (e) {
    console.log(e.message);
})