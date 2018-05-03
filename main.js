/*const fs = require('fs');

const buf = fs.readFile('../main.js');

fs.writeFile("./test-mig.json", JSON.stringify({
    hello: 'world'
}));*/

const prompt = require('prompt'),
    curl = require('curl');

//prompt.input('Test:')

//const t = prompt.list('test', ['valg', 'valg2']);

const c = new curl.Client();

const res = c.request({
    url: "https://upload.wikimedia.org/wikipedia/commons/d/dd/Big_%26_Small_Pumkins.JPG",
    bodyWriter: function (buffer) {
        //console.log('buffer', buffer.length);
    }
});


//console.log(new TextDecoder().decode(res.data));