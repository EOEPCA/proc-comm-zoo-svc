#!/usr/bin/env bash

if [[ ! -d "proc-comm-lib-ows-1.0" ]]
then
  curl -L  "https://github.com/EOEPCA/proc-comm-lib-ows/archive/v1.0.tar.gz" -o proc-comm-lib-ows-v1.0.tar.gz
  tar -zxvf proc-comm-lib-ows-v1.0.tar.gz
  rm -f proc-comm-lib-ows-v1.0.tar.gz
fi



