FROM kuroseaist/openrtm-ubuntu as rtc

RUN apt-get update\
 && DEBIAN_FRONTEND=noninteractive\
 apt-get install -y --no-install-recommends\
 g++\
 ninja-build\
 cmake\
 libomniorb4-dev\
 libopencv-dev\
 pkg-config\
 && apt-get clean\
 && rm -rf /var/lib/apt/lists/*
RUN find /opt/openrtm/ && cp -r /opt/openrtm/bin/* /usr/bin/
COPY / /RTC
RUN cmake -G Ninja -S /RTC -B/RTC/build\
 -DCMAKE_BUILD_TYPE=Release\
 -DCMAKE_INSTALL_PREFIX=/opt/rtc
RUN cmake --build /RTC/build --target install/strip

RUN mkdir -p /opt/rtc_lib
RUN cp -p $(LD_LIBRARY_PATH=/opt/openrtm/lib\
 ldd $(find /opt/rtc -type f -name ImageCalibrationComp)\
 | grep -e 'libRTC' -e 'libomni'\
 | sed -e 's/.*=> //'\
 | sed -e 's/(0x[0-9a-f]*)$//')\
 /opt/rtc_lib
RUN ldd $(find /opt/rtc -type f -name ImageCalibrationComp)
# --------------------------------------------------------------------------------
FROM ubuntu:18.04

RUN apt-get update\
 && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends\
 libopencv-videoio3.2\
 && apt-get clean\
 && rm -rf /var/lib/apt/lists/*

COPY --from=rtc /opt/rtc_lib /usr/lib
COPY --from=rtc /opt/rtc/share/openrtm-*/components/c++/opencv-rtcs/ImageCalibrationComp /work/

WORKDIR /work
