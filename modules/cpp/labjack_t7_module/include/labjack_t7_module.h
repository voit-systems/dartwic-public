#ifndef LABJACK_T7_MODULE_H
#define LABJACK_T7_MODULE_H

#include <BaseModule.h>
#include <memory>

class LabJackT7Controller;

class LabJackT7Module : public DARTWIC::Modules::BaseModule {
public:
    LabJackT7Module(YAML::Node cfg, DARTWIC::API::SDK_API* drtw);
    ~LabJackT7Module() override;

private:
    std::string device_name_;
    std::unique_ptr<LabJackT7Controller> controller_;
};

#endif
