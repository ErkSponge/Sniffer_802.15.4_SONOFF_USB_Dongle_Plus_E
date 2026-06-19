

# 802.15.4 Sniffer

## Supported Dongles

- SONOFF Zigbee 3.0 USB DONGLE Plus-E
- SONOFF Dongle Lite MG21 Zigbee/Thread USB Dongle (Dongle-LMG21)
- SMLIGHT SLZB-07

---

## Contents
- [802.15.4 Sniffer](#802154-sniffer)
  - [Supported Dongles](#supported-dongles)
  - [Contents](#contents)
  - [What's needed](#whats-needed)
  - [Supported dongles](#supported-dongles-1)
    - [SONOFF Zigbee 3.0 USB DONGLE Plus-E](#sonoff-zigbee-30-usb-dongle-plus-e)
    - [SONOFF Dongle Lite MG21 Zigbee/Thread USB Dongle | Dongle-LMG21](#sonoff-dongle-lite-mg21-zigbeethread-usb-dongle--dongle-lmg21)
    - [SMLIGHT SLZB-07](#smlight-slzb-07)
  - [Firmware Flashing](#firmware-flashing)
    - [SONOFF Zigbee 3.0 USB DONGLE Plus-E flashing](#sonoff-zigbee-30-usb-dongle-plus-e-flashing)
    - [SONOFF Dongle Lite MG21 flashing](#sonoff-dongle-lite-mg21-flashing)
    - [SMLIGHT SLZB-07 flashing](#smlight-slzb-07-flashing)
  - [Wireshark Extcap installation](#wireshark-extcap-installation)
  - [How to record packets](#how-to-record-packets)
    - [Windows](#windows)
    - [Linux](#linux)
  - [Wireshark color scheme for ZigBee](#wireshark-color-scheme-for-zigbee)
  - [How it works](#how-it-works)
  - [How to compile](#how-to-compile)
  - [What's next](#whats-next)


This repository contains firmware to allow capturing 802.15.4 packets with supported USB dongles.

Once reflashed, the USB dongle captures 802.15.4 traffic (Zigbee, 6LoWPAN/Thread) and sends frames in JSON format over USB serial to the host computer.

Wireshark can then be used to display the captured packets.

## What's needed

A supported Zigbee dongle and a computer running Windows 10, Windows 11, or Linux with Wireshark.

## Supported dongles

Three USB dongles are supported:

 ### [SONOFF Zigbee 3.0 USB DONGLE Plus-E](https://sonoff.tech/product/gateway-and-sensors/sonoff-zigbee-3-0-usb-dongle-plus-e/)

 ![SONOFF USB DONGLE Plus-E](/docs/Sonoff%20USB%20dongle%20Plus-E.PNG)

 ### [SONOFF Dongle Lite MG21 Zigbee/Thread USB Dongle | Dongle-LMG21](https://sonoff.tech/products/sonoff-dongle-lite-mg21-zigbee-thread-usb-dongle-dongle-lmg21/)

 ![LMG21.PNG](/docs/LMG21.PNG)

### [SMLIGHT SLZB-07](https://smlight.tech/product/slzb-07)

![Smlight slzb-07.PNG](/docs/Smlight%20slzb-07_3.PNG)


## Firmware Flashing

To capture 802.15.4 packets, the USB dongle must be flashed with the sniffer firmware.
***WARNING! Reflashing the device will change its firmware. The USB dongle will no longer provide factory coordinator firmware.***

### SONOFF Zigbee 3.0 USB DONGLE Plus-E flashing
The SONOFF Zigbee 3.0 USB DONGLE Plus-E comes with a pre-installed bootloader and application running a Zigbee coordinator.
To replace the pre-installed application follow the procedure "Firmware Flashing" provided by SONOFF found at https://sonoff.tech/wp-content/uploads/2023/02/SONOFF-Zigbee-3.0-USB-dongle-plus-firmware-flashing.pdf.
The file `Output/Sniffer_802.15.4_SONOFF_USB_Dongle_Plus_E/Sniffer_802.15.4_SONOFF_USB_Dongle_Plus_E.gbl` must be used when performing the XModem transfer.

The following online tool also works well:
https://darkxst.github.io/silabs-firmware-builder/

### SONOFF Dongle Lite MG21 flashing
The SONOFF Dongle Lite MG21 can be flashed using the same XModem transfer method as the Plus-E.

As an alternative, the LMG21 can be flashed using the Sonoff online tool:
https://dongle.sonoff.tech/sonoff-dongle-flasher/

Use the file `Output/Sniffer_802.15.4_SONOFF_LMG21/Sniffer_802.15.4_SONOFF_LMG21.gbl` for the LMG21 dongle.

Please note that once the SONOFF LMG21 is flashed with the sniffer firmware, it cannot be reflashed using the above tools unless you manually enter the bootloader.
To manually enter the bootloader, send the JSON payload `{"B"}` to the device using a terminal at 1 Mbit/s.

As a third alternative, the dongle can be reprogrammed using the universal-silabs-flasher:
https://github.com/NabuCasa/universal-silabs-flasher

Here is the command that worked for me with the universal-silabs-flasher on a Linux computer:

```bash
./universal-silabs-flasher --device /dev/ttyUSB1 --probe-method bootloader --bootloader-reset rts_dtr flash --firmware /path_to_file/Sniffer_802.15.4_SONOFF_LMG21.gbl
```

### SMLIGHT SLZB-07 flashing

The SMLIGHT SLZB-07 can be flashed with:
https://darkxst.github.io/silabs-firmware-builder/

Use the file `Output/Sniffer_802.15.4_SMLIGHT_SLZB_07/Sniffer_802.15.4_SMLIGHT_SLZB_07.gbl` for the SLZB-07 dongle.

Please note that once the SMLIGHT SLZB-07 is flashed with the sniffer firmware, it cannot be reflashed using the above tools unless you manually enter the bootloader.
To manually enter the bootloader, send the JSON payload `{"B"}` to the device using a terminal at 1 Mbit/s. If successful, the red LED will turn off.

Alternatively, install a jumper between the FLSH and GND pads before plugging the dongle into a USB port. The jumper must be removed once the device is reflashed.

<!-- Removed because did not work... obtained a "No matching gpiochip device found" Alternatively, the Universal Silicon Labs Flasher can also be used to reflash a SLZB-07, universal Silicon Labs Flasher can be found at https://github.com/NabuCasa/universal-silabs-flasher:
Typical command to reflash:
```
./universal-silabs-flasher --device /dev/tty8 --bootloader-reset slzb07 flash --firmware /your_path_to/Sniffer_802.15.4_SMLIGHT_SLZB_07.gbl
``` -->

## Wireshark Extcap installation

Wireshark needs a converter to understand and display correctly the packets captured by the USB dongle.
A Wireshark converter is called an Extcap (short for EXTernal CAPture).

To provide Wireshark with the needed Extcap, locate the Wireshark extcap folder.
Start Wireshark, click Help -> About Wireshark, select the "Folders" tab, and locate the Global Extcap path or Personal Extcap path.

Copy the file Extcap_802.15.4.exe to extcap folder for a computer running Windows
Copy the file zigbee_dongle_json_sniffer to extcap folder for a computer running Linux/Python

On Linux additional setup may be required:

```
# Ensure the script has execution permissions set:
chmod +x zigbee_dongle_json_sniffer
# Add yourself to the Wireshark group (optional if you want to run Wireshark as non-root user):
sudo usermod $USER -a -G wireshark
# Make sure you have read permissions for the dongle. Usually this can be done by adding yourself to the dialout group:
sudo usermod $USER -a -G dialout
# Logout and back in for user group changes to take effect.
```

Close Wireshark once the copy is done, the Extcap will be loaded the next time Wireshark is started.

If you are interested in Wireshark Extcap, you can refer to Wireshark doc [8.2. Adding Capture Interfaces And Log Sources Using Extcap](https://www.wireshark.org/docs/wsdg_html_chunked/ChCaptureExtcap.html).

## How to record packets

Once a USB dongle is flashed with sniffer firmware and Wireshark Extcap is copied in one of Wireshark extcap folders, packet capture can begin.
Plug the USB dongle into one of the available USB ports. Drivers should be automatically detected.
Start Wireshark.
### Windows
On a Windows computer, a list of available COM ports should be displayed in the bottom window<br>
![wireshark start screen](/docs/wireshark_start_screen.PNG)

Click on the gear next to the COM port corresponding to USB DONGLE COM port being used.

A configuration window will appear to select the channel to use to capture packets.
Press start once the desired channel is selected
![wireshark channel selection](/docs/wireshark_channel_selection.PNG)

Capture should start!

### Linux
On a Linux computer, the "Zigbee JSON Sniffer: zb_dongle_json_sniffer" will be displayed as one of the available interface to capture from<br>
![wireshark start screen Linux](/docs/wireshark_start_screen_linux.png)<br>
Click on the gear next to the "Zigbee JSON Sniffer: zb_dongle_json_sniffer"<br>

A configuration window will appear to select the serial capture device, channel and Baud rate.<br>
The baud rate must remain at 1000000.<br>
Press save once the desired channel is selected<br>
![wireshark channel selection Linux](/docs/wireshark_selection_linux.png)<br>
Capture can be started by double clicking on "Zigbee JSON Sniffer: zb_dongle_json_sniffer" text<br>

Capture should start!

## Wireshark color scheme for ZigBee

By default Wireshark presents ZigBee packets in black text on white background.
The default color scheme can make it difficult to quickly identify packet when analyzing ZigBee packets.
This repo provides a color scheme for Wireshark (file Wireshark\zigbee_color_scheme) well suited for ZigBee packet analysis.
ZCL and APS packets are in green
ZDP/ZDO packets are colored red
Network layer packets are presented in blue

The color scheme can be imported in Wireshark by selecting View->Coloring Rules...
![wireshark coloring rules](/docs/coloring_rules.PNG)

Click on Import...
and select the file "zigbee_color_scheme"
![wireshark coloring rules file import](/docs/coloring_rules_file_select.PNG)

Resulting coloring rules
![wireshark coloring rules result](/docs/coloring_rules_result.PNG)

Capture example
![wireshark capture example](/docs/example_capture.PNG)


## How it works

The USB dongle records 802.15.4 packets, converts to a JSON format and transfers via COM port at 1Mbit/s.
The extcap processes incoming JSON payloads and converts to Wireshark pcapng TAP (LINKTYPE_IEEE802_15_4_TAP, 283, DLT_IEEE802_15_4_TAP).

The JSON format is:

L = length
Q = LQI
R = RSSI
C = channel
S = string of hexadecimal representation of 802.15.4 packet

Example:

```json
{"L":50,"Q":255,"R":-94,"S":"4188a31e48ffff00000912fcff000001cc0885dafeffd76b0828f6ea32000885dafeffd76b0800295e19cad6ebd84ca2aee2"}
```

The USB dongle accepts channel selection via a JSON payload.
C = channel

Example:
```json
{"C":11}
```

When sent to the USB dongle, it will select channel 11. This command can be used at any time.


## How to compile

The project builds using a docker image.
To build the project you will need Docker installed and running on your computer.

Using VS Code, simply open this project in VS Code and "Reopen in Container".
Compile by issuing in a terminal:

For SONOFF Dongle Plus E:
```bash
make rebuild -f ./Sources/Target/Sonoff_USB_Dongle_Plus_E/makefile
```
To compile SMLIGHT-07 use:
```bash
make rebuild -f ./Sources/Target/smlight_slzb_07/makefile
```
Compile SONOFF LMG21 by using:
```bash
make rebuild -f ./Sources/Target/Sonoff_LMG21/makefile
```

Alternatively:
To start an interactive shell using the docker image from Docker Hub use the following command:

```bash
docker run -it --rm -v ".:/home/app" docker.io/erksponge/gcc_arm_commander_jflash:latest bash
```

Then compile using the `make` invocation mentioned above.

Alternative 3:
The file `Dockerfile` can be used to create an image that contains gcc-arm-none-eabi, SiLabs commander and Segger J-Link software.
To locally build the image, you must first download `SimplicityCommander-Linux.zip` from https://www.silabs.com/documents/login/software/SimplicityCommander-Linux.zip and store it in the same folder as the `Dockerfile`.

The docker image can be constructed by issuing:

```bash
docker image build -t erksponge/gcc_arm_commander_jflash .
```

and to compile the Sonoff Dongle Plus E sniffer:

```bash
docker run --rm -v ".:/home/app" --name build_container erksponge/gcc_arm_commander_jflash:latest make rebuild -f ./Sources/Target/Sonoff_USB_Dongle_Plus_E/makefile -j8
```

## What's next

This is a spare-time project, and feedback is welcome. If you find the project useful, a star is appreciated.

Current priorities include:

- Add support for additional dongles such as SMLIGHT SLZB-07 MG24
- Migrate the build system from Makefiles to CMake for better cross-platform support
- Improve the Docker/container environment to support CMake
- Add GitHub CI/CD for automated builds and releases

