# Assuming that Coordinator runs on port 5000

class FakeCoordinator < SimpleServer
  def initialize
    super 'localhost', 5000
    @received_heartbeat = false
  end

  def received_heartbeat?
    @received_heartbeat
  end

  def request_handler(session, cmd)
    case cmd
    when 'C'
      session.write 'C'
      10.times do |n|
        session.write ServerConnectionInfo.by_params('127.0.0.1', 6000+n)
      end
    when 'S'
      @received_heartbeat = true
    else
      raise RuntimeError, "Comando invalido: #{cmd}"
    end
  end
end