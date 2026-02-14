//
// Created by kemptonburton on 11/16/2025.
//

#ifndef MODBUS_TCP_CLIENT_MODULE_H
#define MODBUS_TCP_CLIENT_MODULE_H

#include <BaseModule.h>
#include <iostream>
#include <modbus_tcp_client.h>

class ModbusTCPClientModule : public DARTWIC::Modules::BaseModule {
public:
    ModbusTCPClientModule(YAML::Node cfg, DARTWIC::API::SDK_API* drtw) : BaseModule(cfg, drtw),
    client_(createClient()){};

    ModbusTCPClient createClient() {
        std::string device_name = getParameter<std::string>("device_name");
        std::string serverip = getParameter<std::string>("server_ip");
        int server_port = getParameter<int>("server_port");
        int num_coils = getParameter<int>("num_coils");
        int num_discrete_inputs = getParameter<int>("num_discrete_inputs");
        int num_holding_registers = getParameter<int>("num_holding_registers");
        int num_input_registers = getParameter<int>("num_input_registers");
        int tv_sec = getParameter<int>("tv_sec");
        int tv_usec = getParameter<int>("tv_usec");


        return ModbusTCPClient(this, device_name, serverip, server_port,
            num_discrete_inputs, num_input_registers, num_coils, num_holding_registers,
            tv_sec, tv_usec);
    }

    ~ModbusTCPClientModule() override {
     };

private:
    ModbusTCPClient client_;
};

#endif //MODBUS_TCP_CLIENT_MODULE_H
