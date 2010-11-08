# Assuming that Server runs on port 6000, 6001, 6002...
# and Server that runs on 6000 will be used on test

class FakeServer < SimpleServer
  attr_accessor :coordinator, :clients
  def initialize(port = 6000)
    super 'localhost', port
    @coordinator = TCPSocket.new 'localhost', 5000
    @received_heartbeat = false
  end

  def received_heartbeat?
    @received_heartbeat
  end

  def coordinator_heartbeat
    @coordinator.write 'S'
  end

  def request_handler(session, cmd)
    case cmd
    when 'C'
      client_name = Datum.get_from(session).data
      @clients ||= {}
      @clients[session] = client_name
    when 'M'
      message = Datum.string "#{@clients[session]}: #{Datum.get_from(session).data}"
      @clients.each do |s, name|
        s.write(message) if s != session
      end
    when 'H'
      @received_heartbeat = true
    when 'L'
      session.write UnsignedDatum[@clients.size]
      @clients.each do |s, name|
        session.write Datum.string(name)
      end
    when 'E'
      message = Datum.get_from(session)
      session.write message
    else
      raise RuntimeError, "Comando invalido: #{cmd}"
    end
  end
end