//
// Created by kemptonburton on 2/27/2026.
//

#include <labjack_module.h>
#include <labjack_t7_module.h>

int LabJackT7Controller::enable_clk(int handle, const std::string& channel) {
    int pwmFreq = query("digital_channels." + channel + ".pulse_frequency", 0.0);
    int coreFreq = 80000000;
    int clockDivisor = 1;
    int rollValue = coreFreq / (pwmFreq * clockDivisor);

    // Configure Clock Registers:
    LJM_eWriteName(handle, "DIO_EF_CLOCK0_ENABLE", 0); 	// Disable clock source
    // Set Clock0's divisor and roll value to configure frequency: 80MHz/1/80000 = 1kHz
    LJM_eWriteName(handle, "DIO_EF_CLOCK0_DIVISOR", clockDivisor); 	// Configure Clock0's divisor
    LJM_eWriteName(handle, "DIO_EF_CLOCK0_ROLL_VALUE", rollValue); 	// Configure Clock0's roll value
    int result = LJM_eWriteName(handle, "DIO_EF_CLOCK0_ENABLE", 1); 	// Enable the clock source

    if (result != LJME_NOERROR)
    {
        //handle issue
        handle_error(result);

        //return becasue it didnt work
        return -1;
    }

    clk_set = true;

    return 0;
}

void LabJackT7Controller::digital_control_loop_on_start() {
    module_->dartwic->upsertChannelValue("servervariables", "loops."+digital_loop_name_+".target_loop_frequency", 100);

    for (std::string channel : {"DIO0", "DIO2", "DIO3", "DIO4", "DIO5"}) {
        insert("digital_channels." + channel + ".enable_pwm", false);
    }

    // 2 = DIO2, 3 = DIO3
    //encoder stuff
    //RAPID does not support strings as values. I should probably change this...
    insert("digital_channels.encoder.channel_A", 2);
    insert("digital_channels.encoder.channel_B", 3);
}

