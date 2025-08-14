// server.js
const http = require('http');

// Port bạn muốn mở server
const PORT = 3000;

// Tạo server
const server = http.createServer((req, res) => {
    console.log(`${req.method} ${req.url}`); // log request
    res.writeHead(200, { 'Content-Type': 'text/plain' });
    res.end('Hello from Node.js HTTP Server!\n');
});

// Lắng nghe port
server.listen(PORT, () => {
    console.log(`Server is running at http://localhost:${PORT}`);
});
