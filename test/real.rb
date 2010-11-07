# Metodos úteis de RealServer e RealClient:
#
#   gets    => retorna linha (até \n) encaminhada ao stdout do servidor
#   close   => fecha o servidor/cliente
#   kill    => fecha o servidor
#   logout  => fecha o servidor
#   closed? => retorna um booleano indicando se o servidor está aberto ou não

# Métodos úties para RealClient apenas
#
#   write   => escreve string no stdin do servidor (não escreve \n)
#   puts    => escreve string no stdin do servidor (coloca \n no final)

class RealServer < IO
  def popen(cmd)
    super "./#{PATH}/#{cmd}", 'r'
  end

  def kill
    close
  end
end

class RealClient < IO
  def popen(cmd)
    super "./#{PATH}/#{cmd}", 'r+'
  end

  def send_message(message)
    puts message
  end

  def logout
    close
  end
end