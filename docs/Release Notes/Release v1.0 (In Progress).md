---
updated: 2026-04-21T22:50
created: 2026-03-15T23:16
---

> [!warning] PRE-RELEASE
> This release is marked as a pre-release, and is in a beta state. Like all new code, it needs to be treated with caution.
> More tests with CAESAR & RAPID need to be completed to verify performance and reliability at large data and ingestion scales, and in complex automation environments. 
> However, v1.0 has been tested multiple times at small scale with no issues.

# Release v1.0
---
Release v1.0 will be the first major public facing release for DARTWIC.

The codebase of DARTWIC has been completely restructured. Major architectural restructuring in both the codebase and devops, which leaves DARTWIC in a much better place development wise than before.

Additionally, countless new features to increase user experience as well as customizability have been added.

### Codebase Restructure
The server (now named engine) and the graphical user interface (now named interface) have been completely separated. This allows for separate distribution of the engine and interface, and is a more efficient and scalable way to develop this project. 

The main benefit for separating the engine and interface comes from the introduction of Client libraries. DARTWIC was always built in a server-client orientation, however, as stated above, both the client and the server were embedded into the same process, with no API to allow outside orchestration from other clients.

### TEMPEST
Now, DARTWIC has a 3rd sub module, TEMPEST. TEMPEST is a transport agnostic messaging and telemetry platform. It supports web clients, and raw TCP clients. Any outside process capable of raw TCP or web communication can send commands, receive queries, and telemetry from the DARTWIC engine. TEMPEST  exposes "operations" for RPC style transport, and "telemetry" for real time data.

### RAPID & CAESAR Upgrades
Additionally, updates have been made to both CAESAR and RAPID, allowing for a better data acquisition and commanding experience for the user. 

Data is now completely recorded within RAPID, and outside databases are no longer needed. Data can be linked to a "data frame", which is a way to mark certain channels to certain tests or events. Use the data frame searcher in the interface to immediately see the existing data frames, what is recorded and currently being recorded, the span of time that exists within the data frame, and more.

Use the telemetry viewer to query all data of a channel and view it in a graph. The graph supports panning an zooming, real time data, historical queries, and exporting to CSV.

CAESAR implements a new thread type: Tasks, and changes to the existing type, Loops. Loops are now used as an internal development tool to engine state checking and automation, and to drive Tasks. 

Tasks are a new and convenient way to view automations within DARTWIC. Use the task searcher to view all currently registered tasks. Start, stop, or hold tasks. View task meta data, like start, stop, and loop source from python, or configure data for more complex tasks. Tasks can easily be created, with Task Types. Task Types define the automation to run, and what data can be configured. Tasks run the actual automation and do the actual work, like for example, a Modbus Read Task, binded to the PLC module. 

RAPID now has command authority orchestration, when paired with CAESAR. Channels can be marked by CAESAR to explain and orchestrate their commanding permissions. Channels owned by automation Tasks will have commanding disabled while the task is running, with options for manual overrides by operators. Channels marked as observe only will be read-only, owned by their CAESAR loop or task. 

### Get Started
---
Download

# Summary
---

v1.0 includes (so far):
- Separate engine and frontend clients
	- Engine: Server that combines RAPID, CAESAR, TEMPEST, and other features.
	- Interface App: Webserver that uses TEMPEST client to connect to the engine
		- Includes Webserver app, and Windows desktop app.
- TEMPEST transport agnostic comms platform
	- Raw TCP sockets
	- Web transport
	- Built in client registration, data, culling, and heartbeat
	- Request backpressure
- More RAPID fields  
	- stale_timeout - quickly identify stale data in the user interface
	- commanded_by - thread or CAESAR id
	- data_frame - organize data by "test"
- RAPID embedded writer & recovery features
	- no more influx db, data is recorded internally by RAPID
	- saves channel snapshot on shutdown & reloads on startup
- Client libraries
	- Javascript TCP & Web, & Python TCP TEMPEST low-level libraries
	- React, and Python TCP DARTWIC higher level libraries
- Engine shutdown
	- send shutdown command to shutdown RAPID, CAESAR, TEMPEST to safely record all data, shutdown all tasks, and stop server
- Dataframe searcher
	- Query dataframes to view all data by test. View channels currently recording, not recording, or set to record in the dataframe
	- Easily set channels to record to a separate dataframe
	- Select channels and easily view in exporter
	- View channels in channel searcher instantly
- Channel Exporter / Telemeter
	- Graph to view real time and historical channel data
	- Easily set query range and time window
	- Pan / zoom
	- Export all data and download CSV to browser
	- Auto bucketing to help w/ viewing large windows
- Schematic Overhauls
	- Editing mode: pan/ drag/ zoom, add & configure nodes. Commanding channels/ buttons is not possible
	- Operation mode: no panning or draging, schematic autofits nodes, commanding allowed
- Context Panel
	- Bottom drawer to host configurations for anything in dartwic (schematic nodes, etc)
- New Channel Searcher
	- Optimized graphs
	- Channel rows show only value, upon clicking subrows appear with all other fields to set or view
	- Select multiple channels to bulk set fields values or open in Channel Exporter
	- Copy channel name button to help w/ writing automation sequences
	- No longer hardlocked to 20 channels - channel seracher views all channels in RAPID


# Interface Changes
---
### Channel Drops
#### Python Drops
#### Schematic Drops
#### Telemetry Viewer Drops

### Light Mode

![](_attachments/Pasted%20image%2020260421225031.png)


![[Pasted image 20260326232847.png]]


### Settings

### Context

### Updated Channel Searcher

![[Pasted image 20260326010830.png]]

### Dataframe Searcher

![[Pasted image 20260326010950.png]]

![[Pasted image 20260326011006.png]]

### New Schematic Layout

Talk about easier schematic setups for adding channels to nodes and making button and valves.

![[Pasted image 20260326011039.png]]


### Telemetry Viewer

![[Pasted image 20260326011105.png]]

![[Pasted image 20260326011114.png]]

### Tasks

![[Pasted image 20260326011146.png]]

![[Pasted image 20260326011317.png|428]]

![[Pasted image 20260326011259.png|431]]

![[Pasted image 20260326011355.png]]

#  Module API Changes & Packages
---

### Engine Modules Updates

## New Interface Modules

## Interface Package Manager


# Clients Libraries
---

### Python Client

![[Pasted image 20260326011435.png|519]]

![[Pasted image 20260326011452.png|551]]

### React Client



# Engine Changes
---

### RAPID Updates

### Command Authority

