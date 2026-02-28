#ifndef LABJACK_T7_MODULE_LABJACK_MODULE_H
#define LABJACK_T7_MODULE_LABJACK_MODULE_H

#include <array>
#include <atomic>
#include <blockingconcurrentqueue.h>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>
#include <LabJackM.h>

class LabJackT7Module;

class LabJackT7Controller {
public:
    LabJackT7Controller(LabJackT7Module* module,
                        std::string device_name,
                        std::string device_type,
                        std::string connection_type,
                        std::string identifier,
                        std::string analog_channel_list);
    ~LabJackT7Controller();

    void start();
    void stop();

    // Analog API
    int startAnalogStream();
    void stopAnalogStream();
    void resetAnalogStream();
    void applyAnalogSettings(bool build);

    // Digital API
    void applyDigitalOutputs();
    int enableDigitalClock(const std::string& channel);

private:
    // module context
    LabJackT7Module* module_;
    std::string portal_;

    // configuration
    std::string device_type_;
    std::string connection_type_;
    std::string identifier_;
    std::string analog_channel_list_;

    // loop names
    std::string control_loop_name_;
    std::string digital_loop_name_;


    // helpers
    double query(const std::string& channel, double default_value = 0.0) const;
    void insert(const std::string& channel, double value) const;
    void upsert(const std::string& channel, double value) const;
    bool remove(const std::string& channel) const;
    void upsertBulk(const std::string& channel, const std::vector<std::pair<double, uint64_t>>& data) const;
    void consoleError(const std::string &title, const std::string &description, std::vector<std::string> channels, const std::string &
                      resolution, int auto_ack = 5) const;

    //---------- DEVICE CONTROL ----------//
    ///// VARIABLES /////
    int handle = -1;
    std::atomic_bool demoMode = false;
    std::string address_list_string = "AIN0, AIN1";

    ///// FUNCTIONS /////
    void get_device_info(int handle);
    int disconnect_from_device(int handle);
    int connect_to_device();
    void handle_error(int &error_number);
    int getChannelAddressFromName(const std::string& registerName);
    void verify_connection();
    std::string determineChanelType(const std::string channelName);
    std::vector<std::string> findRemovedElements(const std::vector<std::string>& oldList,
        const std::vector<std::string>& newList);
    std::vector<std::string> splitCommaSeparated(const std::string& input);

    void control_loop_on_start();
    void control_loop_on_loop();
    void control_loop_on_end();


    //---------- ANALOG STREAM ----------//
    ///// VARIABLES /////
    // Flag to signal consumers that the producer is done
    std::atomic<bool> done = false;

    //start stream data
    bool streamStarted = false;
    int aDataSize = 0;

    std::mutex channelListMutex;
    std::vector<std::string> channelList = {};

    int numAddresses = 0;

    //start parameters
    int initScanRate = 0;
    int scansPerRead = 0;

    //data recieved by stream initializer
    double scanRate = 0;

    //data recieved by stream
    double *aData = nullptr;
    std::mutex aDataMutex;

    int DeviceScanBacklog = 0;
    int LJMScanBacklog = 0;

    // PROCESS DATA //
    //variables for absolute time calcs
    //to keep track of total number of scans
    std::atomic<int> streamReadNumber = 0;

    //atomic variables for absolute time calcs by consumer threads
    std::atomic<int64_t> streamAbsoluteStartNs{0}; // nanoseconds since UNIX epoch
    std::atomic<int64_t> scansPerRead_atomic = 0;
    std::atomic<int64_t> scanRate_atomic = 0;

    //stream data queue
    moodycamel::BlockingConcurrentQueue<std::pair<int, std::vector<double>>, moodycamel::ConcurrentQueueDefaultTraits> streamDataQueue;
    std::vector<std::thread> consumerLabJackT7StreamThreads = std::vector<std::thread>();

    //analog channel settings - to keep track of tag changes
    std::unordered_map<std::string, std::unordered_map<std::string, double>> analog_channel_settings;
    std::mutex analog_channel_settings_mutex;

    ///// FUNCTIONS /////
    void analogChannelSettingsCheck(bool build);
    void removeUnusedChannels(const std::vector<std::string> new_list);
    int startStream();
    void streamCallback(void* data);
    void stopStream();
    void resetStream();
    int readStream();
    void processStreamData(std::pair<int, std::vector<double>> stream_data);
    void streamDataConsumer();
    std::pair<std::vector<int>, std::vector<std::string>> parseAnalogChannels(const std::string& channels);


    //---------- DIGITAL CONTROL ----------//
    ///// VARIABLES /////
    std::unordered_map<std::string, std::array<int,2>> PWM_DIO_CURRENT_STATE = {
        {"DIO0", {0,0}},
        {"DIO1", {0,0}},
        {"DIO2", {0,0}},
        {"DIO3", {0,0}},
        {"DIO4", {0,0}},
        {"DIO5", {0,0}}
    };

    bool clk_set = false;
    bool encoder = false;

    ///// FUNCTIONS /////
    int enable_clk(int handle, const std::string& channel);

    void digital_control_loop_on_start();
    void digital_control_loop_on_loop();
    void digital_control_loop_on_end();


};

#endif
