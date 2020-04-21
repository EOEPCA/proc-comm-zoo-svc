#CFLAGS=-L/var/lib64 -I../../zoo-project/thirds/cgic206 -I../t2-zoo/src/include -I/usr/local/include/zoo -I./ -I`xml2-config --cflags` -I`python-config --cflags`  -I`gdal-config --cflags`   -DLINUX_FREE_ISSUE  
#-l:libt2Wps.so.1 
# CFLAGS=-L/var/lib64 -I/opt/t2ZooServiceBuilder/src/include -I/usr/local/include/t2wpsZoolib -I/usr/local/include/t2wpslib/ -I/usr/local/include/zoo -I./ -I`xml2-config --cflags` -I`python-config --cflags`  -I`gdal-config --cflags`   -DLINUX_FREE_ISSUE
CFLAGS=-L/var/lib64   -I/usr/local/include/t2wpsZoolib -I/usr/local/include/zoo -I./ -I./include  `xml2-config --cflags` -DLINUX_FREE_ISSUE

all:
	g++ -g  -std=c++17 -Wall  -DSERVICENAME=${COMPILE} ${CFLAGS}  ./t2zooservice.cpp -lzoo_service -shared -fpic -lm -ldl  -o build/${COMPILE}.zo

clean:
	rm -f build/${COMPILE}.zo

install: all
	cp build/${COMPILE}.zo ${OUTPUTPATH}/${COMPILE}.zo