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
	
	
	docker run -d --rm --name zoo -p 7777:80  ${EOEPCA_ADES_ZOO}
	if [ $? -ne 0 ]
	then
		echo "Can't Run the Docker image ${EOEPCA_ADES_ZOO}"
		exit 1
	fi	

	sleep 15

	echo "Send GetCababilities"
	curl -s -L  "http://localhost:7777/zoo/?service=WPS&version=1.0.0&request=GetCapabilities"

	sleep 2
	echo "Send Deploy service"
	curl -s -L "http://localhost:7777/zoo/?service=wps&version=1.0.0&request=Execute&identifier=eoepcaadesdeployprocess&dataInputs=applicationPackage=https%3A%2F%2Fcatalog.terradue.com%2Feoepca-apps%2Fsearch%3Fformat%3Datom%26uid%3Dapplication_package_sample_app;&ResponseDocument=deployResult@mimeType=application/xml"

	sleep 2
	echo "Send GetCababilities"
	curl -s -L  "http://localhost:7777/zoo/?service=WPS&version=1.0.0&request=GetCapabilities"

	sleep 2
	echo "Send Undeploy service"
	curl -s -L "http://localhost:7777/zoo/?service=wps&version=1.0.0&request=Execute&identifier=eoepcaadesundeployprocess&dataInputs=applicationPackage=https%3A%2F%2Fcatalog.terradue.com%2Feoepca-apps%2Fsearch%3Fformat%3Datom%26uid%3Dapplication_package_sample_app;&ResponseDocument=unDeployResult@mimeType=application/xml"

	sleep 2
	echo "Send GetCababilities"
	curl -s -L  "http://localhost:7777/zoo/?service=WPS&version=1.0.0&request=GetCapabilities"

fi

