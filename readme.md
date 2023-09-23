

# 802.15.4 Sniffer for SONOFF Zigbee 3.0 USB DONGLE Plus-E

- [802.15.4 Sniffer for SONOFF Zigbee 3.0 USB DONGLE Plus-E](#802154-sniffer-for-sonoff-zigbee-30-usb-dongle-plus-e)
  - [What's needed](#whats-needed)
  - [Firmware Flashing](#firmware-flashing)
  - [Wireshark Extcap installation](#wireshark-extcap-installation)
  - [How to record packets](#how-to-record-packets)
  - [Wireshark color scheme for ZigBee](#wireshark-color-scheme-for-zigbee)
  - [How it works](#how-it-works)
  - [How to compile](#how-to-compile)
  - [What's next](#whats-next)


This reposiroty contains firmware to allow capturing 802.15.4 packets with a USB SONOFF Zigbee 3.0 DONGLE Plus-E.

The USB dongle once reflashed will capture 802.15.4 traffic (Zigbee, 6lowpan/Thread) and send a frame in JSON format on USB Serial COM port to the host computer.

Wireshark can be used to display captured packets.

## What's needed

A [SONOFF Zigbee 3.0 USB DONGLE Plus-E](https://sonoff.tech/product/gateway-and-sensors/sonoff-zigbee-3-0-usb-dongle-plus-e/) is required.

![SONOFF USB DONGLE Plus-E](/docs/Sonoff%20USB%20dongle%20Plus-E.PNG)

And a computer running Windows 10 or Windows 11 with Wireshark.

## Firmware Flashing

To capture 802.15.4 packets, the Sonoff USB dongle must be flashed with the sniffer firmware.
WARNING! Reflashing the device will change its firmware. The USB dongle will no longer provide factory coordinator firmware.

The SONOFF Zigbee 3.0 USB DONGLE Plus-E comes with a pre-installed bootloader and application running a Zigbee coordinator.
To replace the pre-installed application follow the procedure "Firmware Flashing" provided by SONOFF found at https://sonoff.tech/wp-content/uploads/2023/02/SONOFF-Zigbee-3.0-USB-dongle-plus-firmware-flashing.pdf.
The file Sniffer_802.15.4_SONOFF_USB_Dongle_Plus_E.gbl must be used when performing the XModem transfer (located in folder Output\Sniffer_802.15.4_SONOFF_USB_Dongle_Plus_E)

## Wireshark Extcap installation

Wireshark needs a converter to understand and display correctly the packets captured by the USB dongle.
A wireshark converter is called an Extcap (short for EXTernal CAPture).

To provide Wireshark with the needed Extcap, copy the file found in this repo under folder /Wireshrak/Extcap_802.15.4.exe to wireshark extcap folder.
To locate the wireshark extcap folder, start wireshark, click on Help->About wireshark, select TAB named "folders", locate the
Global Extcap path
or
Personal Extcap path

The file Extcap_802.15.4.exe needs to be copied in only one of the two folders.
Close wireshark once the copy is done, the Extcap will be loaded the next time wireshark is started.

If you are interested in wireshark Extcap, you can refer to wireshark doc [8.2. Adding Capture Interfaces And Log Sources Using Extcap](https://www.wireshark.org/docs/wsdg_html_chunked/ChCaptureExtcap.html).
The Wireshark Extcap plugin provided in this repo is compatible with computer running Windows 10 and up.


## How to record packets

Once a USB dongle is flashed with sniffer firmware and wireshark Extcap is copied in one of wireshark extcap folders, packet capture can begin.
Plug the USB dongle in one of the available USB port. Drivers should be automatically detected on Windows 10 or Windows 11.
Start wireshark.
A list of available COM port should be displayed in the bottom window
![wireshark start screen](/docs/wireshark_start_screen.PNG)

Click on the gear next to the COM port corresponding to USB DONGLE COM port being used.

A configuration window will appear to select the channel to use to capture packets.
Press start once the desired channel is selected
![wireshark channel selection](/docs/wireshark_channel_selection.PNG)

Capture should start!

## Wireshark color scheme for ZigBee

By default Wireshark presents ZigBee packets in black text on white background.
The default color schem can make it difficult to quickly identify packet when analyzing ZigBee packets.
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

The USB dongle records 802.15.4 packets, convert to a JSON format and transfer via COM port at 1Mbit/s.
The extcap process inconing JSON payload and convert to wireshark pcapng TAP (LINKTYPE_IEEE802_15_4_TAP, 283, DLT_IEEE802_15_4_TAP).

The JSON format is:
L = length
Q = LQI
R = RSSI
C = channel
S = string of hexadecimal representation of 802.15.4 packet

Example:
{"L":50,"Q":255,"R":-94,"S":"4188a31e48ffff00000912fcff000001cc0885dafeffd76b0828f6ea32000885dafeffd76b0800295e19cad6ebd84ca2aee2"}


The USB dongle accepts channel selection via a JSON payload.
C = channel

Example:
{"C":11}
when sent to the usb dongle Will select channel 11, can be used at anytime

## How to compile

The project currently build by using gcc arm (gcc-arm-none-eabi) under Windows.
The project also package the output firmware in a .gbl file using Silicon Labs commander.exe command line utility.
The Arm compiler suite and Silicon Labs commander.exe can be downloaded from their companies websites.
A makefile is provided and is intended for gnu make.
Assuming a path to make.exe is present in environment variable, the following command can be used to compile

make all -f .\Sources\Target\Sonoff_USB_Dongle_Plus_E\makefile


## What's next

This is a hobby project, so I do it for learning and fun.
Here are some wish list items that I have in mind
- Convert extcap to Python so it will be portable to Linux and Mac.
- Port to other hardware from other vendor (ATSAMR21 and TI2652P)
- Improve BSP when porting evolves
- Learn VSCode dev container (or docker) to create a portable build environment
- Provide a .gbl having the factory image for user desiring to return to the factory firmware
- keep having fun...

