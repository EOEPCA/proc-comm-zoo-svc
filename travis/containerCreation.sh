#!/usr/bin/env bash

set -euov pipefail

source travis/libs/variables.sh

#run script
scripts/build.sh

if [ "${TRAVIS}" == "true" ]
then #send push
	
	docker tag ${LOCAL_IMAGE_NAME} ${EOEPCA_ADES_ZOO}
	if [ "$?" -ne 0 ]
	then
		echo "docker tag ${LOCAL_IMAGE_NAME} --> ${EOEPCA_ADES_ZOO} failed"
		exit 1
	fi

	echo "${DOCKER_PASSWORD}" | docker login -u "${DOCKER_USERNAME}" --password-stdin
	if [ "$?" -ne 0 ]
	then
		echo "Docker Hub login failed"
		exit 1
	fi

	docker push "${EOEPCA_ADES_ZOO}"
	if [ "$?" -ne 0 ]
	then
		echo "Docker push ${EOEPCA_ADES_ZOO} failed"
		exit 1
	fi
fi

