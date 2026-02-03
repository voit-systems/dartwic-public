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


# Architecture
DARTWIC is a monorepo, containing a few different software projects put together, to create the unified controls and data acquisition platform.
- [Engine](/releases/engines): The main project that combines RAPID, CAESAR, and TEMPEST, to create a unified controls and data acquisition platform.
- [Interface](releases/interfaces): The user-interface that is used to interact with DARTWIC. Written in ReactJS. Contains a frontend webserver UI, and an desktop app that is built using Electron.
- **RAPID**: Real-time Access Parallel In-memory Database. A database that is highly preformant in multi-threaded environments, focused around data aquisition, automation, and control aplications. DARTWIC uses this to store data and commands called "channels", directly in memory for fastests realtime access, and records historical data to disk.
- **CAESAR**: Command Authority Engine for Sensitive Automated Resources. Commanding engine for DARTWIC. This contains the threaded loops that users can create in DARTWIC, and will also later implement command authority triplication by allowing multiple RAPID or DARTWIC nodes to be triplicaited, and voting on the final command in the threaded loops.
- **TEMPEST**: Transaction &amp; Event Messaging Platform for Encrypted System Telemetry. A message router that routes all data and commands to and from DARTWIC node. Client libraries, TAARS, and the interface app will use this to connect to a DARTWIC node.
- [SDKS](/sdks): Software Development Kits for embedding functionality into DARTWIC. Includes creating additional user interface components via the ReactJS SDK (planned), or C++ device drivers via the Modules SDK.
- [Modules](/modules): A collection of device drivers and user interface components created using the DARTWIC SDK's.
- [Clients](/clients): Client libraries for connecting to a DARTWIC node. Planned libraires include Python, C++, and ReactJS.
