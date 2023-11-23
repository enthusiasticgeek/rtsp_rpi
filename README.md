# rtsp_rpi
Simple no-frills RTSP server and client example (H.264 encoding/decoding) for Raspberry Pi (KISS - Keep It Simple Stupid).

### Dependencies ###

sudo apt-get install libgstreamer1.0-dev libgstrtspserver-1.0-dev gstreamer1.0-libav gstreamer1.0-libav gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly libx264-dev -y && sudo apt install gstreamer1.0-*

### Build ###

make

### RTSP Server ###

**Run RTSP server**

./rtsp_server

**Run RTSP client**

./rtsp_client

**Alternatively, one may run bash scripts (Live Video, Record Video or do both simultaneously - script names are self explanatory)**

### Notes ###

**Note:** Tested with both the cameras - Raspberry Pi HQ camera (IMX477) as well as Raspberry Pi Camera (IMX219). It uses V4L2SRC driver -> Hence should work with any camera that it supports. Ensure that the raspi-config camera settings (MIPI-CSI2 cameras) is turned on.

**Note:** Treat this as a POC (non-production code) and improve further. I have tested it on Raspberry Pi 3b and with Raspberry Pi HQ camera. Resolution is deliberately restricted at 640x480 for faster processing on the edge especially with older Raspberry Pi models. OMXENC is disabled by default (can be enabled in RTSP server C code with a MACRO).

**Note:** Currently no audio (but easy to add) and no SSL (easy to add) - see https://github.com/enthusiasticgeek/gstreamer-rtsp-ssl-example.
