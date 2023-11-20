#!/bin/bash
IP=192.168.1.5
/usr/bin/gst-launch-1.0 -v rtspsrc location="rtsp://${IP}:8554/test" latency=0 ! decodebin ! videoconvert ! autovideosink
