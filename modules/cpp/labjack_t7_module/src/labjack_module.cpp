#include <labjack_module.h>
#include <labjack_t7_module.h>
#include <LabJackM.h>
#include <chrono>
#include <sstream>
#include <thread>
#include <unordered_set>

LabJackT7Controller::LabJackT7Controller(LabJackT7Module* module,
                                         std::string device_name,
                                         std::string device_type,
                                         std::string connection_type,
                                         std::string identifier,
                                         std::string analog_channel_list)
    : module_(module),
      portal_(std::move(device_name)),
      device_type_(std::move(device_type)),
      connection_type_(std::move(connection_type)),
      identifier_(std::move(identifier)),
      address_list_string(std::move(analog_channel_list)),
      control_loop_name_(portal_+"_control_loop"),
      digital_loop_name_(portal_+"_digital_control_loop") {

    // BUILD DEVICE CONTROL LOOP
    module_->dartwic->onStart(control_loop_name_, [&]() { control_loop_on_start(); });
    module_->dartwic->onLoop(control_loop_name_, [&]() { control_loop_on_loop(); });
    module_->dartwic->onEnd(control_loop_name_, [&]() { control_loop_on_end(); });

    // BUILD DIGITAL CONTROL LOOP
    module_->dartwic->onStart(digital_loop_name_, [&]() { digital_control_loop_on_start(); });
    module_->dartwic->onLoop(digital_loop_name_, [&]() { digital_control_loop_on_loop(); });
    module_->dartwic->onEnd(digital_loop_name_, [&]() { digital_control_loop_on_end(); });

}

LabJackT7Controller::~LabJackT7Controller() {
    done=true;

    module_->dartwic->removeLoop(control_loop_name_);
    module_->dartwic->removeLoop(digital_loop_name_);
}

double LabJackT7Controller::query(const std::string& channel, double default_value) const {
    return module_->dartwic->queryChannelValue(portal_, channel, default_value);
}

void LabJackT7Controller::insert(const std::string& channel, double value) const {
    module_->dartwic->insertChannelValue(portal_, channel, value);
}

void LabJackT7Controller::upsert(const std::string& channel, double value) const {
    module_->dartwic->upsertChannelValue(portal_, channel, value);
}

bool LabJackT7Controller::remove(const std::string& channel) const {
    return module_->dartwic->removeChannel(portal_, channel);
}

void LabJackT7Controller::upsertBulk(const std::string& channel,
                                     const std::vector<std::pair<double, uint64_t>>& data) const {
    module_->dartwic->upsertChannelValueBulk(portal_, channel, data);
}

void LabJackT7Controller::consoleError(const std::string& title, const std::string& description, std::vector<std::string> channels,
                                       const std::string& resolution, int auto_ack) const {
    module_->dartwic->consoleError(title, description, channels, resolution, auto_ack);
}

void LabJackT7Controller::get_device_info(int handle) {
    int LJMError;
    int portOrPipe, ipAddress, serialNumber, packetMaxBytes;
    int deviceType = LJM_dtANY;
    int connectionType = LJM_ctANY;
    char string[LJM_STRING_ALLOCATION_SIZE];
    char ipAddressString[LJM_IPv4_STRING_SIZE];
    int handle_local = handle;

    if (handle != -1) {
        LJMError = LJM_GetHandleInfo(handle_local, &deviceType, &connectionType, &serialNumber, &ipAddress,
        &portOrPipe, &packetMaxBytes);
    } else {
        LJMError = -1;
    }

    //if error
    if (LJMError != LJME_NOERROR) {
        //reset device info
        //RAPID DOES NOT SUPPORT STRINGS - I should probably change this
        // upsertTag(currentProject+":labjackT7/device.info.deviceType.value", "-");
        // upsertTag(currentProject+":labjackT7/device.info.connectionType.value", "-");
        // upsertTag(currentProject+":labjackT7/device.info.serialNumber.value", "-");
        // upsertTag(currentProject+":labjackT7/device.info.IP.value", "-");
        // upsertTag(currentProject+":labjackT7/device.info.usbPipe.value", "-");
        // upsertTag(currentProject+":labjackT7/device.info.port.value", "-");
        // upsertTag(currentProject+":labjackT7/device.info.packet_max_bytes.value", "-");
    }
    //if no error
    else {
        // upsertTag(currentProject+":labjackT7/device.info.deviceType.value", std::to_string(deviceType));
        // upsertTag(currentProject+":labjackT7/device.info.connectionType.value", std::to_string(connectionType));
        // upsertTag(currentProject+":labjackT7/device.info.serialNumber.value", std::to_string(serialNumber));
        //
        // if (connectionType == LJM_ctETHERNET || connectionType == LJM_ctWIFI) {
        //     LJM_NumberToIP(ipAddress, ipAddressString);
        //     upsertTag(currentProject+":labjackT7/device.info.IP.value", ipAddressString);
        // } else {
        //     upsertTag(currentProject+":labjackT7/device.info.IP.value", "-");
        // }
        //
        // if (connectionType == LJM_ctUSB) {
        //     upsertTag(currentProject+":labjackT7/device.info.usbPipe.value", std::to_string(portOrPipe));
        // }
        // else {
        //     upsertTag(currentProject+":labjackT7/device.info.port.value", std::to_string(portOrPipe));
        // }
        //
        // upsertTag(currentProject+":labjackT7/device.info.packet_max_bytes.value", std::to_string(packetMaxBytes));
    }
}

