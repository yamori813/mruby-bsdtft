# mruby-bsdtft   [![Build Status](https://travis-ci.org/yamori813/mruby-bsdtft.svg?branch=master)](https://travis-ci.org/yamori813/mruby-bsdtft)
BsdTft class
## install by mrbgems
- add conf.gem line to `build_config.rb`

```ruby
MRuby::Build.new do |conf|

    # ... (snip) ...

    conf.gem :github => 'yamori813/mruby-bsdtft'
end
```
## example
```ruby
W = 128
H = 160

RST = 21

tft = BsdTft.new(1, 0, BsdTft::S6D0151)

tft.setsize(W, H)
tft.setreset(RST)
tft.init

tft.cls(0)

c = Cairo.new(W, H)
c.set_source_rgb(1 , 0, 0)
c.move_to(0, 0)
c.line_to(100, 100)
c.stroke()

tft.lcdCopy(c.get_data)
```

## License
under the BSD License:
- see LICENSE file
