//
// Created by kemptonburton on 11/16/2025.
//

#include "modbus_tcp_client.h"
#include <modbus_tcp_client_module.h>

#include <iostream>
#include <sstream>

class ModbusTCPClientModule;

ModbusTCPClient::ModbusTCPClient(ModbusTCPClientModule* module, const std::string device_name, const std::string server_ip, int server_port,
                                 int num_discrete_inputs, int num_input_registers, int num_coils,
                                 int num_holding_registers, uint32_t tv_sec, uint32_t tv_usec)
    : server_ip_(server_ip),
      server_port_(server_port), ctx_(nullptr),
      device_name_(device_name),
      num_discrete_inputs_(num_discrete_inputs),
      num_input_registers_(num_input_registers),
      input_register_mapping_tag_names_(num_input_registers),
      num_coils_(num_coils),
      num_holding_registers_(num_holding_registers), discrete_inputs_(num_discrete_inputs),
      input_registers_(num_input_registers),
      actual_coil_states_(num_coils),
      desired_coil_states_(num_coils),
      holding_registers_(num_holding_registers),
      tv_sec_(tv_sec),
      module_(module),
      tv_usec_(tv_usec) {
    //INIT TAGS - if in constructor, this should always be 0
    module_->dartwic->upsertChannelValue(device_name, "info.connected", 0);

    //DESIRED COIL STATES - only set them if they do not already exist
    for (int i = 0; i < num_coils_; ++i) {
        module_->dartwic->insertChannelValue(device_name_, "coils." + std::to_string(i) + ".desired_state", 0);
    }

    std::string loop_name = "modbus_tcp_client-" + device_name;

    module_->dartwic->insertChannelValue("servervariables", "loops."+loop_name+".target_loop_frequency", 100);

    //re-create the client with new parameters, just incase the user changes them
    module_->dartwic->onStart(loop_name, [=]() {
        //SET TAGS
        //RAPID does not support values a strings, should I change this???
        // upsertTag(currentProject+":"+device_name+"/info.server_address.value", server_ip);
        // upsert<VALUE>(device_name,"info.server_address", server_ip);
        // upsertTag(currentProject+":"+device_name+"/info.server_port.value", port);
    });
    module_->dartwic->onLoop(loop_name, [=]() {

        double connected = module_->dartwic->queryChannelValue(device_name, "info.connected", 0.0);

        /// IF NOT CONNECTED ///
        if (connected == 0) {
            connect(); //connect
        ///IF CONNECTED
        } else {
            //verify connection
            read_holding_register(0);

            /// QUERY DISCRETE INPUTS ///
            query_discrete_inputs();

            /// QUERY INPUT REGISTERS ///
            query_input_registers();

            /// COILS ///
            read_coils(); // READ COIL STATES
            sync_coils(); // SEND DESIRED COIL STATES
        }
    });
    module_->dartwic->onEnd(loop_name, [=]() {
        //nothing
    });
}

ModbusTCPClient::~ModbusTCPClient() {
    std::cout << "destructor in tcp client" << std::endl;

    disconnect();

    // remove loop
    std::string loop_name = "modbus_tcp_client-" + device_name_;

    module_->dartwic->removeLoop(loop_name);

    // delete used channels
    for (int i = 0; i < num_discrete_inputs_; ++i) {
        module_->dartwic->removeChannel(device_name_, "discrete_inputs." + std::to_string(i));
    }
    for (int i = 0; i < num_input_registers_; ++i) {
        module_->dartwic->removeChannel(device_name_, "input_registers." + std::to_string(i));
    }
    for (int i = 0; i < num_coils_; ++i) {
        module_->dartwic->removeChannel(device_name_, "coils."+ std::to_string(i) + ".actual_state");
        module_->dartwic->removeChannel(device_name_, "coils."+ std::to_string(i) + ".desired_state");
    }
    module_->dartwic->removeChannel(device_name_, "coils.sync_desired_states");
    module_->dartwic->removeChannel(device_name_, "info.connected");
}

