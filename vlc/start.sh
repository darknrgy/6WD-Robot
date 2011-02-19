#!/bin/bash
vlc dshow:// :dshow-vdev="QuickCam Pro for Notebooks" \
:dshow-adev="Microphone (Pro for Notebooks)" :dshow-size="320x240" \
--sout='#transcode{acodec=mp3,ab=16,vcodec=mp1v,vb=64}'\
':standard{mux=ts,access=udp,dst=localhost:12340}'

