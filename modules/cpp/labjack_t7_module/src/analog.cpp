//
// Created by kemptonburton on 1/19/2025.
//

#include <labjack_module.h>
#include <LabJackM.h>
#include <blockingconcurrentqueue.h>
#include <random>


/* STREAM TIPS
 * Scan Rate = rate at which each analog channel is read
 * Scans Per Read = number of analog channel values that is returned on each LabJack T7 data thread loop
 * Do not let backlog increase steadily. If it is, increase scans per read to pull more values out of the backlog.
 * The more analog channels you want to recieve, at a high frequency, the slower the LabjackT7 data thread loop will run (more latency)
 * eStreamRead frequency = ScanRate / ScansPerRead
 */

void LabJackT7Controller::analogChannelSettingsCheck(bool build) {
    bool settings_changed = false;

    {
        std::lock_guard<std::mutex> lock_analog_channel_settings(analog_channel_settings_mutex);
        std::lock_guard<std::mutex> lock_channelList(channelListMutex);

        //if we are building for the first time
        if (build) {
            //clear current map
            analog_channel_settings.clear();
        }

        //go through channel list and check tags
        for (int channelNum = 0; channelNum < channelList.size(); ++channelNum) {
            auto channelName = channelList[channelNum];

            //if channel type is not analog, skip
            std::string type = determineChanelType(channelName);
            if (type != "analog") continue;

            //NEGATIVE CHANNEL
            double negative_channel_tag = query("stream_data.analog_channels."+channelName+".negative_channel", 199.0);
            int negative_channel = analog_channel_settings[channelName]["negative_channel"];
            //if what the user wants is different than what is currently set
            if (negative_channel_tag != negative_channel) {

                //GND
                if (negative_channel_tag == 199) {
                    //write GND
                    auto error = LJM_eWriteName(handle, (channelName+"_NEGATIVE_CH").c_str(), 199);
                    if (error != LJME_NOERROR) {
                        handle_error(error);
                    }

                    //update the map - default if error also
                    analog_channel_settings[channelName]["negative_channel"] = 199;
                    settings_changed = true;

                //NOT GND
                } else {

                    //get just number from negative channel tag
                    int negative_channel_numbers_only = negative_channel_tag;

                    //attempt to write the negative channel
                    int error = LJM_eWriteName(handle, (channelName+"_NEGATIVE_CH").c_str(), negative_channel_numbers_only);

                    //if error
                    if (error != LJME_NOERROR) {
                        //default to GND
                        LJM_eWriteName(handle, (channelName+"_NEGATIVE_CH").c_str(), 199);
                        //update the map
                        analog_channel_settings[channelName]["negative_channel"] = 199;
                        //update the tag
                        upsert("stream_data.analog_channels."+channelName+".negative_channel", 199);

                        handle_error(error);

                    //no error
                    } else {
                        //update internal map
                        analog_channel_settings[channelName]["negative_channel"] = negative_channel_tag;
                        settings_changed = true;
                    }
                }
            }

            //RANGE
            double range_tag = query("stream_data.analog_channels."+channelName+".range", 10.0);
            double range = analog_channel_settings[channelName]["range"];

            if (range_tag != range) {
                //attempt to write the range
                int error = LJM_eWriteName(handle, (channelName+"_RANGE").c_str(), range_tag);

                //if error
                if (error != LJME_NOERROR) {
                    //default to 10.0
                    LJM_eWriteName(handle, (channelName+"_RANGE").c_str(), 10.0);
                    //update the map
                    analog_channel_settings[channelName]["range"] = 10.0;
                    //update the tag
                    upsert("stream_data.analog_channels."+channelName+".range", 10.0);

                    handle_error(error);


                //no error
                } else {
                    //update internal map
                    analog_channel_settings[channelName]["range"] = range_tag;
                    settings_changed = true;
                }
            }
        }
    }

    //if any LJM analog settings were changed, restart stream. (IF WE ARE NOT BUILDING)
    if (settings_changed && !build) {
        //reset stream
        resetStream();
    }
}

void LabJackT7Controller::removeUnusedChannels(const std::vector<std::string> new_list) {
    std::vector<std::string> old_list;
    {
        std::lock_guard<std::mutex> lock(channelListMutex);
        old_list = channelList;
    }

    //get removed channels
    const std::vector<std::string> removed_channels = findRemovedElements(old_list, new_list);

    for (int channelNum = 0; channelNum < removed_channels.size(); ++channelNum) {

        std::string channelName = removed_channels[channelNum];
        //delete the stream data tag
        remove("stream_data.analog_channels."+channelName);
        remove("stream_data.analog_channels."+channelName+".negative_channel");
        remove("stream_data.analog_channels."+channelName+".range");

        //delete the mapped tag name
        //function above deletes all mapped channels associated with the channels
    }
}

