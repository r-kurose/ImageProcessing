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

COPY / /RTC
RUN cmake -G Ninja -S /RTC -B/RTC/build\
 -DCMAKE_BUILD_TYPE=Release\
 -DCMAKE_INSTALL_PREFIX=/opt/rtc
RUN cmake --build /RTC/build --target install/strip

RUN mkdir -p /opt/rtc_lib
RUN cp -p $(LD_LIBRARY_PATH=/opt/openrtm/lib\
 ldd $(find /opt/rtc -type f -name FlipComp)\
 | grep -e 'libRTC' -e 'libomni'\
 | sed -e 's/.*=> //'\
 | sed -e 's/(0x[0-9a-f]*)$//')\
 /opt/rtc_lib

# --------------------------------------------------------------------------------
FROM ubuntu:18.04

ARG username=noroot

RUN apt-get update\
 && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends\
 libopencv-core3.2\
 && apt-get clean\
 && rm -rf /var/lib/apt/lists/*\
 && mkdir -m 777 /work\
 && adduser --no-create-home --disabled-login ${username}

COPY --from=rtc /opt/rtc_lib /usr/lib
COPY --from=rtc /opt/rtc/share/openrtm-*/components/c++/opencv-rtcs/FlipComp /work/

USER ${username}
WORKDIR /work
