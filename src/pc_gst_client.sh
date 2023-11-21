#!/bin/bash
/usr/bin/gst-launch-1.0 -v rtspsrc location="rtsp://192.168.1.7:8554/test" latency=0 ! decodebin ! videoconvert ! autovideosink
