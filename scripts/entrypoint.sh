#!/usr/bin/env bash

cp /opt/t2service/*  /zooservices/

ldconfig

/usr/sbin/httpd -DFOREGROUND

