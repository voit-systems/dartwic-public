//
// Created by kemptonburton on 11/14/2025.
//

#ifndef MYMODULE_H
#define MYMODULE_H

#include <iostream>
#include <BaseModule.h>

class MyModule : public DARTWIC::Modules::BaseModule {
public:
    MyModule(YAML::Node cfg, DARTWIC::API::SDK_API* drtw) : BaseModule(cfg, drtw) {
        std::string device_name = getParameter<std::string>("device_name");

        std::cout << device_name << std::endl;

        dartwic->onLoop("plc_module", [&]() {
            double val = dartwic->queryChannelValue("plc_module", "test_tag_1", 10.0);

            std::cout << "test_tag_1: " << val << std::endl;
        });
    };

    ~MyModule() override {
        dartwic->removeLoop("plc_module");
    };

};

#endif //MYMODULE_H

