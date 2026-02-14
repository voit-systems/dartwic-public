//
// Created by kemptonburton on 11/16/2025.
//

#include "modbus_tcp_client_module.h"

#ifdef _WIN32
    #define EXPORT_API __declspec(dllexport)
#else
    #define EXPORT_API __attribute__((visibility("default")))
#endif

extern "C" EXPORT_API DARTWIC::Modules::BaseModule* createModule(YAML::Node cfg, DARTWIC::API::SDK_API* drtw) {
    return new ModbusTCPClientModule(cfg, drtw);
}