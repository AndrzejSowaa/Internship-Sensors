from http.server import BaseHTTPRequestHandler, HTTPServer
import urllib

class MyRequestHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        print("Received GET request")
        # Parse query parameters
        query_components = urllib.parse.parse_qs(urllib.parse.urlparse(self.path).query)
        print("Query components:", query_components)
        
        # Zapisywanie parametrów do pliku
        with open("received_data.txt", "a") as file:
            file.write(str(query_components) + "\n")
        
        # Send response status code
        self.send_response(200)
        
        # Send headers
        self.send_header('Content-type', 'text/html')
        self.end_headers()
        
        # Send response message
        response_message = "Data received and saved successfully"
        self.wfile.write(response_message.encode())
        print("Response sent")

def run(server_class=HTTPServer, handler_class=MyRequestHandler, port=8080):
    server_address = ('', port)
    httpd = server_class(server_address, handler_class)
    print(f'Starting httpd server on port {port}...')
    httpd.serve_forever()

if __name__ == "__main__":
    run()