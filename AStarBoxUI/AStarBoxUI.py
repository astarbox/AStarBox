#!/usr/bin/env python

import tkinter as tk
from tkinter import ttk

import subprocess

import os

switch_names = ["Port 1", "Port 2", "Port 3", "Port 4"]
slider_names = ["Dew Heater 1", "Dew Heater 2"]

class EditableCheckbox(tk.Frame):
    def __init__(self, master=None, text="", initial_state=False, switch_var=None, switch_number=0):
        super().__init__(master)
        self.master = master
        self.text = tk.StringVar(value=text)
        self.state = tk.BooleanVar(value=initial_state)
        self.editing = False
        self.checkbox = tk.Checkbutton(self, textvariable=self.text, variable=switch_var, command=lambda: on_switch_change(switch_number))
        self.checkbox.pack(side="left")

        self.checkbox.bind("<Double-Button-1>", self.start_editing)

    def start_editing(self, event):
        if self.editing == False:
            self.editing = True
            self.entry = tk.Entry(self, textvariable=self.text)
            self.entry.pack(side="left")
            self.entry.bind("<Return>", self.stop_editing)
            self.entry.focus_set()

    def stop_editing(self, event):
        self.editing = False
        write_names()
        self.entry.destroy()


class EditableLabel(tk.Frame):
    def __init__(self, master=None, text=""):
        super().__init__(master)
        self.master = master
        self.text = tk.StringVar(value=text)
        self.editing = False

        self.label = tk.Label(self, textvariable=self.text)
        self.label.pack(side="left")

        self.label.bind("<Double-Button-1>", self.start_editing)

    def start_editing(self, event):
        if self.editing == False:
            self.editing = True
            self.entry = tk.Entry(self, textvariable=self.text)
            self.entry.pack(side="left")
            self.entry.bind("<Return>", self.stop_editing)
            self.entry.focus_set()
            
    def stop_editing(self, event):
        self.editing = False
        write_names()
        self.entry.destroy()



def on_switch_change(switch_number):
    switch_states[switch_number] = switch_vars[switch_number].get()
    result = subprocess.run(["astarbox_port", str(switch_number+1), "on" if switch_states[switch_number] == 1 else "off"], capture_output=True, text=True)
    write_conf()

def on_slider_change(slider_number, event):
    pwm_states[slider_number-1] = int(4096*int(float(event))/100)
    result = subprocess.run(["astarbox_port", "pwm" + str(slider_number), event], capture_output=True, text=True)
    write_conf()


def update_external_command_label():
    result = execute_external_command()
    ressplit = result.split(' ')
    voltage = ressplit[0]
    external_command_label.config(text=f"Input voltage: {voltage}v")
    root.after(1000, update_external_command_label)  # Schedule the function to run after 1000 milliseconds (1 second)

def execute_external_command():
    # Replace 'your_command_here' with the actual command you want to execute
    result = subprocess.check_output('astarbox_volt', shell=True, text=True)
    return result.strip()

def read_names():
    global switch_names, slider_names
    if os.path.isfile("/etc/astarbox.names"):
        fnames = open('/etc/astarbox.names', 'r')
        txtsplit = fnames.readline().split(':')
        fnames.close()
        switch_names = [txtsplit[i] for i in range(4)]
        slider_names = [txtsplit[i] for i in [4, 5]]


def write_names():
    if (True):
        string = switches[0].checkbox.cget("text")
        for i in range(1,4):
            string += ':' + switches[i].checkbox.cget("text")
            
        string += ':' + str(label_a.label.cget("text"))
        string += ':' + str(label_b.label.cget("text"))
        fnames = open('/etc/astarbox.names', 'w')
        fnames.write(string)
        fnames.close()
        
def read_conf():
    global switch_states, pwm_states
    fconf = open('/etc/astarbox.conf', 'r')
    txtsplit = fconf.readline().split(':')
    fconf.close()
    switch_states = [int(txtsplit[i]) for i in range(4)]
    pwm_states = [int(txtsplit[i]) for i in [4, 5]]

def write_conf():
    if (True):
        string = str(switch_states[0])
        for i in range(1,4):
            string += ':' + str(switch_states[i])
        for i in range(2):
            string += ':' + str(pwm_states[i])
                
        fconf = open('/etc/astarbox.conf', 'w')
        fconf.write(string)
        fconf.close()


    
    
# Create main window
root = tk.Tk()
root.title("AStarBox Power Control")

# Add Icon to main window
p1 = tk.PhotoImage(file = "/usr/share/pixmaps/AStarBoxIcon.png")
# Icon set for program window
root.iconphoto(False, p1) 

# Variables for on/off switches
switch_vars = [tk.IntVar() for _ in range(4)]

# Read initial switch and pwm states from conf file
read_conf()

# Read initial switch and pwm names
read_names()



# Style configuration for switches
style = ttk.Style()

# Set color for "OFF" state
#style.configure("TCheckbutton", background="black", foreground="#000000", indicatoron=False)

# Set color for "ON" state
style.map("TCheckbutton", background=[("selected", "#00FF00")])

1
# Create on/off switches with initial states
switches = []
for i in range(4):
    switch = EditableCheckbox(root, text=switch_names[i], initial_state=True if switch_states[i] == 1 else False, switch_var=switch_vars[i], switch_number=i)
    switch.grid(row=1, column=i*2, columnspan=2, padx=10, pady=10)
    switch_vars[i].set(switch_states[i])
    switches.append(switch)

# Create sliders with labels and set initial values
label_a = EditableLabel(root, text=slider_names[0])
label_a.grid(row=2, column=1, padx=10, pady=10)

slider1 = ttk.Scale(root, from_=0, to=100, orient="horizontal", command=lambda value: on_slider_change(1, value))
slider1.grid(row=2, column=2, columnspan=2, padx=10, pady=10)
slider1.set(pwm_states[0]/4096*100)  # Set initial value for slider A

label_b = EditableLabel(root, text=slider_names[1])
label_b.grid(row=2, column=4, padx=10, pady=10)

slider2 = ttk.Scale(root, from_=0, to=100, orient="horizontal", command=lambda value: on_slider_change(2, value))
slider2.grid(row=2, column=5, columnspan=2, padx=10, pady=10)
slider2.set(pwm_states[1]/4096*100)  # Set initial value for slider B

# Load and embed a logo
logo_image = tk.PhotoImage(file="/usr/share/pixmaps/AStarBoxLogoSmall.png")  # Provide the actual path to your logo file
logo_label = tk.Label(root, image=logo_image)
logo_label.grid(row=0, column=0, columnspan=8, pady=10)

# Create a label to display the result of the external command
external_command_label = tk.Label(root, text="External Command Result: ", padx=10, pady=10)
external_command_label.grid(row=3, column=0, columnspan=8)

# Uppdate the voltage immediately, then set to update every second
root.after(1, update_external_command_label)

# Run the application
root.mainloop()
