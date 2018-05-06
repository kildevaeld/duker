/*const fs = require('fs');

const buf = fs.readFile('../main.js');

fs.writeFile("./test-mig.json", JSON.stringify({
    hello: 'world'
}));*/

const prompt = require('prompt'),
    curl = require('curl');

//prompt.input('Test:')

//const t = prompt.list('test', ['valg', 'valg2']);

const req = new curl.Request({
    url: 'http://localhost:8000',
    method: 'POST',
    data: 'Test'
});



/*const res = c.request({
    url: "https://upload.wikimedia.org/wikipedia/commons/d/dd/Big_%26_Small_Pumkins.JPG",
    
    header: {
        'Accept': "text/html"
    },
    progress: function (p, t) {

    }
});*/

/*var req = new curl.Request({
    url: 'https://google.com',
    header: {
        'content-type': 'application/json'
    }
});*/

const rsp = curl.req(req);
console.log(new TextDecoder().decode(rsp.body));


//req.progress = '' //function () {}


console.log(req);
//console.log(res.header, res.statusCode);

//console.log(new TextDecoder().decode(res.data));