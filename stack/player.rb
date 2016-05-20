require 'socket'
require 'base64'
require 'minitest/autorun'

class Player < Minitest::Test
  def setup
    sleep 1 if ENV.fetch('GO_SLOW', false)
    @host = ENV.fetch('HOST', 'localhost')
    @port = ENV.fetch('PORT', 21000)
    @flag = ENV.fetch('FLAG', "baby's first crs cirvyudta")
  end

  def test_runs_correctly
    sock = TCPSocket.new @host, @port
    sock.setsockopt(Socket::IPPROTO_TCP, Socket::TCP_NODELAY, true)
    assert_equal "send your crash string as base64, followed by a newline", sock.gets.strip

    10.times do
      binary_name = sock.gets.strip
      puts binary_name

      refute_equal "didn't pass canary, sorry", binary_name
      refute_equal "didn't segfault, sorry", binary_name

      assert_equal File.basename(binary_name), binary_name
      crasher_path = File.join 'tmp/gen/raw', binary_name
      assert File.exist? crasher_path
      crasher = File.read crasher_path
      encoded_crasher = Base64.strict_encode64 crasher

      sock.puts encoded_crasher
    end

    puts flag_message = sock.gets.strip

    assert_equal "The flag is: #{@flag}", flag_message

  end

  def test_stringblast
    sock = TCPSocket.new @host, @port
    sock.setsockopt(Socket::IPPROTO_TCP, Socket::TCP_NODELAY, true)
    assert_equal "send your crash string as base64, followed by a newline", sock.gets.strip

    binary_name = sock.gets.strip
    puts binary_name

    refute_equal "didn't pass canary, sorry", binary_name
    refute_equal "didn't segfault, sorry", binary_name

    assert_equal File.basename(binary_name), binary_name
    crasher_path = File.join 'tmp/gen/raw', binary_name
    assert File.exist? crasher_path

    crasher = 'A' * 1000
    encoded_crasher = Base64.strict_encode64 crasher

    sock.puts encoded_crasher

    crash_mesg = sock.gets.strip
    puts crash_mesg
    assert_equal "didn't pass canary, sorry", crash_mesg
  end

  def test_no_crash
    sock = TCPSocket.new @host, @port
    sock.setsockopt(Socket::IPPROTO_TCP, Socket::TCP_NODELAY, true)
    assert_equal "send your crash string as base64, followed by a newline", sock.gets.strip

    binary_name = sock.gets.strip
    puts binary_name

    refute_equal "didn't pass canary, sorry", binary_name
    refute_equal "didn't segfault, sorry", binary_name

    assert_equal File.basename(binary_name), binary_name
    crasher_path = File.join 'tmp/gen/raw', binary_name
    assert File.exist? crasher_path

    crasher = "hehe\n"
    encoded_crasher = Base64.strict_encode64 crasher

    sock.puts encoded_crasher

    crash_mesg = sock.gets.strip
    puts crash_mesg
    assert_equal "didn't segfault, sorry", crash_mesg
  end
end
