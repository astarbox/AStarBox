#!/bin/bash

function installPlugin() {
	TheSkyX_Install=~/Library/Application\ Support/Software\ Bisque/TheSkyX\ Professional\ Edition/TheSkyXInstallPath.txt
	echo "TheSkyX_Install = $TheSkyX_Install"

	if [ ! -f "$TheSkyX_Install" ]; then
		echo TheSkyXInstallPath.txt not found
		TheSkyX_Path=`/usr/bin/find ~/ -maxdepth 3 -name TheSkyX`
		if [ ! -d "$TheSkyX_Path" ]; then
		echo TheSkyX application was not found.
			return 1
		fi
	else
		TheSkyX_Path=$(<"$TheSkyX_Install")
	fi

	echo "Installing to $TheSkyX_Path"


	if [ ! -d "$TheSkyX_Path" ]; then
		echo TheSkyX Install dir not exist
		return 1
	fi

	if [ -d "$TheSkyX_Path/Resources/Common/PlugIns64" ]; then
		PLUGINS_DIR="PlugIns64"
	elif [ -d "$TheSkyX_Path/Resources/Common/PlugInsARM32" ]; then
		PLUGINS_DIR="PlugInsARM32"
	elif [ -d "$TheSkyX_Path/Resources/Common/PlugInsARM64" ]; then
		PLUGINS_DIR="PlugInsARM64"
	else
		PLUGINS_DIR="PlugIns"
	fi

	cp "./powercontrollist AStarBox.txt" "$TheSkyX_Path/Resources/Common/Miscellaneous Files/"
	cp "./AStarBox.ui" "$TheSkyX_Path/Resources/Common/$PLUGINS_DIR/PowerControlPlugIns/"
	cp "./libAStarBox.so" "$TheSkyX_Path/Resources/Common/$PLUGINS_DIR/PowerControlPlugIns/"

	app_owner=`/usr/bin/stat -c "%u" "$TheSkyX_Path" | xargs id -n -u`
	if [ ! -z "$app_owner" ]; then
		chown $app_owner "$TheSkyX_Path/Resources/Common/Miscellaneous Files/powercontrollist AStarBox.txt"
		chown $app_owner "$TheSkyX_Path/Resources/Common/$PLUGINS_DIR/PowerControlPlugIns/AStarBox.ui"
		chown $app_owner "$TheSkyX_Path/Resources/Common/$PLUGINS_DIR/PowerControlPlugIns/libAStarBox.so"
	fi
	chmod  755 "$TheSkyX_Path/Resources/Common/$PLUGINS_DIR/PowerControlPlugIns/libAStarBox.so"

}

function installUI() 
{
	sudo mkdir -p /usr/local/bin/
	sudo cp -f AStarBoxUI.py /usr/local/bin/
	sudo mkdir -p /usr/share/pixmaps/
	sudo cp -f AStarBoxLogoSmall.png /usr/share/pixmaps
	sudo cp -f AStarBoxIcon.png /usr/share/pixmaps
	cp AStarBox.desktop ~/Desktop
	if [ ! -f /etc/astarbox.names ]; then
	    # Default port names
	    sudo echo "Port 1:Port 2:Port 3:Port 4:Dew 1:Dew 2" > /tmp/astarbox.names
	    sudo cp /tmp/astarbox.names /etc/
	    sudo rm /tmp/astarbox.names
	    sudo chmod 666 /etc/astarbox.names
	fi
	sudo desktop-file-install ~/Desktop/AStarBox.desktop
	sudo update-desktop-database
}


function installCliTools() 
{
	sudo mkdir -p /usr/local/bin/
	sudo cp -f astarbox_config /usr/local/bin/
	sudo cp -f astarbox_port /usr/local/bin/
	sudo cp -f astarbox_volt /usr/local/bin/
	sudo cp -f astarbox.service /lib/systemd/system/
	sudo systemctl daemon-reload
	if [ ! -f /etc/astarbox.conf ]; then
		# all ports off by default
		sudo touch /etc/astarbox.conf
		sudo chmod 666 /etc/astarbox.conf
		echo "0:0:0:0:0:0" > /etc/astarbox.conf
	fi
	sudo usermod -a -G i2c `whoami`
	sudo cp 10-i2c.rules /etc/udev/rules.d
	sudo udevadm control --reload
	sudo udevadm trigger --attr-match=subsystem=i2c-dev
	sudo systemctl enable astarbox
	sudo systemctl stop astarbox >/dev/null 2>&1 || true
	sudo systemctl start astarbox >/dev/null 2>&1  || true
}

