# rtsp_rpi
Simple no-frills RTSP server and client example (H.264 encoding/decoding) for Raspberry Pi (KISS - Keep It Simple Stupid).

**Note:** Tested with both the cameras - Raspberry Pi HQ camera (IMX477) as well as Raspberry Pi Camera (IMX219). It uses V4L2SRC driver -> Hence should work with any camera that it supports. Ensure that the raspi-config camera settings (MIPI-CSI2 cameras) is turned on.

**Note:** Treat this as a POC (non-production code) and improve further. I have tested it on Raspberry Pi 3b and with Raspberry Pi HQ camera

**Note:** Currently no audio (but easy to add) and no SSL (easy to add) - see https://github.com/enthusiasticgeek/gstreamer-rtsp-ssl-example.
