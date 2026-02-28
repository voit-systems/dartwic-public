#include "labjack_t7_module.h"

#include <iostream>

#include "labjack_module.h"

LabJackT7Module::LabJackT7Module(YAML::Node cfg, DARTWIC::API::SDK_API* drtw)
    : BaseModule(cfg, drtw) {
    device_name_ = getParameter<std::string>("device_name", "labjackT7");

    std::string device_type = getParameter<std::string>("device_type", "T7");
    std::string connection_type = getParameter<std::string>("connection_type", "ANY");
    std::string identifier = getParameter<std::string>("identifier", "ANY");
    std::string analog_channel_list = getParameter<std::string>("analog_channel_list", "AIN0");

    controller_ = std::make_unique<LabJackT7Controller>(
        this,
        device_name_,
        device_type,
        connection_type,
        identifier,
        analog_channel_list
    );

}

LabJackT7Module::~LabJackT7Module() = default;

#ifdef _WIN32
#define EXPORT_API __declspec(dllexport)
#else
#define EXPORT_API __attribute__((visibility("default")))
#endif

extern "C" EXPORT_API DARTWIC::Modules::BaseModule* createModule(YAML::Node cfg, DARTWIC::API::SDK_API* drtw) {
    return new LabJackT7Module(cfg, drtw);
}
