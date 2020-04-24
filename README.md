[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]
[![MIT License][license-shield]][license-url]

<!-- PROJECT LOGO -->
<br />
<p align="center">
  <a href="https://github.com/EOEPCA/proc-comm-zoo-svc">
    <img src="images/logo.png" alt="Logo" width="80" height="80">
  </a>

  <h3 align="center">proc-comm-zoo-svc</h3>

  <p align="center">
    OWS base parser
    <br />
    <a href="https://github.com/EOEPCA/proc-comm-zoo-svc"><strong>Explore the docs »</strong></a>
    <br />
    <a href="https://github.com/EOEPCA/proc-comm-zoo-svc">View Demo</a>
    ·
    <a href="https://github.com/EOEPCA/proc-comm-zoo-svc/issues">Report Bug</a>
    ·
    <a href="https://github.com/EOEPCA/proc-comm-zoo-svc/issues">Request Feature</a>
    
  </p>
</p>

<!-- TABLE OF CONTENTS -->

## Table of Contents

- [About the Project](#about-the-project)
  - [Built With](#built-with)
  - [Travis Build](#travis-build)
  - [Manual Build](#manual-build)
    - [Prerequisites](#prerequisites)
- [Documentation](#documentation)
- [Usage](#usage)
- [Roadmap](#roadmap)
- [Contributing](#contributing)
- [License](#license)
- [Contact](#contact)
- [Acknowledgements](#acknowledgements)

<!-- ABOUT THE PROJECT -->

## About The Project

The aim of the project is to provide a transactional interface to a [OGC](https://en.wikipedia.org/wiki/Open_Geospatial_Consortium)-[wps server(WPS)](https://en.wikipedia.org/wiki/Web_Processing_Service).

To achieve our goal, we are going to use as a basis a project presented by the EOEPCA team:
[proc-comm-zoo](https://github.com/EOEPCA/proc-comm-zoo)

We will add the deploy and undeploy functionality of any custom service to the *proc-comm-zoo* project.

Before talking about the concept of transactional WPS, we need to understand one basic element of a WPS server:

A WPS server provides a set of processes that are described by an XML interface.
Each process is identified by its own ID and it is unique.

```xml
<ows:Identifier>GetStatus</ows:Identifier>
```

We can have the list of all the processes of a WPS service using a getCapabilities call:

```shell
curl -s -L  "http://server_wps_uri/wps/?service=WPS&version=1.0.0&request=GetCapabilities"
```

```xml
<?xml version="1.0" encoding="UTF-8"?>
<wps:Capabilities xmlns:ows="http://www.opengis.net/ows/1.1" xmlns:wps="http://www.opengis.net/wps/1.0.0" xmlns:xlink="http://www.w3.org/1999/xlink" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://www.opengis.net/wps/1.0.0 http://schemas.opengis.net/wps/1.0.0/wpsGetCapabilities_response.xsd" service="WPS" version="1.0.0" xml:lang="en-US">
  <ows:ServiceIdentification>

...
<wps:ProcessOfferings>
<wps:Process wps:processVersion="1.0.0">
  <ows:Identifier>eoepcaadesdeployprocess</ows:Identifier>
  <ows:Title>Eoepca Deploy Process</ows:Title>
  <ows:Abstract>This method will deploy an application encapsulated within a Docker container as a process accessible through the WPS interface.</ows:Abstract>
</wps:Process>
<wps:Process wps:processVersion="1">
  <ows:Identifier>GetStatus</ows:Identifier>
  <ows:Title>Produce an updated ExecuteResponse document. </ows:Title>
  <ows:Abstract>Create an ExecuteResponse document from a sid (Service ID), it will use the niternal ZOO Kernel mechanisms to access the current status from a running Service and update the percentCompleted from the original backup file used by the ZOO Kernel when running a Service in background. </ows:Abstract>
  <ows:Metadata xlink:title="Demo GetStatus request"/>
</wps:Process>
<wps:Process wps:processVersion="1.0.0">
  <ows:Identifier>eoepcaadesundeployprocess</ows:Identifier>
  <ows:Title>Eoepca Deploy Process</ows:Title>
  <ows:Abstract>This method will undeploy an application encapsulated within a Docker container as a process accessible through the WPS interface.</ows:Abstract>
</wps:Process>
</wps:ProcessOfferings>
...
</wps:Capabilities>

```

The key elements that define our processes are `GetStatus`,`eoepcaadesdeployprocess` and `eoepcaadesundeployprocess`.

We can get more information about a process using a DescribeProcess call:

```sh
curl -s -L "http://server_wps_uri/wps/?service=WPS&version=1.0.0&request=DescribeProcess&identifier=eoepcaadesdeployprocess"
```

`identifier` is the unique ID that describes the process


```xml
<?xml version="1.0" encoding="UTF-8"?>
<wps:ProcessDescriptions xmlns:ows="http://www.opengis.net/ows/1.1" xmlns:wps="http://www.opengis.net/wps/1.0.0" xmlns:xlink="http://www.w3.org/1999/xlink" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://www.opengis.net/wps/1.0.0 http://schemas.opengis.net/wps/1.0.0/wpsDescribeProcess_response.xsd" service="WPS" version="1.0.0" xml:lang="en-US">
  <ProcessDescription wps:processVersion="1.0.0" storeSupported="true" statusSupported="true">
    <ows:Identifier>eoepcaadesdeployprocess</ows:Identifier>
    <ows:Title>Eoepca Deploy Process</ows:Title>
    <ows:Abstract>This method will deploy an application encapsulated within a Docker container as a process accessible through the WPS interface.</ows:Abstract>
    <DataInputs>
      <Input minOccurs="1" maxOccurs="1">
        <ows:Identifier>applicationPackage</ows:Identifier>
        <ows:Title>Application Package</ows:Title>
        <ows:Abstract>An application package, encoded as an ATOM-encoded OGC context document, describing the details of the application.</ows:Abstract>
        <ComplexData>
          <Default>
            <Format>
              <MimeType>application/atom+xml</MimeType>
            </Format>
          </Default>
          <Supported>
            <Format>
              <MimeType>application/atom+xml</MimeType>
            </Format>
          </Supported>
        </ComplexData>
      </Input>
    </DataInputs>
    <ProcessOutputs>
      <Output>
        <ows:Identifier>deployResult</ows:Identifier>
        <ows:Title>Deploy Result</ows:Title>
        <ows:Abstract>The server's response to deploying a process.  A successful response will contain a summary of the deployed process.</ows:Abstract>
        <ComplexOutput>
          <Default>
            <Format>
              <MimeType>application/xml</MimeType>
              <Encoding>ISO-8859-1</Encoding>
            </Format>
          </Default>
          <Supported>
            <Format>
              <MimeType>application/xml</MimeType>
              <Encoding>ISO-8859-1</Encoding>
            </Format>
          </Supported>
        </ComplexOutput>
      </Output>
    </ProcessOutputs>
  </ProcessDescription>
</wps:ProcessDescriptions>

```

In the response the identifier is repeated and we will also have all the information relating to the requested process:
input and output parameters

In the last example we asked for the description of the process that will allow us to install a new service (the code relating to the implementation of this service is available in this project: *proc-comm-zoo-svc/src/zoo/deploy.cpp*).

The `applicationPackage` parameter

```xml
<Input minOccurs="1" maxOccurs="1">
  <ows:Identifier>applicationPackage</ows:Identifier>
  <ows:Title>Application Package</ows:Title>
  <ows:Abstract>An application package, encoded as an ATOM-encoded OGC context document, describing the details of the application.</ows:Abstract>
```

requires the URI of an OWS file that describes the functionality of the process to be installed.

The OWS file defines the elements necessary to describe a wps service as defined by the
[OGC standards](https://www.ogc.org/standards/wps#downloads).
An example of this file can be found in the project [proc-comm-lib-ows](https://github.com/EOEPCA/proc-comm-lib-ows/blob/master/tests/application/assets/ows1.xml)

The OWS file, [shown in the example](https://github.com/EOEPCA/proc-comm-lib-ows/blob/master/tests/application/assets/ows1.xml)
, describes an interface that must be added to a WPS server defined by the unique
[sample-app](https://github.com/EOEPCA/proc-comm-lib-ows/blob/master/tests/application/assets/ows1.xml#L23) ID.

```xml
<owc:operation code="DescribeProcess" href="https://ems.eoepca.com/zoo-bin/zoo_loader.cgi?request=DescribeProcess&amp;service=WPS&amp;version=1.0.0&amp;Identifier=sample_app_0_0_1_2" method="GET" type="application/xml">
                <owc:result type="text/xml">
                    <ProcessDescription xmlns:wps="http://www.opengis.net/wps/1.0.0" xmlns:ows="http://www.opengis.net/ows/1.1" xmlns="http://www.w3.org/2005/Atom" processVersion="1.12" storeSupported="true" statusSupported="true">
                        <ows:Identifier>sample-app</ows:Identifier>
                        <ows:Title>A sample App</ows:Title>
                        <ows:Abstract>Sample abstract</ows:Abstract>
                        <DataInputs>
```

```xml
<ows:Identifier>sample-app</ows:Identifier>
```

For the description of the file and the conversion, we will use the resources made available by the EOEPCA team: [proc-comm-lib-ows](https://github.com/EOEPCA/proc-comm-lib-ows)

It is important to know how the deployment and undeploy services create a unique identifier for the WPS server.

To avoid ID collisions we cannot rely on the simple parameter of `<ows: Identifier>sample-app</ows: Identifier>` to create our unique identifier.

The OWS file provides us with many other information from which we can create a unique Id.

The first information is the OWS file URI:

```html
https://catalog.sameoust.com/eoepca-apps/search?format=atom&uid=application_package_sample_app
```

The second piece of information is retrieved from the Identifier of the Entry tag

```xml
<entry>
        <identifier xmlns="http://purl.org/dc/elements/1.1/">application_package_sample_app</identifier>
        <title type="text">Application package for a sample App</title>
        <summary type="html">Application package a sample App</summary>
```

In the `ProcessDescription` tag we can find the version of the process to be installed

```xml
<owc:result type="text/xml">
                    <ProcessDescription xmlns:wps="http://www.opengis.net/wps/1.0.0" xmlns:ows="http://www.opengis.net/ows/1.1" xmlns="http://www.w3.org/2005/Atom" processVersion="1.12" storeSupported="true" statusSupported="true">


```

and at last the process Identifier:

```xml
<ProcessDescription xmlns:wps="http://www.opengis.net/wps/1.0.0" xmlns:ows="http://www.opengis.net/ows/1.1" xmlns="http://www.w3.org/2005/Atom" processVersion="1.12" storeSupported="true" statusSupported="true">
                        <ows:Identifier>sample-app</ows:Identifier>
                        <ows:Title>A sample App</ows:Title>
                        <ows:Abstract>Sample abstract</ows:Abstract>
```

We create a string with all these elements and using the SHA1 algorithm we produce a new unique ID.

```txt
w05232691f576bae9b5715986e98d0def32065477
```

### Built With

- [Docker](https://www.docker.com/)
- [Docker Image eoepca/eoepca-build-cpp:1.0](https://hub.docker.com/r/eoepca/eoepca-build-cpp)
- [Docker Image eoepca/proc-comm-zoo:latest](https://hub.docker.com/r/eoepca/proc-comm-zoo)
- [CMAKE](https://cmake.org/)
- [googletest](https://github.com/google/googletest)

<!-- GETTING STARTED -->


### Travis Build

The project can be compiled using Travis. All necessary scripts are provided in the `travis` folder

Travis uses DockerHub resources to build the final image:

- [Docker Image eoepca/eoepca-build-cpp:1.0](https://hub.docker.com/r/eoepca/eoepca-build-cpp)

- [Docker Image eoepca/proc-comm-zoo:latest](https://hub.docker.com/r/eoepca/proc-comm-zoo)

`eoepca/eoepca-build-cpp:1.0` can be created using the file `proc-comm-zoo-svc/Dockerfile_builder`

`eoepca/proc-comm-zoo` can be created using the project [proc-comm-zoo](https://github.com/EOEPCA/proc-comm-zoo)

### Manual Build

#### Prerequisites

- [Docker](https://www.docker.com/)
- [Linux bash](https://en.wikipedia.org/wiki/Bash_(Unix_shell))

1) Clone the repository

```sh
git clone https://github.com/EOEPCA/proc-comm-zoo-svc.git
```

2) Run script

```sh
cd proc-comm-zoo-svc

./scripts/build.sh
```

The script creates the Docker Image locally

```txt
REPOSITORY                TAG                 IMAGE ID            CREATED              SIZE
eoepca-ades-wps           1.0                 154d2111b280        ..................   1.68GB
```

## Documentation

The component documentation can be found at https://github.com/EOEPCA/proc-comm-zoo-svc/

<!-- USAGE EXAMPLES -->

## Usage

The WPS server interface can be configured by following the instructions provided by the site[http://zoo-project.org/](http://zoo-project.org/docs/kernel/configuration.html).

The configuration file is /proc-comm-zoo-svc/assets/zoo/main.cfg

Important elements of the configuration are:

```ini
[main]
servicePath=/zooservices

[metadata]
adesId=1388036c-83ee-11ea-a0ab-5f92989a690f

[eoepca]
owsparser=/opt/t2libs/libeoepcaows.so
```

`servicePath` Defines where the WPS server will look for process configuration files.

If you intend to modify this parameter, the following files must also be changed

- proc-comm-zoo-svc/CMakeLists.txt
- proc-comm-zoo-svc/Dockerfile

`adesId` represents the unique ID of the WPS server instance

`owsparser` Represents the path of where the [OWS file parser is installed](https://github.com/EOEPCA/proc-comm-lib-ows/blob/master/tests/application/assets/ows1.xml).

### Running

### Prerequisites

- [Docker](https://www.docker.com/)
- [Linux bash](https://en.wikipedia.org/wiki/Bash_(Unix_shell))
- [curl](https://curl.haxx.se/docs/manpage.html)

### Run

For this run we will use the version created locally. The EOEPCA team provides a pre-built version [eoepca/eoepca-ades-wps](https://hub.docker.com/r/eoepca/eoepca-ades-wps)

1) Run the container

```sh
export EOEPCA_ZOO='eoepca-ades-wps:1.0'

docker run -d --rm --name zoo -p 7777:80  ${EOEPCA_ZOO}
```

We check the active containers

```txt
Images:
REPOSITORY                TAG                 IMAGE ID            CREATED             SIZE
eoepca-ades-wps           1.0                 154d2111b280        About an hour ago   1.68GB
eoepca/proc-comm-zoo      latest              80cda9ce446b        21 hours ago        1.68GB
eoepca/eoepca-ades-wps    travis_develop_3    ba62d8babafb        23 hours ago        1.68GB
eoepca/eoepca-build-cpp   1.0                 f066b8e6d51a        4 weeks ago         1.78GB

Container:
CONTAINER ID        IMAGE                 COMMAND                  CREATED             STATUS              PORTS                  NAMES
f391b5ebb607        eoepca-ades-wps:1.0   "container-entrypoin…"   17 seconds ago      Up 17 seconds       0.0.0.0:7777->80/tcp   zoo

Run:
CONTAINER ID        IMAGE                 COMMAND                  CREATED             STATUS              PORTS                  NAMES
f391b5ebb607        eoepca-ades-wps:1.0   "container-entrypoin…"   17 seconds ago      Up 17 seconds       0.0.0.0:7777->80/tcp   zoo
```

Star with GetCapabilities

```sh
curl -s -L  "http://localhost:7777/zoo/?service=WPS&version=1.0.0&request=GetCapabilities"
```

```xml
<?xml version="1.0" encoding="UTF-8"?>
<wps:Capabilities xmlns:ows="http://www.opengis.net/ows/1.1" xmlns:wps="http://www.opengis.net/wps/1.0.0" xmlns:xlink="http://www.w3.org/1999/xlink" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://www.opengis.net/wps/1.0.0 http://schemas.opengis.net/wps/1.0.0/wpsGetCapabilities_response.xsd" service="WPS" version="1.0.0" xml:lang="en-US">
 ...

  <ows:OperationsMetadata>
    <ows:Operation name="GetCapabilities">
      <ows:DCP>
        <ows:HTTP>
          <ows:Get xlink:href="http://localhost:7777/zoo-bin/zoo_loader.cgi"/>
          <ows:Post xlink:href="http://localhost:7777/zoo-bin/zoo_loader.cgi"/>
        </ows:HTTP>
      </ows:DCP>
    </ows:Operation>
    <ows:Operation name="DescribeProcess">
      <ows:DCP>
        <ows:HTTP>
          <ows:Get xlink:href="http://localhost:7777/zoo-bin/zoo_loader.cgi"/>
          <ows:Post xlink:href="http://localhost:7777/zoo-bin/zoo_loader.cgi"/>
        </ows:HTTP>
      </ows:DCP>
    </ows:Operation>
    <ows:Operation name="Execute">
      <ows:DCP>
        <ows:HTTP>
          <ows:Get xlink:href="http://localhost:7777/zoo-bin/zoo_loader.cgi"/>
          <ows:Post xlink:href="http://localhost:7777/zoo-bin/zoo_loader.cgi"/>
        </ows:HTTP>
      </ows:DCP>
    </ows:Operation>
  </ows:OperationsMetadata>
  <wps:ProcessOfferings>
    <wps:Process wps:processVersion="1.0.0">
      <ows:Identifier>eoepcaadesundeployprocess</ows:Identifier>
      <ows:Title>Eoepca Deploy Process</ows:Title>
      <ows:Abstract>This method will undeploy an application encapsulated within a Docker container as a process accessible through the WPS interface.</ows:Abstract>
    </wps:Process>
    <wps:Process wps:processVersion="1">
      <ows:Identifier>GetStatus</ows:Identifier>
      <ows:Title>Produce an updated ExecuteResponse document. </ows:Title>
      <ows:Abstract>Create an ExecuteResponse document from a sid (Service ID), it will use the niternal ZOO Kernel mechanisms to access the current status from a running Service and update the percentCompleted from the original backup file used by the ZOO Kernel when running a Service in background. </ows:Abstract>
      <ows:Metadata xlink:title="Demo GetStatus request"/>
    </wps:Process>
    <wps:Process wps:processVersion="1.0.0">
      <ows:Identifier>eoepcaadesdeployprocess</ows:Identifier>
      <ows:Title>Eoepca Deploy Process</ows:Title>
      <ows:Abstract>This method will deploy an application encapsulated within a Docker container as a process accessible through the WPS interface.</ows:Abstract>
    </wps:Process>
  </wps:ProcessOfferings>
  <wps:Languages>
    <wps:Default>
      <ows:Language>en-US</ows:Language>
    </wps:Default>
    <wps:Supported>
      <ows:Language>en-US</ows:Language>
      <ows:Language>en-GB</ows:Language>
    </wps:Supported>
  </wps:Languages>
</wps:Capabilities>
```

Add new service defined from https://catalog.terradue.com/eoepca-apps/search?format=atom&uid=application_package_sample_app

```sh
curl -s -L "http://localhost:7777/zoo/?service=wps&version=1.0.0&request=Execute&identifier=eoepcaadesdeployprocess&dataInputs=applicationPackage=https%3A%2F%2Fcatalog.terradue.com%2Feoepca-apps%2Fsearch%3Fformat%3Datom%26uid%3Dapplication_package_sample_app;&ResponseDocument=deployResult@mimeType=application/xml"
```

response:

```xml
<?xml version="1.0" encoding="UTF-8"?>
<wps:ExecuteResponse xmlns:ows="http://www.opengis.net/ows/1.1" xmlns:wps="http://www.opengis.net/wps/1.0.0" xmlns:xlink="http://www.w3.org/1999/xlink" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://www.opengis.net/wps/1.0.0 http://schemas.opengis.net/wps/1.0.0/wpsExecute_response.xsd" service="WPS" version="1.0.0" xml:lang="en-US" serviceInstance="http://localhost:7777/zoo-bin/zoo_loader.cgi">
  <wps:Process wps:processVersion="1.0.0">
    <ows:Identifier>eoepcaadesdeployprocess</ows:Identifier>
    <ows:Title>Eoepca Deploy Process</ows:Title>
    <ows:Abstract>This method will deploy an application encapsulated within a Docker container as a process accessible through the WPS interface.</ows:Abstract>
  </wps:Process>
  <wps:Status creationTime="2020-04-24T09:52:11Z">
    <wps:ProcessSucceeded>The service "eoepcaadesdeployprocess" ran successfully.</wps:ProcessSucceeded>
  </wps:Status>
  <wps:ProcessOutputs>
    <wps:Output>
      <ows:Identifier>deployResult</ows:Identifier>
      <ows:Title>Deploy Result</ows:Title>
      <ows:Abstract>The server's response to deploying a process.  A successful response will contain a summary of the deployed process.</ows:Abstract>
      <wps:Data>
        <wps:ComplexData mimeType="application/xml" encoding="ISO-8859-1">
          <result applicationPackageFile="https://catalog.terradue.com/eoepca-apps/search?format=atom&amp;uid=application_package_sample_app">
            <operations packageId="application_package_sample_app">
              <operation processId="sample-app" processVersion="1.12" version="1.12" type="deploy">
                <wpsId>w05232691f576bae9b5715986e98d0def32065477</wpsId>
                <title>A sample App</title>
                <abstract>Sample abstract</abstract>
                <status err="0" mess="">ready</status>
              </operation>
            </operations>
          </result>
        </wps:ComplexData>
      </wps:Data>
    </wps:Output>
  </wps:ProcessOutputs>
</wps:ExecuteResponse>
```

The return parameter `deployResult` defines the result of the operation:

```xml
<operations packageId="application_package_sample_app">
  <operation processId="sample-app" processVersion="1.12" version="1.12" type="deploy">
    <wpsId>w05232691f576bae9b5715986e98d0def32065477</wpsId>
    <title>A sample App</title>
    <abstract>Sample abstract</abstract>
    <status err="0" mess="">ready</status>
  </operation>
</operations>
</result>
```

`wpsId` wpsId represents the Id assigned to the service

The status tag returns an integer error and an error message

```xml
<status err="0" mess="">ready</status>
```

Now, we can do another GetCapabilities request to see the new process:

```sh
curl -s -L  "http://localhost:7777/zoo/?service=WPS&version=1.0.0&request=GetCapabilities"
```

```xml

...

  <wps:ProcessOfferings>
    <wps:Process wps:processVersion="1.12">
      <ows:Identifier>w05232691f576bae9b5715986e98d0def32065477</ows:Identifier>
      <ows:Title>A sample App</ows:Title>
      <ows:Abstract>Sample abstract</ows:Abstract>
    </wps:Process>
    <wps:Process wps:processVersion="1.0.0">
      <ows:Identifier>eoepcaadesundeployprocess</ows:Identifier>
      <ows:Title>Eoepca Deploy Process</ows:Title>
      <ows:Abstract>This method will undeploy an application encapsulated within a Docker container as a process accessible through the WPS interface.</ows:Abstract>
    </wps:Process>
    <wps:Process wps:processVersion="1">
      <ows:Identifier>GetStatus</ows:Identifier>
      <ows:Title>Produce an updated ExecuteResponse document. </ows:Title>
      <ows:Abstract>Create an ExecuteResponse document from a sid (Service ID), it will use the niternal ZOO Kernel mechanisms to access the current status from a running Service and update the percentCompleted from the original backup file used by the ZOO Kernel when running a Service in background. </ows:Abstract>
      <ows:Metadata xlink:title="Demo GetStatus request"/>
    </wps:Process>
    <wps:Process wps:processVersion="1.0.0">
      <ows:Identifier>eoepcaadesdeployprocess</ows:Identifier>
      <ows:Title>Eoepca Deploy Process</ows:Title>
      <ows:Abstract>This method will deploy an application encapsulated within a Docker container as a process accessible through the WPS interface.</ows:Abstract>
    </wps:Process>
  </wps:ProcessOfferings>

  ...

```

Now, to complete the tutorial, let's remove the process

```sh
curl -s -L "http://localhost:7777/zoo/?service=wps&version=1.0.0&request=Execute&identifier=eoepcaadesundeployprocess&dataInputs=applicationPackage=https%3A%2F%2Fcatalog.terradue.com%2Feoepca-apps%2Fsearch%3Fformat%3Datom%26uid%3Dapplication_package_sample_app;&ResponseDocument=unDeployResult@mimeType=application/xml"
```

response:

```xml
<?xml version="1.0" encoding="UTF-8"?>
<wps:ExecuteResponse xmlns:ows="http://www.opengis.net/ows/1.1" xmlns:wps="http://www.opengis.net/wps/1.0.0" xmlns:xlink="http://www.w3.org/1999/xlink" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://www.opengis.net/wps/1.0.0 http://schemas.opengis.net/wps/1.0.0/wpsExecute_response.xsd" service="WPS" version="1.0.0" xml:lang="en-US" serviceInstance="http://localhost:7777/zoo-bin/zoo_loader.cgi">
  <wps:Process wps:processVersion="1.0.0">
    <ows:Identifier>eoepcaadesundeployprocess</ows:Identifier>
    <ows:Title>Eoepca Deploy Process</ows:Title>
    <ows:Abstract>This method will undeploy an application encapsulated within a Docker container as a process accessible through the WPS interface.</ows:Abstract>
  </wps:Process>
  <wps:Status creationTime="2020-04-24T10:00:26Z">
    <wps:ProcessSucceeded>The service "eoepcaadesundeployprocess" ran successfully.</wps:ProcessSucceeded>
  </wps:Status>
  <wps:ProcessOutputs>
    <wps:Output>
      <ows:Identifier>unDeployResult</ows:Identifier>
      <ows:Title>Deploy Result</ows:Title>
      <ows:Abstract>The server's response to undeploying a process.  A successful response will contain a summary of the removed process.</ows:Abstract>
      <wps:Data>
        <wps:ComplexData mimeType="application/xml" encoding="ISO-8859-1">
          <result applicationPackageFile="https://catalog.terradue.com/eoepca-apps/search?format=atom&amp;uid=application_package_sample_app">
            <operations packageId="application_package_sample_app">
              <operation processId="sample-app" processVersion="1.12" version="1.12" type="undeploy">
                <wpsId>w05232691f576bae9b5715986e98d0def32065477</wpsId>
                <title>A sample App</title>
                <abstract>Sample abstract</abstract>
                <status err="0" message="0">removed</status>
              </operation>
            </operations>
          </result>
        </wps:ComplexData>
      </wps:Data>
    </wps:Output>
  </wps:ProcessOutputs>
</wps:ExecuteResponse>
```

and GetCapabilities again:

```sh
curl -s -L  "http://localhost:7777/zoo/?service=WPS&version=1.0.0&request=GetCapabilities"
```

```xml
...

<wps:ProcessOfferings>
  <wps:Process wps:processVersion="1.0.0">
    <ows:Identifier>eoepcaadesundeployprocess</ows:Identifier>
    <ows:Title>Eoepca Deploy Process</ows:Title>
    <ows:Abstract>This method will undeploy an application encapsulated within a Docker container as a process accessible through the WPS interface.</ows:Abstract>
  </wps:Process>
  <wps:Process wps:processVersion="1">
    <ows:Identifier>GetStatus</ows:Identifier>
    <ows:Title>Produce an updated ExecuteResponse document. </ows:Title>
    <ows:Abstract>Create an ExecuteResponse document from a sid (Service ID), it will use the niternal ZOO Kernel mechanisms to access the current status from a running Service and update the percentCompleted from the original backup file used by the ZOO Kernel when running a Service in background. </ows:Abstract>
    <ows:Metadata xlink:title="Demo GetStatus request"/>
  </wps:Process>
  <wps:Process wps:processVersion="1.0.0">
    <ows:Identifier>eoepcaadesdeployprocess</ows:Identifier>
    <ows:Title>Eoepca Deploy Process</ows:Title>
    <ows:Abstract>This method will deploy an application encapsulated within a Docker container as a process accessible through the WPS interface.</ows:Abstract>
  </wps:Process>
</wps:ProcessOfferings>
...
```

## Roadmap

See the [open issues](https://github.com/EOEPCA/proc-comm-zoo-svc/issues) for a list of proposed features (and known issues).

<!-- CONTRIBUTING -->

## Contributing

Contributions are what make the open source community such an amazing place to be learn, inspire, and create. Any contributions you make are **greatly appreciated**.

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

<!-- LICENSE -->

## License

Distributed under the Apache-2.0 License. See `LICENSE` for more information.

<!-- CONTACT -->

## Contact

Your Name - [@twitter_handle](https://twitter.com/twitter_handle) - email

Project Link: [https://github.com/EOEPCA/proc-comm-lib-ows](https://github.com/EOEPCA/template-svce)

<!-- ACKNOWLEDGEMENTS -->

## Acknowledgements

- []()
- []()
- README.md is based on [this template](https://github.com/othneildrew/Best-README-Template) by [Othneil Drew](https://github.com/othneildrew).

<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->

[contributors-shield]: https://img.shields.io/github/contributors/EOEPCA/template-svce.svg?style=flat-square
[contributors-url]: https://github.com/EOEPCA/template-svce/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/EOEPCA/template-svce.svg?style=flat-square
[forks-url]: https://github.com/EOEPCA/template-svce/network/members
[stars-shield]: https://img.shields.io/github/stars/EOEPCA/template-svce.svg?style=flat-square
[stars-url]: https://github.com/EOEPCA/template-svce/stargazers
[issues-shield]: https://img.shields.io/github/issues/EOEPCA/template-svce.svg?style=flat-square
[issues-url]: https://github.com/EOEPCA/template-svce/issues
[license-shield]: https://img.shields.io/github/license/EOEPCA/template-svce.svg?style=flat-square
[license-url]: https://github.com/EOEPCA/template-svce/blob/master/LICENSE
[product-screenshot]: images/screenshot.png
