#!/bin/env ruby

require 'shellwords'
require 'fileutils'
require 'nokogiri'
require 'base64'
require 'digest'
require 'csv'
require 'set'

$level = 'medium'

$sausages = CSV.read('../sausages.csv').flatten
$sauces = CSV.read('../sauces.csv').flatten
$breads = CSV.read('../breads.csv').flatten

$used_names = Set.new

iterations = (ENV['ITERATIONS'] || 1001).to_i
seed = (ENV['SEED'] || "#{$level} thousand cuts".unpack('Q').first).to_i

prng = Random.new seed

FileUtils.rm_rf 'tmp/gen'
%w{mk pov raw cb chk dist}.each{ |d| FileUtils.mkdir_p "tmp/gen/#{d}" }

pov_template = File.read('pov/pov-template.xml')

cb_orig_name = "bin/VITOG_00001"

puts ''

buildall = File.open("tmp/gen/buildall.sh", 'w')
buildall.puts '#!/bin/sh'

chkall = File.open("tmp/gen/chkall.sh", 'w')
chkall.puts '#!/bin/sh'

def pick_name(prng)
  choices = 3.times.map{ prng.rand(69) }
  while $used_names.include? choices
    choices = 3.times.map{ prng.rand(69) }
  end

  $used_names.add choices

  [
    $level,
    $sausages[choices[0]],
    'with',
    $sauces[choices[1]],
    'on',
    $breads[choices[2]]
  ].join('-')
end

names = iterations.times.map do |n|
  if 0 == (n % 10)
    data = "\r#{n}\t/ #{iterations}"
    print data
    buildall.puts "echo -n #{data.inspect}"
  end

  gets_buf_len = prng.rand(0x10..0x40)
  print_buf_len = prng.rand(0x10..0x40)
  echo_buf_len = prng.rand(0x10..0x80)
  canary_len = prng.rand(0x04...0x10)
  canary = prng.bytes(canary_len)
  while canary.include? "\0"
    canary = prng.bytes(canary_len)
  end

  canary = Base64.urlsafe_encode64 canary

  escaped_canary = canary.bytes.map{|b| "\\x#{b.to_s(16)}"}.join

  mk_buf = <<-EOF.gsub(/\s/, ' ').+("\n")
GENFLAGS =
-DGETS_BUF_LEN=#{gets_buf_len}
-DPRINT_BUF_LEN=#{print_buf_len}
-DECHO_BUF_LEN=#{echo_buf_len}
-DCANARY=#{Shellwords.escape canary.inspect}
EOF

  name = pick_name(prng)
  mk_name = "tmp/gen/mk/#{name}"
  pov_name = "tmp/gen/pov/#{name}.xml"
  raw_name = "tmp/gen/raw/#{name}"
  cb_name = "tmp/gen/cb/#{name}"
  chk_name = "tmp/gen/chk/#{name}.sh"

  File.open(mk_name, 'w') do |f|
    f.write mk_buf
  end

  File.open(pov_name, 'w') do |f|
    pov_write_data = ("A" * echo_buf_len) + escaped_canary + "\\x00" + ("A" * 16) + "\n"
    f.write pov_template.gsub(/DATA/, pov_write_data)
  end

  File.open(raw_name, 'w') do |f|
    raw_write_data = ("A" * echo_buf_len) + canary + "\0" + ("A" * 16) + "\n"
    f.write raw_write_data
  end

  ENV['MAKEFILES'] = mk_name
  buildall.puts "MAKEFILES=#{mk_name} make clean release > /dev/null"

  buildall.puts "cp #{cb_orig_name} #{cb_name}"
  buildall.puts "cp #{cb_name} tmp/gen/dist/"

  chk_buf = <<-EOF
#!/bin/sh
cb-test --timeout 2 --xml #{pov_name} --directory tmp/gen/cb --failure_ok --should_core --cb #{name} | tail -1
EOF

  File.open(chk_name, 'w') do |f|
    f.write chk_buf
  end

  File.chmod(0555, chk_name)

  chkall.puts chk_name

  name
end.sort

buildall.puts "cp tmp/gen/pov/#{names.first}.xml tmp/gen/dist/"

buildall.close
File.chmod(0500, 'tmp/gen/buildall.sh')

chkall.close
File.chmod(0500, 'tmp/gen/chkall.sh')
