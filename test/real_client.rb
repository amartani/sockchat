# Metodos úteis para RealClient:
#
#   gets    => retorna linha (até \n) encaminhada ao stdout do servidor
#   close   => fecha o servidor/cliente
#   kill    => fecha o servidor
#   logout  => fecha o servidor
#   closed? => retorna um booleano indicando se o servidor está aberto ou não
#   write   => escreve string no stdin do servidor (não escreve \n)
#   puts    => escreve string no stdin do servidor (coloca \n no final)

class RealClient < IO
  attr_accessor :servers, :messages

  def puts(*args)
    super
    flush
  end

  def self.popen(cmd)
    @messages = []
    super "#{PATH}/./#{cmd}", 'r+'
  end

  def ask_for_servers
    @servers = get_servers
  end

  def connect(name, server = 0)
    puts server
    puts name
  end

  def send_message(message)
    puts 'M'
    puts message
  end

  def heartbeat
    sleep 0.5
  end

  def ask_for_clients
    puts 'L'
    # Forma: "2\nFulano\nSicrano"
    read(1).to_i.times.map do
      gets.chomp
    end
  end

  def echo(message)
    puts 'E'
    # Forma: "ECHO: #{message}"
    puts message
    gets.chomp[6..-1]
  end

  def listen
    @messages << gets.chomp
  end

  def logout
    close unless closed?
  end

  protected
  def get_servers
    10.times.map do
      # Forma: "IP: 127.0.0.1 Porta: 6000\n"
      line = gets
      ip   = line.match(/\d+\.\d+\.\d+\.\d+/)[0]
      port = line.match(/Porta\:\s+(\d+)/)[1].to_i
      ServerConnectionInfo.by_params ip, port
    end
  end
end