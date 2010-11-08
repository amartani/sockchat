DEBUG = true

PATH = File.dirname(__FILE__)
require "#{PATH}/test/test_helper.rb"

system "cd #{PATH} && ./compile_coordinator.sh" unless DEBUG

class TestCoordinator < TestSockchat
  def setup
    @coordinator = DEBUG ? FakeCoordinator.new : RealCoordinator.popen('coordinator.out')
    @server      = FakeServer.new 6000
    @client      = FakeClient.new
    @coordinator.run
    @server.run
  end

  def test_something
    skip
  end

end