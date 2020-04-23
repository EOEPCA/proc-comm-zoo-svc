#!/usr/bin/env bash

set -euov pipefail

source travis/libs/variables.sh

if [ "${TRAVIS}" == "true" ]
then #send push

	docker pull ${EOEPCA_ADES_ZOO}
	if [ $? -ne 0 ]
	then
		echo "Can't pull the Docker image ${EOEPCA_ADES_ZOO}"
		exit 1
	fi


	echo "${DOCKER_PASSWORD}" | docker login -u "${DOCKER_USERNAME}" --password-stdin
	if [ "$?" -ne 0 ]
	then
		echo "Docker Hub login failed"
		exit 1
	fi

	DOCKER_RELEASE="${EOEPCA_REPOSITORY}/${EOEPCA_IMAGE}:latest"

	docker tag ${EOEPCA_ADES_ZOO} ${DOCKER_RELEASE}
	if [ "$?" -ne 0 ]
	then
		echo "Docker tag ${EOEPCA_ADES_ZOO} --> ${DOCKER_RELEASE} failed"
		exit 1
	fi


	docker push ${DOCKER_RELEASE}
	if [ "$?" -ne 0 ]
	then
		echo "Docker push ${DOCKER_RELEASE} failed"
		exit 1
	fi

fi