int LabJackT7Controller::disconnect_from_device(int handle) {
    int LJMError = LJM_Close(handle);
    //Error
    if (LJMError != LJME_NOERROR) {
        handle_error(LJMError);
    }
    //reset device info and connection tag
    upsert("device.connected", 0);
    //update device info with invalid handle
    get_device_info(-1);

    return LJMError;
}

int LabJackT7Controller::connect_to_device() {
    //set demo mode parameter
    if (strcmp(identifier_.c_str(), LJM_DEMO_MODE) == 0) {
        demoMode = true;
    } else {
        demoMode = false;
    }

    int LJMError;
    LJMError = LJM_OpenS(device_type_.c_str(), connection_type_.c_str(), identifier_.c_str(), &handle);


    //if error
    if (LJMError != 0) {
        upsert("device.connected", 0);
        //set handle
        handle = -1;

        return LJMError;

        //success
    } else {
        //set connected tag
        upsert("device.connected", 1);

        //set device info
        get_device_info(handle);

        return 0;
    }
}

void LabJackT7Controller::handle_error(int &error_number) {
    //get error into error string
    char ErrorString[LJM_MAX_NAME_SIZE] = "LJM error not found.";
    LJM_ErrorToString(error_number, ErrorString);

    std::string title;
    std::string description;
    std::string resolution;
    std::vector<std::string> tags = {};

    //Connection failure
    if (error_number == LJME_RECONNECT_FAILED
        or error_number == LJME_DEVICE_NOT_OPEN
        or error_number == 1314
        or error_number == 1227 //device not found
        ) {
        upsert("device.connected", 0);

        title = "LabJackT7 Connection Error";
        description =
            std::string("The LabJackT7 Device encountered a connection error.")
            + "\n[Device Name: " + portal_ + "]"
            + "\n[LJM Error: " + ErrorString + "]";
        resolution = "Read the exact error code and act accordingly. "
                     "Ensure any wire connections are not broken, and that the network router has power.";
        tags = {"labjackT7/device.connected.value"};

        //Stream Not Initialized
        } else if (error_number == LJME_STREAM_NOT_INITIALIZED or error_number == 2605) {
            //stop stream
            stopStream();

            title = "LabJackT7 Stream Error";
            description = "Stream could not be initialized on the LabJackT7 device.";
            resolution = "Attempt to restart the stream, or cycle power to the device.";

            //Else
        } else {

            title = "LabjackT7 Misc Error [" + std::to_string(error_number) + "]";
            description =
                std::string("The LabJackT7 Device Encountered an Error")
                + "\n[Device Name: LabJackT7]"
                + "\n[LJM Error: " + ErrorString + "]";

            resolution = "Evaluate Error";;
        }

    //print error
    consoleError(title, description,tags, resolution,5);
}

int LabJackT7Controller::getChannelAddressFromName(const std::string& registerName) {
    int address;
    int type;
    int result = LJM_NameToAddress(registerName.c_str(), &address, &type);

    //Error
    if (result != LJME_NOERROR) {
        handle_error(result);
        return -1;  // Return -1 for error
    }

    return address;
}