int LabJackT7Controller::startStream() {

    try {
        ///// INIIT TAGS /////
        /// STREAMING ///
        //stream start status
        insert("stream.enabled", 0);

        //GLOBAL VARIABLES FOR STREAM//
        //scan rate
        double scan_rate_default = 100.0;
        initScanRate = query("stream.target_scan_rate", scan_rate_default);

        //scans per read
        double scans_perRead_default = 10.0;
        scansPerRead = query("stream.scans_per_read", scans_perRead_default);

        //consumer threads desired count initial value
        double default_consumer_count_desired = 1.0;
        insert("stream.data_consumer_threads.desired_count", default_consumer_count_desired);

        ///// STREAM START /////
        //Addresses to scan
        auto [aScanList, channelList_copy] = parseAnalogChannels(address_list_string);
        //remove unused channels
        removeUnusedChannels(channelList_copy);
        {
            std::lock_guard<std::mutex> lock(channelListMutex);
            channelList = channelList_copy;
        }
        int numAddresses = aScanList.size();

        //Scans
        scanRate = initScanRate; // scan rate - set the current global scan rate to the initial scan rate.
        // this global scan rate is set by the labjack LJM thread - to reflect actual scan rate. we must initialize it however.

        //create new a data variable
        aDataSize = numAddresses * scansPerRead;
        if (aData) {
            delete[] aData;
            aData = nullptr;  // Prevents dangling pointers
        }
        aData = new double[aDataSize];

        //Analog channel settings
        analogChannelSettingsCheck(true);

        // Start the stream
        //DEMO mode does not support stream
        int error;
        if (!demoMode) {
            error = LJM_eStreamStart(handle, scansPerRead, numAddresses, aScanList.data(), &scanRate);
        } else {
            //if demo mode is active, don't actually start stream, but proceed as normal.
            error = LJME_NOERROR;
        }

        //error
        if (error != LJME_NOERROR) {
            handle_error(error);
            upsert("stream.enabled", 0);
            streamStarted = false;
            done = true;

        //no error
        } else {
            upsert("stream.actual_scan_rate", scanRate);
            //stream is started
            upsert("stream.enabled", 1);
            streamStarted = true;

            //tell consumers there is work for them
            done = false;
            //add consumer threads that are desired
            int consumer_count_desired = query("stream.data_consumer_threads.desired_count");
            int i = 0;
            for (int i = 0; i < consumer_count_desired; i++) {
                consumerLabJackT7StreamThreads.emplace_back(
                    &LabJackT7Controller::streamDataConsumer,
                    this
                );
            }
            upsert("stream.data_consumer_threads.count", static_cast<int>(consumerLabJackT7StreamThreads.size()));

            //stream callback
            auto callback = [](void* data) {
                auto* self = static_cast<LabJackT7Controller*>(data);
                if (self) {
                    auto error = self->readStream();
                    if (error != LJME_NOERROR) {
                        self->handle_error(error);
                    }
                }
            };

            //set stream callback (called once stream data is available)
            auto error = LJM_SetStreamCallback(handle, callback, this);
            if (error != LJME_NOERROR) {
                handle_error(error);
            }

            /// FOR ABSOLUTE TIME CALCS ///
            auto now_system = std::chrono::system_clock::now(); // absolute time
            streamAbsoluteStartNs = std::chrono::duration_cast<std::chrono::nanoseconds>(
                now_system.time_since_epoch()
            ).count();

            // Copy atomic scan params
            scansPerRead_atomic = scansPerRead;
            scanRate_atomic = scanRate;

            // Initialize read number
            streamReadNumber = 0;
        }

        return error;
    } catch (const std::exception& e) {

    }
}

void LabJackT7Controller::streamCallback(void* data) {

    //call read stream function
    auto error = readStream();
    if (error != LJME_NOERROR) {
        handle_error(error);
    }
}

void LabJackT7Controller::stopStream() {

    try {
        //stop stream
        auto error = LJM_eStreamStop(handle);
        //error
        if (error != LJME_NOERROR) {
            handle_error(error);
        }

        upsert("stream.enabled", 0);
        streamStarted = false;

        //join consumer threads - they will block the thread they were created in until they are done
        //consumer threads are done with work
        done = true;
        //they will block the labjackt7_stream loop until done.
        for (auto &consumerThread : consumerLabJackT7StreamThreads) {
            for (int i = 0; i < consumerLabJackT7StreamThreads.size(); i++) {
                streamDataQueue.enqueue({-1, {}});  // Sentinel value
            }
            consumerThread.join();
        }

        //remove all consumer threads from list - once they are done with their work
        consumerLabJackT7StreamThreads.clear();
        upsert("stream.data_consumer_threads.count", static_cast<int>(consumerLabJackT7StreamThreads.size()));

        auto error1 = LJM_SetStreamCallback(handle, 0, nullptr);
    } catch (const std::exception& e) {

    }
}

