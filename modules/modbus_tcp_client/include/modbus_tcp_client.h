//
// Created by kemptonburton on 11/16/2025.
//

#ifndef MODBUS_TCP_CLIENT_H
#define MODBUS_TCP_CLIENT_H

#include <modbus.h>
#include <mutex>
#include <string>
#include <vector>

class ModbusTCPClientModule;

class ModbusTCPClient {
public:

    ModbusTCPClient(ModbusTCPClientModule* module, std::string device_name, std::string server_ip, int server_port,
                                     int num_discrete_inputs, int num_input_registers, int num_coils, int num_holding_registers, uint32_t tv_sec, uint32_t tv_usec);

    // Destructor
    ~ModbusTCPClient();

    // Connect to the Modbus server
    bool connect();

    void on_error(const char* error);

    void disconnect();

    void reconnect();

    // Query and display discrete inputs
    void query_discrete_inputs();

    // Query and display input registers
    void query_input_registers();

    // Read and display coil values
    void read_coils();

    // Synchronize coil values to the desired state
    void sync_coils();

    uint16_t read_holding_register(int address);
    int write_holding_register(int address, uint16_t value);

    std::string device_name_;   // Device name


private:

    /// MODULE ///
    ModbusTCPClientModule* module_;

    /// SERVER INITIALIZERS
    std::string server_ip_;     // Server IP address
    int server_port_{};           // Server port (default 502)
    modbus_t* ctx_{};             // Modbus context
    std::mutex ctx_lock_;   //mutex lock for the modbus object
    uint32_t tv_sec_;  //timeout seconds
    uint32_t tv_usec_; //timeout milliseconds

    const uint8_t num_discrete_inputs_; //read only - 1 bit (DIO's)
    const uint16_t num_input_registers_; //read only - 16 bit (AIN's)
    const uint8_t num_coils_; //read-write - 1 bit (RELAYS)
    const uint16_t num_holding_registers_; //read-write - 16 bit (DEVICE VARIABLES)

    /// VALUES
    //read only
    std::vector<uint8_t> discrete_inputs_;
    std::vector<uint16_t> input_registers_;

    //coils - read/write
    std::vector<uint8_t> desired_coil_states_;
    std::vector<uint8_t> actual_coil_states_;

    //holding register - read/write
    std::vector<uint16_t> holding_registers_;

    /// MAPPINGS / OTHER SETTINGS
    //read only mappings
    std::vector<std::string> input_register_mapping_tag_names_;

};

#endif //MODBUS_TCP_CLIENT_H