void LabJackT7Controller::digital_control_loop_on_loop() {
    std::string channel_nums = "8, 9, 10, 11, 12, 13, 14, 15, 17, 18, 19";
    std::vector<std::string> channel_string_nums_to_sync = splitCommaSeparated(channel_nums);

    // DEMO MODE DOES NOT SUPPORT STATE CHECKS FOR DIO
    if (!demoMode) {

        /// DIO OUPUT STATE SYNCYING (DESIRED -> EXPECTED) ///
        for (const std::string channel_num : channel_string_nums_to_sync) {

            std::string channel = "DIO"+channel_num;

            /// SYNC EXPECTED AND DESIRED STATE - from mappings and dio tag
            //dio tag
            double expectedState = query("digital_channels." + channel + ".expected_state", 0.0);
            double desiredState = query("digital_channels." + channel + ".desired_state", 0.0);


            //if any desired state does not match expected state
            if (expectedState != desiredState) {

                int result = LJM_eWriteName(handle, channel.c_str(), desiredState);
                if (result == LJME_NOERROR) {
                    //dio tag
                    upsert("digital_channels." + channel + ".expected_state", desiredState);
                    upsert("digital_channels." + channel + ".desired_state", desiredState);
                } else {
                    // handle error
                    handle_error(result);
                }
            }
        }

        double temp;
        int result = LJM_eReadName(handle, "EIO_CIO_STATE", &temp);
        if (result != LJME_NOERROR) {
            // handle error
            handle_error(result);
            // when push to main - use main error function
        } else {
            for (const std::string channel_num : channel_string_nums_to_sync) {
                int i;
                int first;
                try {
                    i = std::stoi(channel_num);
                    first = std::stoi(channel_string_nums_to_sync[0]);
                } catch (...) {continue;}

                bool actual = static_cast<bool>(static_cast<int>(temp) & (1 << (i-first)));

                //update dio tag actual state
                upsert("digital_channels.DIO" + std::to_string(i) + ".actual_state", actual);

                double expected = query("digital_channels.DIO" + std::to_string(i) + ".expected_state");
                if (expected != actual) {
                    result = LJM_eWriteName(handle, ("DIO" + std::to_string(i)).c_str(), expected);
                    if (result != LJME_NOERROR) {
                        // handle error
                        handle_error(result);
                    }
                }
            }
        }


        /// PWM ///
        //TODO: make a check to see if any tag changes and then reconfigure
        for (std::string channel : {"DIO0", "DIO2", "DIO3", "DIO4", "DIO5"}) {

            /// DETERMINE IF MAPPINGS HAVE CHANGED

            /// IF PWM IS ENABLED
            double enable = query("digital_channels." + channel + ".enable_pwm", 0.0);

            if (enable) {
                double targetAngle = query("digital_channels." + channel + ".target_angle", 0.0);

                int current_target_angle = PWM_DIO_CURRENT_STATE[channel][1];

                int target_angle_to_use = targetAngle; //default target angle

                //dio != current
                if (targetAngle != current_target_angle) {
                    //use dio target angle
                    target_angle_to_use = targetAngle;

                //if none of them have changed
                } else {
                    //use current target angle
                    target_angle_to_use = current_target_angle;
                }

                /// SET ANGLE
                //if enabled for the first time - mapping target angle is used by default
                if (!PWM_DIO_CURRENT_STATE[channel][0]) {

                    //create tags
                    insert("digital_channels." + channel + ".pulse_frequency", 50);
                    insert("digital_channels." + channel + ".max_angle", 180);
                    insert("digital_channels." + channel + ".target_angle", 0);
                    insert("digital_channels." + channel + ".pulse_range.min", 1);
                    insert("digital_channels." + channel + ".pulse_range.max", 2);

                    //if clock is not set already
                    if (!clk_set) {
                        int result = enable_clk(handle, channel);

                        if (result != 0) {
                            //stop
                            break;
                        }
                    }
                    int coreFreq = 80000000;
                    int clockDivisor = 1;
                    double pwmFreq = query("digital_channels." + channel + ".pulse_frequency");
                    double maxAngle = query("digital_channels." + channel + ".max_angle");
                    double minPulse = query("digital_channels." + channel + ".pulse_range.min");
                    double maxPulse = query("digital_channels." + channel + ".pulse_range.max");

                    int rollValue = coreFreq / (pwmFreq * clockDivisor);
                    double targetPulse = ((target_angle_to_use * (maxPulse - minPulse)) / maxAngle) + minPulse; // in ms
                    double period = 1000.0 / pwmFreq; // in ms
                    int configA = static_cast<int>(rollValue * (targetPulse / period));

                    upsert("digital_channels." + channel + ".pulse_target", targetPulse);
                    upsert("digital_channels." + channel + ".configA", configA);

                    // CLEANER - FROM LABJACK - MISSING CIO STOP -----------------------------------
                    // Configure EF Channel Registers:
                    LJM_eWriteName(handle, (channel + "_EF_ENABLE").c_str(), 0); 	// Disable the EF system for initial configuration
                    LJM_eWriteName(handle, (channel + "_EF_INDEX").c_str(), 0); 	// Configure EF system for PWM
                    LJM_eWriteName(handle, (channel + "_EF_OPTIONS").c_str(), 0); 	// Configure what clock source to use: Clock0
                    //LJM_eWriteName(handle, (channel + "_EF_CLOCK_SOURCE").c_str(), 0); 	// Configure what clock source to use: Clock0
                    LJM_eWriteName(handle, (channel + "_EF_CONFIG_A").c_str(), configA); 	// Configure duty cycle to correspond with target angle
                    LJM_eWriteName(handle, (channel + "_EF_ENABLE").c_str(), 1); 	// Enable the EF system, PWM wave is now being outputted

                    PWM_DIO_CURRENT_STATE[channel][0] = true;
                    PWM_DIO_CURRENT_STATE[channel][1] = target_angle_to_use;

                //if target angle has changed
                } else if (current_target_angle != target_angle_to_use) {

                    int coreFreq = 80000000;
                    int clockDivisor = 1;
                    double pwmFreq = query("digital_channels." + channel + ".pulse_frequency");
                    double maxAngle = query("digital_channels." + channel + ".max_angle");
                    double minPulse = query("digital_channels." + channel + ".pulse_range.min");
                    double maxPulse = query("digital_channels." + channel + ".pulse_range.max");
                    PWM_DIO_CURRENT_STATE[channel][1] = target_angle_to_use;

                    int rollValue = coreFreq / (pwmFreq * clockDivisor);
                    double targetPulse = ((target_angle_to_use * (maxPulse - minPulse)) / maxAngle) + minPulse; // in ms
                    double period = 1000.0 / pwmFreq; // in ms
                    int configA = static_cast<int>(rollValue * (targetPulse / period));

                    upsert("digital_channels." + channel + ".pulse_target", targetPulse);
                    upsert("digital_channels." + channel + ".configA", configA);

                    // Reconfigure EF Channel Registers:
                    LJM_eWriteName(handle, (channel + "_EF_CONFIG_A").c_str(), configA); 	// Configure duty cycle to correspond with target angle
                }

            //was just disabled
            } else {

                //delete tags
                remove("digital_channels." + channel + ".pulse_frequency");
                remove("digital_channels." + channel + ".max_angle");
                remove("digital_channels." + channel + ".target_angle");
                remove("digital_channels." + channel + ".pulse_range.min");
                remove("digital_channels." + channel + ".pulse_range.max");
                remove("digital_channels." + channel + ".configA");
                remove("digital_channels." + channel + ".pulse_target");

                //std::string aNames[3] = {"DIO_EF_CLOCK0_ENABLE", channel + "_EF_ENABLE", "CIO0_EF_ENABLE"};
                //int aValues[3] = {0, 0, 0};
                //result = LJM_eWriteNames(handle, 3, aNames, aValues);
                //LJM_eWriteName(handle, "DIO_EF_CLOCK0_ENABLE", 0); 	// Disable clock source
                LJM_eWriteName(handle, (channel + "_EF_ENABLE").c_str(), 0); 	// Disable the EF system
                PWM_DIO_CURRENT_STATE[channel][0] = false;
            }
        }


        /// QUADATURE ENCODER ///
        if (!encoder && query("digital_channels.encoder.enable", 0.0)) {

            //get channels
            std::string channel_a = "DIO" + std::to_string(query("digital_channels.encoder.channel_A"));
            std::string channel_b = "DIO" + std::to_string(query("digital_channels.encoder.channel_B"));

            LJM_eWriteName(handle, (channel_a+"_EF_ENABLE").c_str(), 0); 	// Disable the EF system for initial configuration
            LJM_eWriteName(handle, (channel_b+"_EF_ENABLE").c_str(), 0);
            LJM_eWriteName(handle, (channel_a+"_EF_INDEX").c_str(), 10); 	// Configure EF system
            LJM_eWriteName(handle, (channel_b+"_EF_INDEX").c_str(), 10);
            LJM_eWriteName(handle, (channel_a+"_EF_ENABLE").c_str(), 1); 	// Enable the EF system
            LJM_eWriteName(handle, (channel_b+"_EF_ENABLE").c_str(), 1);
            if (result == LJME_NOERROR) {
                encoder = true;
            } else {
                // handle error
                handle_error(result);
            }
        } else if (encoder && !query("digital_channels.encoder.enable", 0.0)) {

            //get channels
            std::string channel_a = "DIO" + std::to_string(query("digital_channels.encoder.channel_A"));
            std::string channel_b = "DIO" + std::to_string(query("digital_channels.encoder.channel_B"));

            LJM_eWriteName(handle, (channel_a+"_EF_ENABLE").c_str(), 0); 	// Disable the EF system for initial configuration
            LJM_eWriteName(handle, (channel_b+"_EF_ENABLE").c_str(), 0);
            if (result == LJME_NOERROR) {
                encoder = false;
            } else {
                // handle error
                handle_error(result);
            }

        //encoder is enabled - reset count functionality
        } else if (encoder && query("digital_channels.encoder.enable", 0.0)) {

            if (query("digital_channels.encoder.reset_count", 0.0) == 1 ) {
            std::string channel_a = "DIO" + std::to_string(query( "digital_channels.encoder.channel_A"));
                //read the reset register
                auto result = LJM_eReadName(handle, (channel_a+"_EF_READ_A_F_AND_RESET").c_str(), &temp);

                //reset tag to 0
                upsert("digital_channels.encoder.reset_count", 0);
            }

        }
    }
}

void LabJackT7Controller::digital_control_loop_on_end() {

}