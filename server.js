const http = require('http');

http.createServer((req, res) => {
    if (req.method != "POST") {
        res.statusCode = 400;
        res.end();
        return;
    } else {
        res.statusCode = 200;
        req.pipe(res);
    }
}).listen(8000);