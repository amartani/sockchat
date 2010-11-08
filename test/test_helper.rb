require 'rubygems'
require 'test/unit'
require 'socket'
require 'bit-struct'
require 'monitor'

test_files = [ 'simple_server.rb', 'fake_client.rb', 'fake_server.rb', 'fake_coordinator.rb', 'util.rb', 'real.rb', 'real_client.rb' ]

test_files.each do |file|
  require "#{PATH}/test/#{file}"
end