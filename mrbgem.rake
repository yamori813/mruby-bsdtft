MRuby::Gem::Specification.new('mruby-bsdtft') do |spec|
  spec.license = 'BSD'
  spec.authors = 'Hiroki Mori'

  spec.linker.libraries << ['gpio']
end
