class FakeClient
  attr_accessor :coordinator_socket, :server_socket, :servers, :messages, :echoed, :clients

  def initialize
    @servers  = []
    @messages = []
  end

  def ask_for_servers
    @coordinator_socket = TCPSocket.new '127.0.0.1', 5000
    @coordinator_socket.write 'C'
    match_command 'C', @coordinator_socket.read(1)
    @servers = 10.times.map do
      ServerConnectionInfo.by_data @coordinator_socket.read(20)
    end
    @coordinator_socket.close
  end

  def connect(name)
    @server_socket = TCPSocket.new '127.0.0.1', 6000
    @server_socket.write 'C'
    @server_socket.write Datum.string(name).to_s
  end

  def send_message(message)
    @server_socket.write 'M'
    @server_socket.write Datum.string(message)
  end

  def heartbeat
    @server_socket.write 'H'
  end

  def ask_for_clients
    @server_socket.write 'L'
    match_command 'L', @server_socket.read(1)
    number_of_clients = @server_socket.read(4).to_unsigned
    number_of_clients.times.map do
      Datum.get_from(@server_socket).data
    end
  end

  def echo(message)
    sent_message = Datum.string(message)
    @server_socket.write 'E'
    @server_socket.write sent_message
    match_command 'E', @server_socket.read(1)
    @echoed = Datum.get_from(@server_socket).data
  end

  def listen
    if @server_socket.read(1) == 'M'
      @messages << Datum.get_from(@server_socket).data
    end
  end

  def logout
    @server_socket.close if @server_socket
  end

  def match_command(expected, cmd)
    raise "Command Not Match! Expected #{expected}, but got #{cmd}" unless expected == cmd
  end
end