function configPi()
{
	IS_DEBIAN=`cat /etc/os-release | grep ID | grep -i debian`
	IS_ARCH=`cat /etc/os-release | grep ID | grep -i archarm`
	EEPROM_CONFIG_PRESENT=`which rpi-eeprom-config`
	# check for debian type install
	if [ ! -z "${IS_DEBIAN}" ]; then 
		if [ -z "${EEPROM_CONFIG_PRESENT}" ]; then
			sudo apt install -fy rpi-eeprom
		fi

		PSU_MAX_PRESENT=`rpi-eeprom-config | grep PSU_MAX_CURRENT`
		if [ -z "${PSU_MAX_PRESENT}" ]; then
			sudo rpi-eeprom-config >/tmp/boot.conf
			echo "PSU_MAX_CURRENT=5000" >> /tmp/boot.conf
			sudo rpi-eeprom-config  --apply /tmp/boot.conf
		fi

		PCIE_PROBE_ENABLE=`rpi-eeprom-config | grep PCIE_PROBE`
		if [ -z "${PCIE_PROBE_ENABLE}" ]; then
			sudo rpi-eeprom-config >/tmp/boot.conf
			echo "PCIE_PROBE=1" >> /tmp/boot.conf
			sudo rpi-eeprom-config  --apply /tmp/boot.conf
		fi
	# check for AstroArch
	elif [ ! -z "${IS_ARCH}" ]; then
		if [ -z "${EEPROM_CONFIG_PRESENT}" ]; then
			echo "You need to add this line to the PI 5 eeprom config"
			echo "PSU_MAX_CURRENT=5000"
			echo "rpi-eeprom-config was not found on your system and is needed to update"
			echo "the Raspberry PI 5 eeprom configuration to enable full power (5V @5A)."
			echo "You can find rpi-eeprom here : https://github.com/raspberrypi/rpi-eeprom"
		fi
	fi

	# add our stuff at the end of config.txt
	if [ -f /boot/firmware/config.txt ]; then
		I2C_ENABLED=`cat /boot/firmware/config.txt| grep "dtparam=i2c_arm=on" | grep -v "#"`
		USB_MAX_ENABLED=`cat /boot/firmware/config.txt| grep "usb_max_current_enable=1"`
        
		all_last=""
		while IFS= read -r line
		do
			case "$line" in
				\[*\]*) all_last=`echo  $line | grep "[all]"` ;;
			esac
		done < /boot/firmware/config.txt

		if [ -z "${all_last}" ]; then
			sudo bash -c "echo -e '\n[all]\n' >> /boot/firmware/config.txt"
		fi

		if [ -z "${USB_MAX_ENABLED}" ]; then
			sudo bash -c "echo -e 'usb_max_current_enable=1\n' >> /boot/firmware/config.txt"
		fi
		
		if [ -z "${I2C_ENABLED}" ]; then
			sudo bash -c "echo -e 'dtparam=i2c_arm=on\n' >> /boot/firmware/config.txt"
		fi
  
	else
		I2C_ENABLED=`cat /boot/config.txt| grep "dtparam=i2c_arm=on" | grep -v "#"`
		USB_MAX_ENABLED=`cat /boot/config.txt| grep "usb_max_current_enable=1"`
        
		all_last=""
		while IFS= read -r line
		do
			case "$line" in
				\[*\]*) all_last=`echo  $line | grep "[all]"` ;;
			esac
		done < /boot/config.txt

		if [ -z "${all_last}" ]; then
			sudo bash -c "echo -e '\n[all]\n' >> /boot/config.txt"
		fi

		if [ -z "${USB_MAX_ENABLED}" ]; then
			sudo bash -c "echo -e 'usb_max_current_enable=1\n' >> /boot/config.txt"
		fi
		if [ -z "${I2C_ENABLED}" ]; then
			sudo bash -c "echo -e 'dtparam=i2c_arm=on\n' >> /boot/config.txt"
		fi
	fi
	echo -e "\n\n\n***** You need to reboot after this install *****\n\n\n"
}

configPi

installUI
UIOk=$?
if [ "$UIOk" = "0" ]; then
	echo "AStarBox UI tools and config installed"
else
	echo "AStarBox UI tools and config installed failed"
fi

installCliTools
cliOk=$?
if [ "$cliOk" = "0" ]; then
	echo "AStarBox cli tools and config installed"
else
	echo "AStarBox cli tools and config installed failed"
fi

installPlugin
pluginOk=$?
if [ "$pluginOk" = "0" ]; then
	echo "AStarBox TheSKyX X2 plugin installed"
else
	echo "AStarBox TheSKyX X2 plugin NOT installed"
fi
