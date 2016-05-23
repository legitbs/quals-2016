#!/bin/sh

rm -rf thousand_cuts thousand_cuts.tar.bz2
cp -r tmp/gen/dist thousand_cuts
tar jcf thousand_cuts.tar.bz2 thousand_cuts
