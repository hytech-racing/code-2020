/*
 * MC_modulation_index_flux_weakening_output_information.cpp - CAN message parser: RMS Motor Controller modulation index & flux weakening output information message
 * Created by Ryan Gallaway, December 1, 2016.
 */

#include "HyTech_CAN.h"

MC_modulation_index_flux_weakening_output_information::MC_modulation_index_flux_weakening_output_information() : Abstract_CAN_Container() {};
MC_modulation_index_flux_weakening_output_information::MC_modulation_index_flux_weakening_output_information(uint8_t buf []) : Abstract_CAN_Container(buf) {};

uint16_t MC_modulation_index_flux_weakening_output_information::get_modulation_index() {
    return message.modulation_index;
}

int16_t MC_modulation_index_flux_weakening_output_information::get_flux_weakening_output() {
    return message.flux_weakening_output;
}

int16_t MC_modulation_index_flux_weakening_output_information::get_id_command() {
    return message.id_command;
}

int16_t MC_modulation_index_flux_weakening_output_information::get_iq_command() {
    return message.iq_command;
}