bool ModbusTCPClient::connect() {
    ctx_ = modbus_new_tcp(server_ip_.c_str(), server_port_);

    //ERRORS
    if (ctx_ == nullptr) {
        std::cerr << "Unable to create Modbus TCP context." << std::endl;

        module_->dartwic->upsertChannelValue(device_name_, "info.connected", 0);

        return false;
    }

    if (modbus_connect(ctx_) == -1) {

        std::string title = "MODBUS TCP Client Connection Error [" + device_name_ + "]";
        std::string tag = device_name_ + "/info.connected.value";
        std::string description = "The MODBUS TCP client was unable to connect to the MODBUS server on the given IP and Port."
                                  " \n[Device Name: " + device_name_ +
                                    "]  \n[MODBUS Error: " + modbus_strerror(errno) + "]";

        module_->dartwic->consoleError(title,
            description,
            {tag},
            "Ensure the configured IP and Port is correct, both on the server and client device. "
            "Ensure any wire connections are not broken, and that the network router and device the MODBUS "
            "client is trying to connect to has power.",
            5
        );

        modbus_free(ctx_);
        ctx_ = nullptr;

        module_->dartwic->upsertChannelValue(device_name_, "info.connected", 0);

        return false;
    }

    //CONNECTED
    module_->dartwic->upsertChannelValue(device_name_, "info.connected", 1);

    //SET TIMEOUT
    modbus_set_response_timeout(ctx_, tv_sec_, tv_usec_);

    std::ostringstream  stream;
    stream << "Connected to Modbus TCP server at " << server_ip_ << ":" << server_port_ << " device name:" <<
            device_name_ << std::endl;
    std::cout << stream.str() << std::endl;

    return true;
}

void ModbusTCPClient::on_error(const char *error) {
    //print error
    std::cerr << "Error: " << error << std::endl;

    //reconnect automatically sets status to disconnected
    reconnect();
}

void ModbusTCPClient::disconnect() {
    if (ctx_ != nullptr) {
        modbus_close(ctx_);
        modbus_free(ctx_);
        ctx_ = nullptr;
    }
    module_->dartwic->upsertChannelValue(device_name_, "info.connected", 0);
}

void ModbusTCPClient::reconnect() {
    disconnect();
    connect();
}

void ModbusTCPClient::query_discrete_inputs() {
    if (num_discrete_inputs_ > 0) {
        int rc = modbus_read_input_bits(ctx_, 0, num_discrete_inputs_, discrete_inputs_.data());
        //FAIL
        if (rc == -1) {
            on_error(modbus_strerror(errno));
            return;
        }

        //SUCCESS
        for (int i = 0; i < rc; ++i) {
            module_->dartwic->upsertChannelValue(device_name_, "discrete_inputs." + std::to_string(i), discrete_inputs_[i]);
        }
    }
}

void ModbusTCPClient::query_input_registers() {
    if (num_input_registers_ > 0) {
        int rc = modbus_read_input_registers(ctx_, 0, num_input_registers_, input_registers_.data());
        //FAIL
        if (rc == -1) {
            on_error(modbus_strerror(errno));
            return;
        }

        //SUCCESS
        for (int i = 0; i < rc; ++i) {
            //cast from unsigned int to signed int
            int s = static_cast<int16_t>(input_registers_[i]);

            //input register
            module_->dartwic->upsertChannelValue(device_name_, "input_registers." + std::to_string(i), s);
        }
    }
}

void ModbusTCPClient::read_coils() {
    if (num_coils_ > 0) {
        int rc = modbus_read_bits(ctx_, 0, num_coils_, actual_coil_states_.data());
        //FAIL
        if (rc == -1) {
            on_error(modbus_strerror(errno));
            return;
        }

        //SUCCESS
        for (int i = 0; i < rc; ++i) {
            module_->dartwic->upsertChannelValue(device_name_, "coils."+ std::to_string(i) + ".actual_state", actual_coil_states_[i]);
        }
    }
}

void ModbusTCPClient::sync_coils() {
    //IF WE DONT WANT TO SYNC THE COILS
    int sync = module_->dartwic->queryChannelValue(device_name_, "coils.sync_desired_states", 1.0);
    if (!sync) return; //return

    //GET DESIRED COIL STATES
    if (num_coils_ > 0) {
        for (int i = 0; i < num_coils_; ++i) {
            //default to 0 if not found
            int state = module_->dartwic->queryChannelValue(device_name_, "coils." + std::to_string(i)+ ".desired_state", 0.0);
            desired_coil_states_[i] = state;
        }

        //SEND TO SERVER
        std::vector<uint8_t> coils(desired_coil_states_.begin(), desired_coil_states_.end());
        int start_address = 0;
        int num_coils = coils.size();

        //WRITE COILS
        int rc = modbus_write_bits(ctx_, start_address, num_coils, coils.data());

        //FAIL
        if (rc == -1) {
            on_error(modbus_strerror(errno));
        }
    }
}


uint16_t ModbusTCPClient::read_holding_register(int address) {
    if (this == nullptr or ctx_ == nullptr) return 0;

    // Read one holding register
    uint16_t tempValue = 5;
    int rc = modbus_read_registers(ctx_, address, 1, &tempValue);

    if (rc == -1) {
        on_error(modbus_strerror(errno));
        return -1;
    }

    return tempValue;
}

int ModbusTCPClient::write_holding_register(int address, uint16_t value) {
    // write one holding register

    if (this == nullptr or ctx_ == nullptr) return 0;


    int rc = modbus_write_register(ctx_, address, value);
    if (rc == -1) {
        on_error(modbus_strerror(errno));
        return -1;
    } else {
        return 0;
    }
}
