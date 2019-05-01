class BsdTft

# for S6D0151
  
  def set_start_address(left, top)
    addr = (((top&0xff)<<8)|(left&0xff))
      
    regwr(0x0021 , addr )
  end
  
  def window (x, y, w, h) 
    hea=x+w-1;
    hsa=x;
    vea=y+h-1;
    vsa=y;
   
    dd=((hea&0xff)<<8)|(hsa&0xff)
    regwr(0x0044, dd )
    dd=((vea&0xff)<<8)|(vsa&0xff);
    regwr(0x0045, dd )
   
    set_start_address(hsa,vsa);
    reg0(0x0022);
  end
  
  def wr_dat_start
    arr = Array.new
    arr.push(0x72)
    return arr
  end
  
  def wr_dat_stop(arr)
    self.transfer(arr, 0)
  end
  
  def reg0(reg)
    arr = Array.new
    arr.push(0x70)
    arr.push(reg >> 8)
    arr.push(reg & 0xff)
    self.transfer(arr, 0)
  end
  
  def regwr(reg, data)
    arr = Array.new
    arr.push(0x70)
    arr.push(reg >> 8)
    arr.push(reg & 0xff)
    self.transfer(arr, 0)
  
    usleep(1)
  
    arr.clear
    arr.push(0x72)
    arr.push(data >> 8)
    arr.push(data & 0xff)
    self.transfer(arr, 0)
  end
   
  def tft_reset

    self.gpio_setflags(self.reset, BsdTft::OUTPUT)
    self.gpio_set(self.reset, 1)
    sleep 1
    self.gpio_set(self.reset, 0)
    sleep 1
    self.gpio_set(self.reset, 1)

    regwr(0x0007,0x0020)
    regwr(0x00b6,0x013f)
    regwr(0x00b4,0x0010)
    regwr(0x0012,0x00b2)
    regwr(0x0013,0x080e)
    regwr(0x0014,0x5bca)
    regwr(0x0061,0x0018)
    regwr(0x0010,0x190c)
    usleep(100)
    regwr(0x0013,0x081e)
    usleep(100)
    regwr(0x0001,0x0014)
    regwr(0x0002,0x0100)
    regwr(0x0003,0x0030)
    regwr(0x0008,0x0202)
    regwr(0x000b,0x0000)
    regwr(0x000c,0x0000)
    regwr(0x0061,0x0018)
    regwr(0x0069,0x0000)
    regwr(0x0070,0x0000)
    regwr(0x0071,0x0000)
    regwr(0x0011,0x0000)
    regwr(0x0030,0x0303)
    regwr(0x0031,0x0303)
    regwr(0x0032,0x0303)
    regwr(0x0033,0x0000)
    regwr(0x0034,0x0404)
    regwr(0x0035,0x0404)
    regwr(0x0036,0x0404)
    regwr(0x0037,0x0000)
    regwr(0x0038,0x0707)
    
    regwr(0x0040,0x0000)
    regwr(0x0042,0x9f00)
    regwr(0x0043,0x0000)
    regwr(0x0044,0x7f00)
    regwr(0x0045,0x9f00)
    regwr(0x0069,0x0000)
    regwr(0x0070,0x0000)
    regwr(0x0071,0x0000)
    regwr(0x0073,0x0000)
    regwr(0x00B3,0x0000)
    regwr(0x00BD,0x0000)
    regwr(0x00BE,0x0000)
    regwr(0x0021,0x0000)
    reg0(0x0022)
    
    usleep(100)
    regwr(0x0007,0x0020)
    usleep(100)
    regwr(0x0007,0x0021)
    regwr(0x0007,0x0027)
    usleep(100)
    regwr(0x0007,0x0037)
  end
    
  def cls(color)
    window(0,0,self.width,self.hight)
    arr = wr_dat_start
    self.width.times {
      arr.push(color >> 8)
      arr.push(color & 0xff)
    }
    wr_dat_stop(arr)
    (self.hight - 1).times {
      self.transfer(arr, 0)
    }
  end
  
  def lcdCopy(c)
    if self.model == BsdTft::S6D0151
      window(0,0,self.width,self.hight)
      self.transfer2(c)
    elsif self.model == BsdTft::ST7735
      write_cmd([0x2C])
      self.gpio_set(self.rs, 1)
      self.transfer2(c)
    end
  end

# for ST7735
  
  def init_st()

    self.gpio_setflags(self.reset, BsdTft::OUTPUT)	# reset
    self.gpio_setflags(self.rs, BsdTft::OUTPUT)	# self.rs
    self.gpio_set(self.reset, 1)
    sleep(1)
    self.gpio_set(self.reset, 0)
    sleep(1)
    self.gpio_set(self.reset, 1)
    sleep(1)
    write_cmd([0x11])
    usleep(100)
    write([0xB1], [0x01, 0x2C, 0x2D])
    write([0xB2], [0x01, 0x2C, 0x2D])
    write([0xB3], [0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D])
    write([0xB4], [0x07])
    write([0xC0], [0xA2, 0x02, 0x84])
    write([0xC1], [0xC5])
    write([0xC2], [0x0A, 0x00])
    write([0xC3], [0x8A, 0x2A])
    write([0xC4], [0x8A, 0xEE])
    write([0xC5], [0x0E])
    write([0x36], [0xC8])
    write([0xE0], [0x02, 0x1C, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2D, 0x29, 0x25, 0x2B, 0x39, 0x00, 0x01, 0x03, 0x10])
    write([0xE1], [0x03, 0x1D, 0x07, 0x06, 0x2E, 0x2C, 0x29, 0x2D, 0x2E, 0x2E, 0x37, 0x3F, 0x00, 0x00, 0x02, 0x10])
    write([0x2A], [0x00, 0x02, 0x00, 0x81])
    write([0x2B], [0x00, 0x01, 0x00, 0xA0])
    write([0x3A], [0x05])
    write_cmd([0x29])
  end

  def write_cmd(a)
    self.gpio_set(self.rs, 0)
    self.transfer(a, 0)
  end

  def write_data(a)
    self.gpio_set(self.rs, 1)
    self.transfer(a, 0)
  end

  def write(c, d)
    write_cmd(c)
    write_data(d)
  end

  def write_rgb(r, g, b)
    write_data([r & 0xF8 | g >> 5, g & 0xFC << 3 | b >> 3])
  end

  def fill(r, g, b)
    write_cmd([0x2C])
    arr = Array.new
    write([0x2A], [0x00, 0x02, 0x00, 0x81])
    write([0x2B], [0x00, 0x01, 0x00, 0xA0])
    write_cmd([0x2C])
    hi = r & 0xF8 | g >> 5
    lo = g & 0xFC << 3 | b >> 3
    for i in 1..160 do
      for n in 1..128 do
        arr.push(hi)
        arr.push(lo)
      end
      if i % 2 == 0 then
        write_data(arr);
        arr.clear
      end
    end
  end

  def copyspi(c)
    arr = Array.new
    for i in 1..160 do
      rgba = c.getpix(0, i - 1, 128)
      for n in 1..128 do
        arr.push(rgba[1 + (n - 1) * 4] & 0xF8 | rgba[2 + (n - 1) * 4] >> 5)
        arr.push(rgba[2 + (n - 1) * 4] & 0xFC << 3 | rgba[3 + (n - 1) * 4] >> 3)
      end
      if i % 4 == 0 then
        write_data(arr);
        arr.clear
      end
    end
  end

end
