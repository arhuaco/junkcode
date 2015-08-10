from http.server import BaseHTTPRequestHandler, HTTPServer
import logging
HOST_NAME = '0.0.0.0'
PORT_NUMBER = 8010
class MyHandler(BaseHTTPRequestHandler):
    def do_HEAD(s):
        s.send_response(200)
        s.send_header("Content-type", "text/html")
        s.end_headers()
    def do_GET(self):
        self.send_response(200, 'OK')
        self.send_header('Content-type', 'text/plain')
        self.end_headers()
        self.wfile.write(bytes('Hola, mundo', 'utf-8'))
def main():
    httpd = HTTPServer((HOST_NAME, PORT_NUMBER), MyHandler)
    logging.info("Server Starts - {}:{}".format(HOST_NAME, PORT_NUMBER))
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        pass
if __name__ == '__main__':
    main()
