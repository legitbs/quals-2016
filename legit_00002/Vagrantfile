# vi: set ft=ruby
    
nodes = {
    'cb'  => {:ip => ['172.16.0.10'], :memory => 512, :primary => false}, 
    'ids' => {:ip => ['172.16.0.11', '172.16.0.12'], :memory => 256, :primary => false},
    'pov' => {:ip => ['172.16.0.13', '172.16.0.14'], :memory => 512, :primary => false}, 
    'crs' => {:ip => ['172.16.0.15'], :memory => 1024, :primary => true},
    'ti'  => {:ip => ['172.16.0.16'], :memory => 512, :primary => false}
}

Vagrant.configure(2) do |config|
    config.vm.box = "cgc-linux-dev"
    config.vm.box_url = 'http://s3.amazonaws.com/cgcdist/boxes/vm.json'

    nodes.each_key do |name|
        config.vm.define "#{name}", primary: nodes[name][:primary] do |node|
            nodes[name][:ip].each do |ip_addr|
                node.vm.network :private_network, ip: ip_addr
            end
            node.vm.hostname = "#{name}"
        
            node.vm.provider :virtualbox do |vb|
                # vb.gui = true
                vb.customize ["modifyvm", :id, "--memory", nodes[name][:memory]] if nodes[name][:memory]
                vb.customize ["modifyvm", :id, "--ioapic", "on"]
            end
        end

        config.vm.provision "shell", inline: "sudo su -c 'echo #{nodes[name][:ip][0]} #{name} >> /etc/hosts'"

    end
    
    
    # force Vagrant to keep the insecure key, such that we can automatically login from
    # one node to another.
    config.ssh.insert_key = false
    config.ssh.private_key_path = "#{ ENV['HOME'] }/.vagrant.d/insecure_private_key"

    config.vm.provision "file", source: "#{ ENV['HOME'] }/.vagrant.d/insecure_private_key", destination: "/home/vagrant/.ssh/id_rsa"
    config.vm.provision :shell, inline: "echo StrictHostKeyChecking no > /home/vagrant/.ssh/config"

    # avoids 'stdin: is not a tty' error.
    # config.ssh.shell = "bash -c 'BASH_ENV=/etc/profile exec bash'" 

    # if you have a ~/.vimrc or ~/.gdbinit, copy those to the default user
    [".profile", ".bashrc", ".vimrc", ".gdbinit", ".quilt-dpkg"].each { |filename|
        local_path = "#{ ENV['HOME'] }/#{filename}"
        vm_path = "/home/vagrant/#{filename}"
        if File.file?(local_path)
            config.vm.provision "file", source: local_path, destination: vm_path
        end
    }
end

# Vagrant 1.5 does not deal well with the SSH agent storing signed keys.
# This monkey-patch removes support for the agent until Vagrant 1.6 is
# available.
require 'net/ssh'
class Net::SSH::Authentication::KeyManager
    def use_agent?; false; end
    def use_agent=(agent); @use_agent = false; end
    def agent; nil; end
end
