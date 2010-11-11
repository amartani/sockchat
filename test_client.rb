DEBUG_CLIENT = false

require "#{File.dirname(__FILE__)}/test/test_helper.rb"
system "cd #{PATH} && ./compile_client.sh" unless DEBUG_CLIENT

class TestClient < TestSockchat
  def setup
    @coordinator = FakeCoordinator.new
    @server      = FakeServer.new 6000
    @client      = DEBUG_CLIENT ? FakeClient.new : RealClient.popen('client')
    @coordinator.run
    @server.run
    @client.ask_for_servers
  end

  def generate_other_clients
    clients = ['Joao', 'Maria', 'Jose'].inject({}) do |hash, name|
      hash[name] = FakeClient.new
      hash[name].ask_for_servers
      hash[name].connect name
      hash
    end
  end

  def test_ask_for_servers_to_coordinator
    wait_for_timeout do
      assert_equal @client.servers.size, 10
      @client.servers.each_with_index do |server, index|
        assert_equal server.readable_ip, '127.0.0.1'
        assert_equal server.port, 6000+index
      end
    end
  end

  def test_connect_to_server
    wait_for_timeout(1.5) do
      @client.connect 'Joao da Silva'
      sleep 0.5
      assert !@server.clients.empty?, 'Any connected client'
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
      @client.echo message
      assert_equal @client.echoed, message, 'Message not echoed'
    end
  end

  def test_client_sends_and_other_receives_messages
    wait_for_timeout do
      @client.connect 'Mula Sem Cabeca'
      other_client = FakeClient.new
      other_client.ask_for_servers
      other_client.connect 'Curupira'
      message = 'Voce viu minha cabeca? Acho que deixei la atras!'

      @client.send_message message
      other_client.listen
      other_client.logout
      assert_equal other_client.messages.last, "Mula Sem Cabeca: #{message}"
    end
  end

  def test_other_sends_and_client_receives_messages
    wait_for_timeout do
      @client.connect 'Mula Sem Cabeca'
      other_client = FakeClient.new
      other_client.ask_for_servers
      other_client.connect 'Curupira'
      message = 'Achei, vou passar de calcanhar para voce!'
  
      other_client.send_message message
      other_client.logout
      @client.listen
      assert_equal @client.messages.last, "Curupira: #{message}"
    end
  end
end