require 'rubygems'
require 'test/unit'
require 'socket'
require 'bit-struct'
require 'monitor'

PATH = File.dirname File.dirname(__FILE__)

test_files = [ 'simple_server.rb', 'fake_client.rb', 'fake_server.rb', 'fake_coordinator.rb', 'util.rb', 'real_coordinator.rb', 'real_client.rb', 'real_server.rb' ]

test_files.each do |file|
  require "#{PATH}/test/#{file}"
end

class TestSockchat < Test::Unit::TestCase
  def teardown
    @coordinator.kill
    @server.kill
    @client.logout
  end

  def default_test
    assert @coordinator, 'Coordinator undefined'
    assert @server,      'Server undefined'
    assert @client,      'Client undefined'
  end

  def wait_for_timeout(timeout = 1, &block)
    timeout_thread = Thread.start do
      sleep timeout
      raise RuntimeError, "Execution exceeded timeout of #{timeout} seconds!"
    end
    block.call
    timeout_thread.kill if timeout_thread.alive?
  end
end