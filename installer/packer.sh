#!/bin/bash

rm -f *.run
rm -f *.gz

binarycreator --offline-only -c config/config.xml -p packages opensocks-ui-linux-amd64.run
