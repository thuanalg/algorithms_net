// server.js
// Node.js HTTP server đầy đủ với routing và static file
const http = require('http');
const fs = require('fs');
const path = require('path');
const url = require('url');

// Config server
const PORT = 3000;
const HOST = '0.0.0.0'; // chạy trên mọi interface
const PUBLIC_DIR = path.join(__dirname, 'public');

// Helper: lấy content-type theo extension
function getContentType(filePath) {
    const ext = path.extname(filePath).toLowerCase();
    switch (ext) {
        case '.html': return 'text/html';
        case '.css': return 'text/css';
        case '.js': return 'application/javascript';
        case '.json': return 'application/json';
        case '.png': return 'image/png';
        case '.jpg':
        case '.jpeg': return 'image/jpeg';
        case '.gif': return 'image/gif';
        case '.svg': return 'image/svg+xml';
        default: return 'application/octet-stream';
    }
}

// Helper: serve file tĩnh
function serveStaticFile(res, filePath) {
    fs.readFile(filePath, (err, data) => {
        if (err) {
            res.writeHead(404, { 'Content-Type': 'text/plain' });
            res.end('404 Not Found\n');
        } else {
            const contentType = getContentType(filePath);
            res.writeHead(200, { 'Content-Type': contentType });
            res.end(data);
        }
    });
}

// Tạo server
const server = http.createServer((req, res) => {
    const parsedUrl = url.parse(req.url, true);
    const pathname = parsedUrl.pathname;
    const method = req.method;

    // Logging request
    console.log(`[${new Date().toISOString()}] ${method} ${pathname}`);

    // Route API
    if (pathname === '/api') {
        res.writeHead(200, { 'Content-Type': 'application/json' });
        res.end(JSON.stringify({
            status: 'ok',
            timestamp: Date.now(),
            message: 'Hello from Node.js API!'
        }));
        return;
    }

    // Route Homepage
    if (pathname === '/' || pathname === '/index.html') {
        const indexPath = path.join(PUBLIC_DIR, 'index.html');
        serveStaticFile(res, indexPath);
        return;
    }

    // Route static file
    if (pathname.startsWith('/static/')) {
        const filePath = path.join(PUBLIC_DIR, pathname.replace('/static/', ''));
        serveStaticFile(res, filePath);
        return;
    }

    // 404 cho các route khác
    res.writeHead(404, { 'Content-Type': 'text/plain' });
    res.end('404 Not Found\n');
});

// Bắt đầu server
server.listen(PORT, HOST, () => {
    console.log(`Server is running at http://${HOST}:${PORT}`);
    console.log(`Serving static files from: ${PUBLIC_DIR}`);
});
