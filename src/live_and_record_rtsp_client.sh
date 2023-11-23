#!/bin/bash
# Check if an argument (IP) is provided
if [ -z "$1" ]; then
    echo "Usage: $0 <IP>"
    echo "Press CTRL+C in the command line to exit (Closing Video Window may not record the mp4 video properly)"
    exit 1
fi

# Generate date and time suffix for the file
DATE=$(date +"%Y%m%d_%H%M%S")
FILENAME="video_clip_at_${DATE}.mp4"
IP=$1

/usr/bin/gst-launch-1.0 -v -e rtspsrc location="rtsp://${IP}:8554/test" latency=0 is-live=true ! rtph264depay ! tee name=t \
t. ! queue ! decodebin ! videoconvert ! gdkpixbufoverlay location=image.png ! clockoverlay time-format="%e-%h-%G %r" shaded-background=false  font-desc="Sans 10" ! x264enc ! h264parse ! mp4mux ! filesink location="${FILENAME}" sync=false async=false \
t. ! queue ! decodebin ! videoconvert ! gdkpixbufoverlay location=image.png ! clockoverlay time-format="%e-%h-%G %r" shaded-background=false  font-desc="Sans 10" ! autovideosink sync=false async=false 

