#!/bin/bash
HOST=0.0.0.0
PORT=7001
/usr/bin/gst-launch-1.0 tcpclientsrc host=${HOST} port=${PORT}  \
    ! matroskademux \
    ! jpegdec \
    ! autovideosink sync=false
