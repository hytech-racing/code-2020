#pragma once
#include <Metro.h>
#include <HyTech_CAN.h>
#include "HyTech_STL/HT_Vector.h"
#include "HyTech_STL/HT_Map.h"

typedef std::pair<uint32_t, Interface_CAN_Container*> id_can_pair;

class Telemetry {
    public:
        void insert(uint32_t id, Interface_CAN_Container* can, Metro* metro);
        bool load(uint32_t id, uint8_t buf[8]);
        void write(writeFunc callback);
    private:
        HT_Map<uint32_t, Interface_CAN_Container*> messages;
        HT_Map<Metro*, HT_Vector<id_can_pair*>> metros;
};

void Telemetry::insert (uint32_t id, Interface_CAN_Container* can, Metro* metro) {
    id_can_pair* icp = messages.insert(id, can);
    auto metro_it = metros.find(metro);
    if (metro_it != metros.end())
        metro_it->second.push_back(icp);
    else 
        metros.insert(metro, icp);
}

bool Telemetry::load(uint32_t id, uint8_t buf[]) {
    auto it = messages.find(id);
    return it != messages.end() ? it->second->load(buf) : false;
}

void Telemetry::write(writeFunc callback) {
    for (auto it = metros.begin(); it != metros.end(); it++) {
        if (it->first->check()) {
            HT_Vector<id_can_pair*> vec = it->second;
            for (auto icp = vec.begin(); icp != vec.end(); icp++) {
                id_can_pair p = **icp;
                #ifdef HYTECH_COMPRESSION_EN
                    if (p.second->hasChanged())
                        p.second->write(callback, p.first);
                #else
                    p.second->write(callback, p.first);
                #endif
            }
        }
    }
}