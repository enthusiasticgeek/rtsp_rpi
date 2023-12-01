#!/bin/bash
HOST=0.0.0.0
PORT=7001
DEVICE=/dev/video0
/usr/bin/gst-launch-1.0 v4l2src device=${DEVICE} \
    ! jpegenc \
    ! matroskamux \
    ! tcpserversink host=${HOST} port=${PORT}

