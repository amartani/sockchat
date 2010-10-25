import socket
import subprocess

PORT = 25864
PROGRAM = "./server.out"

class test_server():

    def setUp(self):
        self.server_process = subprocess.Popen([PROGRAM, str(PORT)])
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.connect(('localhost', PORT))

    def tearDown(self):
        self.socket.close()
        self.server_process.kill()

    def test_simple(self):
        self.socket.send("blabla")
        msg = self.socket.recv(10)
        assert msg == "blablabla"


