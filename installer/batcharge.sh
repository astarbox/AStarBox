#!/bin/bash

function installconfigBattery()
{
    if [ -f /boot/firmware/config.txt ]; then
        RTC_BATT_CHARGE_ENABLED=`cat /boot/firmware/config.txt| grep "dtparam=rtc_bbat_vchg=3000000"`
        
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
	
        if [ -z "${RTC_BATT_CHARGE_ENABLED}" ]; then
            sudo bash -c "echo -e 'dtparam=rtc_bbat_vchg=3000000\n' >> /boot/firmware/config.txt"
        fi
  
    else
        RTC_BATT_CHARGE_ENABLED=`cat /boot/config.txt| grep "dtparam=rtc_bbat_vchg=3000000"`
        
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
        if [ -z "${RTC_BATT_CHARGE_ENABLED}" ]; then
            sudo bash -c "echo -e 'dtparam=rtc_bbat_vchg=3000000\n' >> /boot/config.txt"
        fi
    fi
    echo -e "\n\n\n***** You need to reboot after this install *****\n\n\n"
}

installconfigBattery
BattOK=$?
if [ "$BattOK" = "0" ]; then
	echo "Battery charging will be enabled after next reboot"
else
	echo "Battery charging install failed"
fi
