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
p BsdTft.hi
#=> "hi!!"
t = BsdTft.new "hello"
p t.hello
#=> "hello"
p t.bye
#=> "hello bye"
```

## License
under the BSD License:
- see LICENSE file
