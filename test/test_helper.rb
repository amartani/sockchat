require 'rubygems'
require 'test/unit'
require 'socket'
require 'bit-struct'
require 'monitor'

[ 'simple_server.rb', 'fake_client.rb', 'fake_server.rb', 'fake_coordinator.rb', 'util.rb', 'real.rb' ].each do |file|
  require "#{PATH}/test/#{file}"
end