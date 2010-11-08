require 'rubygems'
require 'test/unit'
require 'socket'
require 'bit-struct'
require 'monitor'

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

  def wait_for_timeout(timeout = 1, &block)
    lock = Monitor.new
    cond = lock.new_cond

    thread = Thread.start do
      block.call
      lock.synchronize{ cond.signal }
    end

    Thread.start do
      sleep timeout
      lock.synchronize{ cond.signal }
    end

    lock.synchronize do
      cond.wait
      alive = thread.alive?
      assert !alive
      thread.kill if alive
    end
  end
end