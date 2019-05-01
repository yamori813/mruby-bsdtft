##
## BsdTft Test
##

assert("BsdTft#hello") do
  t = BsdTft.new "hello"
  assert_equal("hello", t.hello)
end

assert("BsdTft#bye") do
  t = BsdTft.new "hello"
  assert_equal("hello bye", t.bye)
end

assert("BsdTft.hi") do
  assert_equal("hi!!", BsdTft.hi)
end
