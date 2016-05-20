#!/bin/bash

pushd babysfirst
HOST=easy-prasky_335e35448b30ce7697fbb036cce45e34.quals.shallweplayaga.me PORT=10001 ruby player.rb
popd

pushd stack
HOST=334_cuts_22ffeb97cf4f6ddb1802bf64c03e2aab.quals.shallweplayaga.me PORT=10334 ruby player.rb
popd

pushd vary-stack
HOST=666_cuts_e38431570c1b4b397fa1026bb71a0576.quals.shallweplayaga.me PORT=10666 ruby player.rb
popd

pushd scramble-stack
HOST=1000_cuts_1bf4f5b0948106ad8102b7cb141182a2.quals.shallweplayaga.me PORT=11000 ruby player.rb
popd