//attempts to read serial number register from device handle, will return a reconnection error if not connected.
void LabJackT7Controller::verify_connection() {
    double value;
    int error = LJM_eReadName(handle, "SERIAL_NUMBER", &value);

    //Success
    if (error == LJME_NOERROR) {
        //if was previously unconnected
        int connected = query("device.connected", 0.0);
        if (connected == 0) {
            //update connection tag
            upsert("device.connected", 1);
            //update device info
            get_device_info(handle);

            //Error
        }
    } else {
        handle_error(error);
    }
}

std::string LabJackT7Controller::determineChanelType(const std::string channelName) {

    // ANALOG CHANNEL
    if (channelName.find("AIN") != std::string::npos) {
        return "analog";

        // OTHER
    } else {
        return "other";;
    }
}

std::vector<std::string> LabJackT7Controller::findRemovedElements(
    const std::vector<std::string>& oldList,
    const std::vector<std::string>& newList)
{
    std::unordered_set<std::string> newSet(newList.begin(), newList.end());
    std::vector<std::string> removedElements;

    for (const auto& item : oldList) {
        if (newSet.find(item) == newSet.end()) {
            removedElements.push_back(item);
        }
    }

    return removedElements;
}

std::vector<std::string> LabJackT7Controller::splitCommaSeparated(const std::string& input) {
    std::vector<std::string> result;
    std::stringstream ss(input);
    std::string item;

    while (std::getline(ss, item, ',')) {
        // Trim whitespace
        item.erase(0, item.find_first_not_of(" \t"));
        item.erase(item.find_last_not_of(" \t") + 1);
        if (!item.empty()) {
            result.push_back(item);
        }
    }

    return result;
}

void LabJackT7Controller::control_loop_on_start() {
    module_->dartwic->upsertChannelValue("servervariables", "loops."+control_loop_name_+".target_loop_frequency", 100);
}


void LabJackT7Controller::control_loop_on_loop() {
    ///// DEVICE CONNECTION /////
    //if no valid handle is set, attempt to initiate connection
    if (handle == -1) {
        auto result = connect_to_device();
        if (result != LJME_NOERROR) {
            handle_error(result);
        }
    //if valid handle is set, run further operations below
    } else {

        //// VERIFY CONNECTION ////
        verify_connection();

        //// STREAM START ////
        //IF STREAM IS NOT STARTED
        if (!streamStarted) {
            auto error = startStream();

            //Error
            if (error != LJME_NOERROR) {
                handle_error(error);
            }

        //IF STREAM IS STARTED
        } else {
            //IF DEMO MODE IS ACTIVE
            if (demoMode) {
                //call callback function to call eStreamRead & simulate data gather time - then get data
                streamCallback(nullptr);
            }
        }


        ///// STATE CHECKS /////
        /// STREAM SETTINGS DETECTION
        //if any stream parameters change, restart stream.
        //scan rate, scans per read
        int initScanRate_tag = query("stream.target_scan_rate");
        int scansPerRead_tag = query("stream.scans_per_read");
        if (initScanRate_tag != initScanRate
            or scansPerRead_tag != scansPerRead) {

            //reset stream
            resetStream();
        }

        int reset_stream = query("stream.reset_stream", 0.0);
        if (reset_stream == 1) {

            resetStream();
            upsert("stream.reset_stream", 0);
        }

        ///ANALOG CHANNEL SETTINGS CHECKS
        analogChannelSettingsCheck(false);


        /// DIAGNOSTICS
        //backlog diagnostics
        upsert("stream.device_scan_backlog", DeviceScanBacklog);
        upsert("stream.ljm_scan_backlog", LJMScanBacklog);

        //queue size diagnostics
        int queue_size_approx = streamDataQueue.size_approx();
        upsert("stream.data_queue.size_approx", queue_size_approx);

        //stream read diagnostics
        upsert("stream.read_number", streamReadNumber);
    }
}

void LabJackT7Controller::control_loop_on_end() {
    //stop stream
    stopStream();
    //disconnect from any handle that is set
    disconnect_from_device(handle);
}
