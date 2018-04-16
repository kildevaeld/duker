/*const fs = require('fs');

const buf = fs.readFile('../main.js');

fs.writeFile("./test-mig.json", JSON.stringify({
    hello: 'world'
}));*/

const prompt = require('prompt');

const t = prompt.list('test', ['valg', 'valg2']);

console.log(t);