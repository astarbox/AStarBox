#!/usr/bin/env python3

import subprocess
import time
import os
import sys


switch_names = ["Port 1", "Port 2", "Port 3", "Port 4"]
slider_names = ["Dew Heater 1", "Dew Heater 2"]

def switch_set(switch_number, state):
    result = subprocess.run(["astarbox_port", str(switch_number),
                             "on" if state == 1 else "off"],
                            capture_output=True, text=True)
    print("Switch Number: ", switch_number, " State: ", state) # , result)

def pwm_set(slider_number, percentage):
    result = subprocess.run(["astarbox_port", "pwm" + str(slider_number),
                             str(percentage)],
                            capture_output=True, text=True)
    print("PWM: ", slider_number, " Percentage: ", percentage) #, result)

# Routine to turn on and gradually decrease pwm percentage
def pwm_loop(slider_number,  asleep):
    for percentage in range (100,-2,-2):
        pwm_set(slider_number, percentage)
        time.sleep(asleep)

def main():
	while True:
		# Loops over switches and turns them on and off for a second
		for switch_number in range(1,5,1):
			switch_set(switch_number, 1)
			time.sleep(1)
			switch_set(switch_number, 0)
			time.sleep(1)

		for slider_number in range(1,3,1):
			pwm_loop(slider_number, 0.2)

	return 0
    


if __name__ == "__main__":
    status = 0
    try:
	    status=main()
    except KeyboardInterrupt:
          pass
	# Shutdown command os.system("telinit 0")    
	# Restart command os.system("reboot")
    sys.exit(status)
