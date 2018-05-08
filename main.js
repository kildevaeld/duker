const curl = require('curl');


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

console.log('test', new TextDecoder('utf8').decode(resp.body));