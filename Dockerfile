FROM eoepca/proc-comm-zoo

RUN mkdir -p /opt/t2libs/ /zooservices/

COPY assets/zoo/main.cfg /etc/zoo-project/main.cfg

COPY build/3ty/proc-comm-lib-ows/proc-comm-lib-ows-build/libeoepcaows.so  /opt/t2libs/libeoepcaows.so

COPY zoo_build_services/libepcatransactional.so /opt/t2service/libepcatransactional.so
COPY src/zoo/eoepcaadesdeployprocess.zcfg /opt/t2service/eoepcaadesdeployprocess.zcfg
COPY src/zoo/eoepcaadesundeployprocess.zcfg /opt/t2service/eoepcaadesundeployprocess.zcfg

COPY scripts/entrypoint.sh /opt/t2scripts/entrypoint.sh

RUN chown 48:48 /zooservices && chmod +x /opt/t2scripts/entrypoint.sh

CMD ["/opt/t2scripts/entrypoint.sh"]







