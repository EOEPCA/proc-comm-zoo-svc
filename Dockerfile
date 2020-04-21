FROM eoepca/proc-comm-zoo

RUN mkdir -p /opt/t2libs/
COPY assets/zoo/main.cfg /etc/zoo-project/main.cfg
COPY build/3ty/proc-comm-lib-ows/proc-comm-lib-ows-build/libeoepcaows.so  /opt/t2libs/libeoepcaows.so






