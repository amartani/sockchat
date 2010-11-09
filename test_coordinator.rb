DEBUG_COORDINATOR = true

require "#{File.dirname(__FILE__)}/test/test_helper.rb"
system "cd #{PATH} && ./compile_coordinator.sh" unless DEBUG_COORDINATOR

class TestCoordinator < TestSockchat
  def setup
    @coordinator = DEBUG_COORDINATOR ? FakeCoordinator.new : RealCoordinator.popen('coordinator.out')
    @server      = FakeServer.new 6000
    @client      = FakeClient.new
    @coordinator.run
    @server.run
  end

  def test_receives_server_heartbeat
    wait_for_timeout(1.5) do
      @server.heartbeat
      sleep 0.5
      assert @coordinator.received_heartbeat?, 'Heartbeat not received on coordinator'
    end
  end

  def test_processes_client_resquest_for_servers
    wait_for_timeout do
      @client.ask_for_servers
      assert_equal @client.servers.size, 10
      assert_equal @client.servers.first.readable_ip, '127.0.0.1'
      assert_equal @client.servers.first.port, 6000
    end
  end

end