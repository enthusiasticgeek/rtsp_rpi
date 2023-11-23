#!/bin/bash
# Check if an argument (IP) is provided
if [ -z "$1" ]; then
    echo "Usage: $0 <IP>"
    exit 1
fi

IP=$1

/usr/bin/gst-launch-1.0 -v  rtspsrc location="rtsp://${IP}:8554/test" latency=0 is-live=true ! rtph264depay ! queue ! decodebin ! videoconvert ! gdkpixbufoverlay location=image.png ! clockoverlay time-format="%e-%h-%G %r" shaded-background=false  font-desc="Sans 10" ! autovideosink sync=false async=false 
