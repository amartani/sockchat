DEBUG = true

PATH = File.dirname(__FILE__)
require "#{PATH}/test/test_helper.rb"

system "cd #{PATH} && ./compile_server.sh" unless DEBUG

class TestServer < TestSockchat
  def setup
    @coordinator = FakeCoordinator.new
    @server      = DEBUG ? FakeCoordinator.new : RealServer.popen('server.out 6000')
    @client      = FakeClient.new
    @coordinator.run
    @server.run
  end

  def test_something
    skip
  end

end