class RealServer < IO
  attr_accessor :coordinator, :clients
  def popen(cmd)
    super "./#{PATH}/#{cmd}", 'r'
  end

  def run
    @received_heartbeat = false
    @clients = []
    @parsing_thread = Thread.start do
      loop do
        line = gets
        @received_heartbeat = true if line.match /HEARTBEAT/
        match = line.match /CONNECTED:\s*\"(.*)\"/
        if match
          @clients << match[1]
        end
      end
    end
  end

  def received_heartbeat?
    @received_heartbeat
  end

  def kill
    @parsing_thread.kill if @parsing_thread
    Process.kill 'QUIT', self.pid
    close unless closed?
  end
end