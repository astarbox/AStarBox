#!/bin/bash
rm -rf astarbox
mkdir astarbox
cp installer/install.sh ./astarbox/
cp installer/batcharge.sh ./astarbox/

#
cp AStarBoxUI/* ./astarbox/
#
cp cliTools/10-i2c.rules ./astarbox/
cp cliTools/astarbox_config ./astarbox/
cp cliTools/astarbox_port ./astarbox/
cp cliTools/astarbox_volt ./astarbox/
cp cliTools/astarbox.service ./astarbox/
#
cp "powercontrollist AStarBox.txt" ./astarbox/
cp AStarBox.ui ./astarbox/
cp libAStarBox.so ./astarbox/
