require 'rubygems'
require 'socket'
require 'bit-struct'
require 'monitor'

PATH = File.dirname File.dirname(__FILE__)

test_files = [ 'simple_server.rb', 'fake_client.rb', 'fake_server.rb', 'fake_coordinator.rb', 'util.rb', 'real_coordinator.rb', 'real_client.rb', 'real_server.rb' ]

test_files.each do |file|
  require "#{PATH}/test/#{file}"
end

def setup
  coord, server, client = FakeCoordinator.new, FakeServer.new, FakeClient.new
  coord.run
  server.run
  client.connect 'Stupid Client'
  [coord, server, client]
end