class IntDatum < BitStruct
  signed :value, 32

  def self.[](value)
    instance       = self.new
    instance.value = value
    instance
  end
end

class UnsignedDatum < BitStruct
  unsigned :value, 32

  def self.[](value)
    instance       = self.new
    instance.value = value
    instance
  end
end

class Datum < BitStruct
  signed :datum_size, 32
  rest   :data

  def self.get_from(resource)
    instance            = self.new
    instance.datum_size = resource.read(4).to_unsigned
    instance.data       = resource.read instance.datum_size
    instance
  end

  def self.string(value)
    instance            = self.new
    instance.datum_size = value.length
    instance.data       = value
    instance
  end
end

class ServerConnectionInfo < BitStruct
  unsigned :ip,   32, :endian => :little
  unsigned :port, 16, :endian => :little

  def readable_ip=(string)
    self.ip = self.class.ip_to_value string
  end

  def readable_ip
    value = ip
    readable_ip = []
    4.times do
      readable_ip.unshift value % 256
      value /= 256
    end
    readable_ip.join('.')
  end

  def self.by_params(ip, port)
    instance             = self.new
    instance.readable_ip = ip
    instance.port        = port
    instance
  end

  def self.by_data(data)
    instance      = self.new
    instance.ip   = data.to_s[0..3].bytes.inject(0){ |s,x| s = s*256 + x }
    instance.port = data.to_s[4..5].bytes.inject(0){ |s,x| s = s*256 + x }
    instance
  end

  def self.ip_to_value(string)
    sprintf( "%02x%02x%02x%02x", *string.split('.').map(&:to_i) ).to_i(16)
  end
end

class String
  def to_unsigned
    self.bytes.map(&:to_i).inject(0){ |s,x| s = s*256 + x }
  end
end