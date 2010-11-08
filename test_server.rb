DEBUG_SERVER = true

require "#{File.dirname(__FILE__)}/test/test_helper.rb"
system "cd #{PATH} && ./compile_server.sh" unless DEBUG_SERVER

class TestServer < TestSockchat
  def setup
    @coordinator = FakeCoordinator.new
    @server      = DEBUG_SERVER ? FakeServer.new(6000) : RealServer.popen('server.out 6000')
    @client      = FakeClient.new
    @coordinator.run
    @server.run
  end

  def generate_other_clients
    clients = ['Joao', 'Maria', 'Jose'].inject({}) do |hash, name|
      hash[name] = FakeClient.new
      hash[name].connect name
      hash
    end
  end

  def test_connect_to_server
    wait_for_timeout(1.5) do
      @client.connect 'Joao da Silva'
      sleep 0.5
      assert @server.clients.values.include?('Joao da Silva'), 'Client not connected on server'
    end
  end

  def test_send_heartbeat
    wait_for_timeout(1.5) do
      @client.connect 'HeartBeater'
      @client.heartbeat
      sleep 0.5
      assert @server.received_heartbeat?, 'Heartbeat not received on server'
    end
  end
  
  def test_ask_for_clients
    wait_for_timeout do
      @client.connect 'Ze do Caixao'
      other_clients = generate_other_clients
      @client.ask_for_clients.each do |name|
        assert other_clients[name] if name != 'Ze do Caixao'
      end
      other_clients.values.each{ |c| c.logout }
    end
  end
  
  def test_echo
    wait_for_timeout do
      @client.connect 'PingPong Tester'
      message = "ECHO TEST MESSAGE!"
      assert_equal @client.echo(message), message, 'Message not echoed'
    end
  end
  
  def test_clients_can_communicate_using_server
    wait_for_timeout do
      @client.connect 'Mula Sem Cabeca'
      other_client = FakeClient.new
      other_client.connect 'Curupira'
      message = 'Voce viu minha cabeca? Acho que deixei la atras!'
  
      @client.send_message message
      other_client.listen
      assert_equal other_client.messages.last, "Mula Sem Cabeca: #{message}"
    end
  end

end