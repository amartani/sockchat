import socket
import subprocess
import struct
import time
import sys

PORT = 25864
PROGRAM = "./server.out"
N_CLIENTS = 10

def recv_string(sock):
    size_str = sock.recv(4)
    size = struct.unpack("I", size_str)[0]
    msg = sock.recv(size)
    return msg

def send_string(sock, string):
    sock.send(struct.pack("I", len(string)))
    sock.send(string)

def recv_int(sock):
    num_str = sock.recv(4)
    num = struct.unpack("I", num_str)[0]
    return num

def connect_socket():
    sock = None
    retry = 10
    while retry:
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.connect(('localhost', PORT))
            break
        except:
            retry -= 1
            if retry == 0:
                assert False, "Could not connect socket"
            time.sleep(1)
    sock.settimeout(2.0)
    return sock

class TestServer():

    def setup_method(self, method):
        self.server_process = subprocess.Popen([PROGRAM, str(PORT)], stdout=sys.stderr, stderr=sys.stderr)
        self.socket = connect_socket()

    def teardown_method(self, method):
        self.socket.close()
        self.server_process.kill()
        self.server_process.wait()
        time.sleep(1.0)

    def test_unknown(self):
        self.socket.send("P")
        msg = self.socket.recv(12)
        assert msg == "Desconhecido"

    def test_echo(self):
        test_string = "This is a test string!"
        self.socket.send("E")
        send_string(self.socket, test_string)
        assert test_string == recv_string(self.socket)

    def test_multiple_echo(self):
        test_string = "This is a test string!"
        self.socket.send("E")
        send_string(self.socket, test_string)
        assert test_string == recv_string(self.socket)
        test_string = "This is another test string!"
        self.socket.send("E")
        send_string(self.socket, test_string)
        assert test_string == recv_string(self.socket)

    def test_username(self):
        self.socket.send("C")
        send_string(self.socket, "Armando")

    def test_list(self):
        self.socket.send("C")
        send_string(self.socket, "Armando")
        self.socket.send("L")
        assert 1 == recv_int(self.socket)
        assert "Armando" == recv_string(self.socket)

class TestServerWithMultipleClients():

    def setup_method(self, method):
        self.server_process = subprocess.Popen([PROGRAM, str(PORT)])
        self.sockets = list(connect_socket() for i in range(10))

    def teardown_method(self, method):
        for socket in self.sockets:
            socket.close()
        self.server_process.kill()
        self.server_process.wait()
        time.sleep(1.0)

    def test_setup_and_teardown(self):
        pass

    def test_echo(self):
        number = 0
        for sock in self.sockets:
            sock.send("E")
            send_string(sock, "Test number %d." % number)
            number += 1
        number = 0
        for sock in self.sockets:
            assert ("Test number %d." % number) == recv_string(sock)
            number += 1

    def test_multiple_echo(self):
        self.test_echo()
        self.test_echo()

    def test_usernames(self):
        usernames = list("client %d" % i for i in range(10))
        for sock, username in zip(self.sockets, usernames):
            # Connect
            sock.send("C")
            send_string(sock, username)

    def test_list(self):
        usernames = list("client %d" % i for i in range(10))
        for sock, username in zip(self.sockets, usernames):
            # Connect
            sock.send("C")
            send_string(sock, username)
        time.sleep(1.0)
        for sock in self.sockets:
            sock.send("L")
            assert 10 == recv_int(sock)
            names = list()
            for i in range(10):
                name = recv_string(sock)
                names.append(name)
            names.sort()
            assert usernames == names



