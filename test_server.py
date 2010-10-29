import socket
import subprocess

PORT = 25864
PROGRAM = "./server.out"
N_CLIENTS = 10

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

class TestServerWithMultipleClients():

    def setup_method(self, method):
        self.server_process = subprocess.Popen([PROGRAM, str(PORT)])
        self.sockets = list()
        for i in range(10):
            retry = True
            while retry:
                try:
                    socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                    socket.connect(('localhost', PORT))
                    self.sockets.add(socket)
                    retry = False
                except:
                    pass

    def teardown_method(self, method):
        for socket in self.sockets:
            socket.close()
        self.server_process.kill()
        self.server_process.wait()

    def test_setup_and_teardown(self):
        pass
