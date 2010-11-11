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
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(('localhost', PORT))
    sock.settimeout(2.0)
    return sock

def start_server():
    server = subprocess.Popen([PROGRAM, str(PORT)])
    time.sleep(0.5)
    if server.poll() == None:
        return server
    raise Exception("Nao deu pra subir o server")

class TestServer():

    def setup_method(self, method):
        self.server_process = start_server()
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
        assert "E" == self.socket.recv(1)
        assert test_string == recv_string(self.socket)

    def test_multiple_echo(self):
        test_string = "This is a test string!"
        self.socket.send("E")
        send_string(self.socket, test_string)
        assert "E" == self.socket.recv(1)
        assert test_string == recv_string(self.socket)
        test_string = "This is another test string!"
        self.socket.send("E")
        send_string(self.socket, test_string)
        assert "E" == self.socket.recv(1)
        assert test_string == recv_string(self.socket)

    def test_username(self):
        self.socket.send("C")
        send_string(self.socket, "Armando")

    def test_list(self):
        self.socket.send("C")
        send_string(self.socket, "Armando")
        self.socket.send("L")
        assert "L" == self.socket.recv(1)
        assert 1 == recv_int(self.socket)
        assert "Armando" == recv_string(self.socket)

    def test_list_multiple(self):
        self.socket.send("C")
        send_string(self.socket, "Armando")
        self.socket.send("L")
        assert "L" == self.socket.recv(1)
        assert 1 == recv_int(self.socket)
        assert "Armando" == recv_string(self.socket)
        self.socket.send("L")
        assert "L" == self.socket.recv(1)
        assert 1 == recv_int(self.socket)
        assert "Armando" == recv_string(self.socket)

class TestServerWithMultipleClients():

    def setup_method(self, method):
        self.server_process = start_server()
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
            cmd = sock.recv(1)
            assert "E" == cmd, number
            str_recv = recv_string(sock)
            assert ("Test number %d." % number) == str_recv
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
        self.test_usernames()
        time.sleep(1.0)
        for sock in self.sockets:
            sock.send("L")
            cmd = sock.recv(1)
            assert "L" == cmd
            users = recv_int(sock)
            assert 10 == users
            names = list()
            for i in range(10):
                name = recv_string(sock)
                names.append(name)
            names.sort()
            assert usernames == names

    def test_disconnection(self):
        self.test_usernames()
        time.sleep(1.0)
        disconnected = self.sockets.pop(-1)
        disconnected.close()
        for i in range(2):
            for sock in self.sockets:
                sock.send("H")
            time.sleep(4.0)
        usernames = list("client %d" % i for i in range(9))
        for sock in self.sockets:
            sock.send("L")
            cmd = sock.recv(1)
            assert "L" == cmd
            users = recv_int(sock)
            assert 9 == users
            names = list()
            for i in range(9):
                name = recv_string(sock)
                names.append(name)
            names.sort()
            assert usernames == names

#    def test_message(self):
#        self.test_usernames()
#        message = "TEST MESSAGE"
#        sender = self.sockets[0]
#        sender.send("M")
#        send_string(sender, message)
#        for sock in self.sockets:
#            assert "M" == sock.recv(1)
#            assert "client 0" == recv_string(sock)
#            assert message == recv_string(sock)


