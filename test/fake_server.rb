# Assuming that Server runs on port 6000, 6001, 6002...
# and Server that runs on 6000 will be used on test

class FakeServer < SimpleServer
  attr_accessor :coordinator, :clients, :heartbeats
  def initialize(port = 6000)
    super '127.0.0.1', port
    @coordinator = TCPSocket.new '127.0.0.1', 5000
    @heartbeats  = 0
    @clients     = {}
    @sockets     = [ @coordinator ]
  end

  def heartbeat
    @coordinator.write 'S'
  end

  def received_heartbeat?
    @heartbeats > 0
  end

  def coordinator_heartbeat
    @coordinator.write 'S'
  end

  def request_handler(session, cmd)
    case cmd
    when 'C'
      $stdout.puts 'Connection'
      client_name = Datum.get_from(session).data
      @clients ||= {}
      @clients[session] = client_name
    when 'M'
      message = Datum.string "#{@clients[session]}: #{Datum.get_from(session).data}"
      @clients.each do |s, name|
        if s != session
          s.write 'M'
          s.write message
        end
      end
    when 'H'
      @heartbeats += 1
    when 'L'
      session.write 'L'
      session.write UnsignedDatum[@clients.size]
      @clients.each do |s, name|
        session.write Datum.string(name)
      end
    when 'E'
      message = Datum.get_from(session).data
      session.write 'E'
      session.write message
    else
      raise RuntimeError, "Comando invalido: #{cmd}"
    end
  end
end