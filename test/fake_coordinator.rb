# Assuming that Coordinator runs on port 5000

class FakeCoordinator < SimpleServer
  attr_accessor :heartbeats
  def initialize
    super '127.0.0.1', 5000
    @heartbeats = 0
    @sockets    = []
  end

  def received_heartbeat?
    @heartbeats > 0
  end

  def request_handler(session, cmd)
    case cmd
    when 'C'
      session.write 'C'
      10.times do |n|
        session.write ServerConnectionInfo.by_params('127.0.0.1', 5000+n)
      end
    when 'S'
      @heartbeats += 1
    else
      raise RuntimeError, "Comando invalido: #{cmd}"
    end
  end
end