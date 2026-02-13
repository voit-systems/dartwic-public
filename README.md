# DARTWIC
Public facing releases, software development kits, documentation, issue trackers, and much more for DARTWIC

# What is DARTWIC?
Data Acquisition for Rocket Testing With Integrated Controls.

DARTWIC is a collection of software projects, all put together to create a unified control and data acquisition platform.
Its purpose: to unify and create a standard framework for controlling, monitoring, and operating any physical system, ever.
This includes, but is not limited to: satellites, spacecraft, engines, habitats, rovers, factories, etc.

Anything that can have data collected from it, and/ or commands sent to it, DARTWIC can be used for.

DARTWIC was initially created to gather realtime data from hybrid rocket engines during testing, and to operate the engine itself. 

<img width="1917" height="1078" alt="Screenshot_2025-11-08_001012" src="https://github.com/user-attachments/assets/55d6f7dd-12b6-4346-9eba-785c7b2c0fcc" />
<img width="1919" height="1079" alt="Screenshot_2025-11-08_000620" src="https://github.com/user-attachments/assets/fe93b094-c331-4da8-9b97-2502e838412a" />
<img width="3447" height="1944" alt="screenshot_2" src="https://github.com/user-attachments/assets/0d49caa9-0634-44f5-b03d-73420ab1f5ee" />
<img width="2559" height="1439" alt="newdartwicui" src="https://github.com/user-attachments/assets/deb3e2ea-2e06-4c8d-9b35-b5d617a0d471" />


# Current Architecture
DARTWIC is a monorepo, containing a few different software projects put together, to create the unified controls and data acquisition platform.
- [Engine & Interface](https://github.com/voit-systems/dartwic-public/releases): The main project that combines RAPID, and CAESAR, to create a unified controls and data acquisition platform. Also includes a webserver that runs a user interface that is used to interact with DARTWIC. Written in ReactJS.
- **RAPID**: Real-time Access Parallel In-memory Database. A database that is highly preformant in multi-threaded environments, focused around data aquisition, automation, and control aplications. DARTWIC uses this to store data and commands called "channels", directly in memory for fastests realtime access, and records historical data to disk via an InfluxDB server.
- **CAESAR**: Command Authority Engine for Sensitive Automated Resources. Commanding engine for DARTWIC. This contains the threaded loops that users can create in DARTWIC, and will also later implement command authority triplication by allowing multiple RAPID or DARTWIC nodes to be triplicaited, and voting on the final command in the threaded loops.
- [SDKS](/sdks): Software Development Kits for embedding functionality into DARTWIC. Includes creating C++ device drivers via the Modules SDK.
- Modules: A collection of device drivers created using the DARTWIC SDK's. View [source code](/modules) for driver programming examples, or download directly from [releases](https://github.com/voit-systems/dartwic-public/releases). 
- [Demos](/demos): Examples of projects that are built using DARTWIC. TESTING_NETWORK was used to test and operate a hybrid rocket engine, with sensor alarms, operator prompts for engine autofilling and autoignition, and a mission timeline clock.

# Getting Started
All DARTWIC engine releases contain an autofill software-in-the-loop simulation example.

To view an example project that uses DARTWIC, view the [demos](/demos) folder. The TESTING_NETWORK project is a project used by the Texas A&M Sounding Rocketry Team to gather data, visualize and control rocket systems, and automate procedures for operating and testing hybrid rocket engines.

To view examples of custom drivers built for DARTWIC, view the [modules](/modules) folder. There, you can find a ModBus client module to connect to ModBus devices, with more drivers in the works.

# Setup/ Other Programs Needed
The current releases of DARTWIC uses InfluxDB to record channel values to disk. To view these values, you must install the InfluxDB explorer. Read more here: https://docs.influxdata.com/influxdb3/explorer/install/

DARTWIC runs a local InfluxDB server.

The GUI can only be installed via Docker, which is dumb as hell in my opinon. DARTWIC version 0.5 will replace InfluxDB with SQLite, and all data viewing & downloading will be through DARTWIC interface.

# Documentation
[Documentation](/documentation) is very sparse right now, but more will be added later. 

# Bugs
DARTWIC is a complex project. Reporting bugs would be appreciated to help development!

## Current Bugs

### Performance
Graphs currently are very browser intensive. So if your browser is lagging, for example, when u have multiple schematics open and the channel search up, close the channel search or some schematics to increase performance. Or just open another browser tab to distribute the load.

### Interface
1. While having a python script and schematic open at the same time, the spacebar does not work/ register key presses. To fix, close all opened schematics.
2. Channel search does not show all channels in the RAPID database. Search for a channel to find it. The channel search is one of the first features made, it is in need of a rework, which will come in version 0.5.

# Path to 0.5 Release
DARTWIC is undergoing some very intensive architectural changes at the moment. This includes a new communication platform, TEMPEST, updated database RAPID, the separation of the Engine and Interface, and the release of Client libraries and SDK's for custom interface components. The aim for DARTWIC 1.0 is to deliver a fully distributed server-client stack capable of cross-network communication with other DARTWIC nodes, custom clients, and a configurable interface, with all the features wanted in a data acquisition, controls, and mission operations software.