void LabJackT7Controller::resetStream() {
    stopStream();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    startStream();
}

int LabJackT7Controller::readStream() {
    //Mutated variables
    //all global variables

    int result;

    //IF DEMO MODE
    if (demoMode) {
        if (aDataSize <= 0) {
        }

        // Allocate memory for `aData`
        if (aData) {
            delete[] aData;
            aData = nullptr;  // Prevents dangling pointers
        }
        aData = new double[aDataSize];

        // Seed the random number generator
        std::random_device rd;
        std::mt19937 gen(rd()); // Mersenne Twister RNG
        std::uniform_real_distribution<double> dist(1.0, 10.0); // Range [1.0, 10.0]

        // Populate `aData` with random values (e.g., between 0 and 1)
        for (int i = 0; i < aDataSize; ++i) {
            aData[i] = dist(gen);
        }

        result = LJME_NOERROR;

        //simulate time to get data from labjack device
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(1/scanRate * scansPerRead * 1000)));

    //IF NOT DEMO MODE
    } else {
        //Attempt to read stream buffer
        result = LJM_eStreamRead(handle, aData, &DeviceScanBacklog, &LJMScanBacklog);
    }

    //No Error
    if (result == LJME_NOERROR) {
        ++streamReadNumber;
        int read_number_copy = streamReadNumber; //need to make a copy since we DO NOT want to pass a reference to the global stream number

        //COPY STREAM DATA aDATA INTO A VECTOR
        std::vector copiedData(aData, aData + aDataSize);

        //PLACE INTO QUEUE
        std::pair stream_data(read_number_copy, copiedData);
        streamDataQueue.enqueue(stream_data);
    }

    return result;
}

void LabJackT7Controller::processStreamData(std::pair<int, std::vector<double>> stream_data) {
    int readNumber_local = stream_data.first;
    const std::vector<double>& readData = stream_data.second;

    // Local copy of channel list
    std::vector<std::string> channelList_local;
    {
        std::lock_guard<std::mutex> lock(channelListMutex);
        channelList_local = channelList;
    }

    // Calculate starting scan number for this batch
    int scanNumber = (readNumber_local * scansPerRead_atomic) - scansPerRead_atomic;

    std::unordered_map<std::string, std::vector<std::pair<double, uint64_t>>> stream_channels_tag_group;

    for (int readDataScanNum = 0; readDataScanNum < readData.size(); readDataScanNum += channelList_local.size()) {
        scanNumber++;

        // Time since stream start in nanoseconds
        int64_t scanNanosecondsSinceStreamStart = static_cast<int64_t>(
            (static_cast<double>(scanNumber) / scanRate_atomic) * 1'000'000'000.0
        );

        // Absolute timestamp since UNIX epoch
        int64_t scanNanosecondsSinceEpoch = streamAbsoluteStartNs + scanNanosecondsSinceStreamStart;

        for (int scanIndex = 0; scanIndex < channelList_local.size(); ++scanIndex) {
            std::string channelName = channelList_local[scanIndex];
            std::string type = determineChanelType(channelName);
            double value = readData[readDataScanNum + scanIndex];

            if (type == "analog" || type == "other") {
                stream_channels_tag_group["analog_channels." + channelName].emplace_back(value, scanNanosecondsSinceEpoch);
            } else {
                stream_channels_tag_group["other_channels." + channelName].emplace_back(value, scanNanosecondsSinceEpoch);
            }
        }
    }

    // Upsert to in-memory DB
    for (auto& [key, value] : stream_channels_tag_group) {
        upsertBulk(key, value);
    }
}

void LabJackT7Controller::streamDataConsumer() {

    while (!done) {

        std::pair<int, std::vector<double>> stream_data;

        // dequeue an item
        streamDataQueue.wait_dequeue(stream_data);

        // check for sentinel value
        if (stream_data.first == -1) {
            continue;
        }

        // Process the dequeued data
        processStreamData(stream_data);
    }
}

// Function to parse a string of analog channels and return their integer addresses
std::pair<std::vector<int>, std::vector<std::string>> LabJackT7Controller::parseAnalogChannels(const std::string& channels) {
    std::vector<int> aScanList;
    std::vector<std::string> channelList;

    // Split the input string by commas and parse each channel
    std::stringstream ss(channels);
    std::string channel;
    while (std::getline(ss, channel, ',')) {
        // Remove spaces and check for the "AIN" prefix
        channel.erase(0, channel.find_first_not_of(" \t"));  // Trim leading spaces
        channel.erase(channel.find_last_not_of(" \t") + 1);  // Trim trailing spaces

        //Add channel to channel list
        channelList.push_back(channel);

        // Get the channel address using the LJM_NameToAddress function
        int channelNumber = getChannelAddressFromName(channel);
        if (channelNumber != -1) {
            aScanList.push_back(channelNumber);
        }
    }

    return {aScanList, channelList};
}


