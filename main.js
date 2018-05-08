/*const fs = require('fs');

const buf = fs.readFile('../main.js');

fs.writeFile("./test-mig.json", JSON.stringify({
    hello: 'world'
}));*/

const prompt = require('prompt'),
    curl = require('curl'),
    io = require('io')


const Reader = (function (Super) {

    function Reader() {
        Super.call(this);
        this.sent = false;
    }

    Reader.prototype = Object.create(Super.prototype);

    /*Object.assign(Reader.prototype, Super.prototype, {
        
    });*/
    Reader.prototype._read = function (size) {

        if (this.sent) return null;
        this.sent = true;
        return new Buffer("Hello, World!");
    };

    Reader.prototype.constructor = Super;

    return Reader;

})(io.Reader)

//prompt.input('Test:')

//const t = prompt.list('test', ['valg', 'valg2']);
const req = new curl.Request({
    url: 'http://localhost:8000',
    method: 'POST',
    data: new Reader
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


var file = new io.File();


//req.progress = '' //function () {}


//console.log(res.header, res.statusCode);

//console.log(new TextDecoder().decode(res.data));