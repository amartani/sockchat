PATH = File.dirname(__FILE__)
# system "cd #{PATH} && ./compile_client.sh"
require "#{PATH}/test/test_helper.rb"

class TestClient < Test::Unit::TestCase
  def setup
    @coordinator = FakeCoordinator.new
    @server      = FakeServer.new 6000
    @client      = FakeClient.new
    @coordinator.run
    @server.run
  end

  def teardown
    @coordinator.kill
    @server.kill
    @client.logout
  end

  def generate_other_clients
    ['Joao', 'Maria', 'Jose'].inject({}) do |hash, name|
      hash[name] = FakeClient.new
      hash[name].connect name
      hash
    end
  end

  def wait_for_timeout(&block)
    thread = Thread.start &block
    sleep 1
    alive = thread.alive?
    assert !alive
    thread.kill if alive
  end

  def test_ask_for_servers_to_coordinator
    wait_for_timeout do
      @client.ask_for_servers
      assert_equal @client.servers.size, 10
      assert_equal @client.servers.first.readable_ip, '127.0.0.1'
      assert_equal @client.servers.first.port, 6000
    end
  end

  def test_connect_to_server
    wait_for_timeout do
      @client.connect 'Joao da Silva'
      assert @server.clients.values.include?('Joao da Silva')
    end
  end

  def test_send_heartbeat
    wait_for_timeout do
      @client.heartbeat
      assert @server.received_heartbeat?
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
      message = "ECHO TEST MESSAGE!"
      asser_equal @client.echo(message), message
    end
  end

  def test_client_sends_and_other_receives_messages
    wait_for_timeout do
      @client.connect 'Mula Sem Cabeca'
      other_client = FakeClient.new
      other_client.connect 'Curupira'
      message = 'Voce viu minha cabeca? Acho que deixei la atras!'

      @client.send_message message
      other_client.listen
      assert_equal other_client.messages.last, "#{Mula Sem Cabeca}: #{message}"
    end
  end
end