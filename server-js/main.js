const http = require('http');

let counter = 1;

const requestListener = function(req, res) {
  if (req.method == 'GET' && req.url == '/chat') {
    res.setHeader('Content-Type', 'application/json');
    res.writeHead(200);
    res.end(JSON.stringify({'body': 'Hello World', 'counter': counter}));
    counter++;
  } else {
    res.writeHead(500);
    res.end('Oops...');
  }
}

const server = http.createServer(requestListener);
console.log('Starting server...');
server.listen(12397);
