#!/bin/bash

pushd ns-3.35/src/ofswitch13/lib/ofsoftswitch13/

./boot.sh
./configure --enable-ns3-lib
make

popd

