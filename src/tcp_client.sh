#!/bin/bash
HOST=0.0.0.0
PORT=7001
/usr/bin/gst-launch-1.0 tcpclientsrc host=${HOST} port=${PORT}  \
    ! matroskademux \
    ! jpegdec ! videoconvert ! gdkpixbufoverlay location=image.png ! clockoverlay time-format="%e-%h-%G %r" shaded-background=false  font-desc="Sans 10"  \
    ! autovideosink sync=false
