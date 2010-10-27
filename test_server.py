import socket
import subprocess

PORT = 25864
PROGRAM = "./server.out"

class TestServer():

    def setup_method(self, method):
        self.server_process = subprocess.Popen([PROGRAM, str(PORT)])
        retry = True
        while retry:
            try:
                self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                self.socket.connect(('localhost', PORT))
                retry = False
            except:
                pass

    def teardown_method(self, method):
        self.socket.close()
        self.server_process.kill()
        self.server_process.wait()

    def test_unknown(self):
        self.socket.send("P")
        msg = self.socket.recv(12)
        assert msg == "Desconhecido"

    def test_simple(self):
        self.socket.send("L")
        msg = self.socket.recv(10)
        assert msg == "Comando L"


