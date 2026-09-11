# syntax=docker/dockerfile:1
# Dated Debian base aligned with the audited Bookworm build environment.
FROM debian:bookworm-20230612-slim

ARG DEBIAN_FRONTEND=noninteractive
ARG DEBIAN_SNAPSHOT=20230612T000000Z

RUN printf '%s\n'       "deb [check-valid-until=no] https://snapshot.debian.org/archive/debian/${DEBIAN_SNAPSHOT}/ bookworm main"       > /etc/apt/sources.list     && rm -f /etc/apt/sources.list.d/*     && apt-get -o Acquire::Check-Valid-Until=false update     && apt-get install --yes --no-install-recommends       binutils=2.40-2       dosfstools=4.2-1       faketime=0.9.10-2.1       gcc=4:12.2.0-3       make=4.3-4.1       mtools=4.0.33-1+really4.0.32-1       nasm=2.16.01-1     && rm -rf /var/lib/apt/lists/*

ENV LANG=C.UTF-8     LC_ALL=C.UTF-8     TZ=UTC     SOURCE_DATE_EPOCH=1686528000

WORKDIR /work
COPY scripts/container-build.sh /usr/local/bin/arios-container-build
RUN chmod 0755 /usr/local/bin/arios-container-build

ENTRYPOINT ["/usr/local/bin/arios-container-build"]
