/*const curl = require('curl');


const req = new curl.Request({
    url: "http://localhost:5000/auth/signup",
    method: "POST",
    data: JSON.stringify({
        email: 'test@gmail.com',
        password: 'password'
    }),
    header: {
        'Content-Type': 'application/json'
    }
});

const resp = curl.req(req);

console.log('test', new TextDecoder('utf8').decode(resp.body));*/

const io = require('io');
console.log(io)
var file = new io.File("../main.js", "r");

file.seek(0, io.SEEK_END);
var i = file.tell();
file.rewind();
const buf = file.read(i);

console.log(new TextDecoder().decode(buf));

var file2 = new io.File("test.rap", "w");

file2.write("Hello, World").write("HElo")

file2.close();