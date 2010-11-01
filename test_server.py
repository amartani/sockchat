import socket
import subprocess
import struct
import time
import sys

PORT = 25864
PROGRAM = "./server.out"
N_CLIENTS = 10

def recv_string(socket):
    size_str = socket.recv(4)
    size = struct.unpack("I", size_str)[0]
    msg = socket.recv(size)
    return msg

def send_string(socket, string):
    socket.send(struct.pack("I", len(string)))
    socket.send(string)


class TestServer():

    def setup_method(self, method):
        self.server_process = subprocess.Popen([PROGRAM, str(PORT)], stdout=sys.stderr, stderr=sys.stderr)
        retry = 5
        while retry:
            try:
                self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                self.socket.connect(('localhost', PORT))
                break
            except:
                retry -= 1
                if retry == 0:
                    assert False
                time.sleep(1)
        self.socket.settimeout(1.0)

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

    def test_echo(self):
        test_string = "This is a test string!"
        self.socket.send("E")
        send_string(self.socket, test_string)
        assert test_string == recv_string(self.socket)

class Te_stServerWithMultipleClients():

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
