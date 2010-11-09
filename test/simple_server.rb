class SimpleServer < TCPServer
  def run
    @sessions ||= []
    @sockets  ||= []
    @runner = Thread.start do
      while(session = accept)
        @sessions << session
        # $stdout.puts 'User Connected'
        session.puts 'MERDA'
        Thread.start(session) do |s|
          while(cmd = s.read 1)
            request_handler s, cmd
          end
        end
      end
    end
  end

  def request_handler(session, cmd)
    $stdout.puts 'TEST HANDLER! Overwrite it!\n'
    $stdout.puts "  Debug: #{cmd.upcase} called!"
  end

  def kill
    @sessions.each(&:close) if @sessions
    # @sockets.each(&:close)  if @sockets
    @runner.kill            if @runner
    close
  end
end