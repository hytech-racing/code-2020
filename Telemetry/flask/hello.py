from flask import Flask 
app = Flask(__name__)

from flask import render_template

data = [
    {
        "displayName": "MODULE A TEMP",
        "id": "module-a-temp",
    },

    {
        "displayName": "MODULE B TEMP",
        "id": "module-b-temp",

    },
    {
        "displayName": "MODULE C TEMP",
        "id": "module-c-temp",
        "value": 3 
    },
    {
        "displayName": "GATE DRIVER BOARD TEMP",
        "id": "gate-driver-board-temp"
    },
    {
        "displayName": "RTD 4 TEMP",
        "id": "rtd-4-temp"
    },
    {
        "displayName": "RTD 5 TEMP",
        "id": "rtd-5-temp"
    },
    {
        "displayName": "MOTOR TEMP",
        "id": "motor-temp"
    },
    {
        "displayName": "TORQUE SHUDDER",
        "id": "torque-shudder"
    },
    {
        "displayName": "MOTOR ANGLE",
        "id": "motor-angle"
    },
    {
        "displayNAme": "MOTORSPEED",
        "id": "motor-speed"
    },
    {
        "displayName": "ELEC OUTPUT FREQ",
        "id": "elec-output-freq"
    },
    {
        "displayName": "DELTA RESOLVER FILT",
        "id": "delta-resolver-filt"
    },
    {
        "displayName": "PHASE A CURRENT",
        "id": "phase-a-current"
    },
    {
        "displayName": "PHASE B CURRENT",
        "id": "phase-b-current"
    },
    {
        "displayName": "PHASE C CURRENT",
        "id": "phase-c-current"
    },
    {
        "displayName": "DC BUS CURRENT",
        "id": "dc-bus-current"
    },
    {
        "displayName": "DC BUS VOLTAGE",
        "id": "dc-bus-voltage"
    },
    {
        "displayName": "OUTPUT VOLTAGE",
        "id": "output-voltage"
    },
    {
        "displayName": "PHASE AB VOLTAGE",
        "id": "phase-ab-voltage"
    },
    {
        "displayName": "PHASE BC VOLTAGE",
        "id": "phase-bc-voltage"
    },
    {
        "displayName": "VSM STATE",
        "id": "vsm-state"
    },
    {
        "displayName": "INVERTER STATE",
        "id": "inverter-state"
    },
    {
        "displayName": "DIRECTION COMMAND",
        "id": "direction-command"
    },
    {
        "displayName": "POST FAULT LO",
        "id": "post-fault-lo"
    },
    {
        "displayName": "POST FAULT HI",
        "id": "post-fault-hi"
    },
    {
        "displayName": "RUN FAULT LO",
        "id": "run-fault-lo"
    },
    {
        "displayName": "RUN FAULT HI",
        "id": "run-fault-hi"
    },
    {
        "displayName": "COMMAND TORQUE",
        "id": "command-torque"
    },
    {
        "displayName": "TORQUE FEEDBACK",
        "id": "torque-feedback"
    },
    {
        "displayName": "RMS UPTIME",
        "id": "rms-uptime"
    },
    {
        "displayName": "FCU REQUESTED TORQUE",
        "id": "fcu-requested-torque"
    },
    {
        "displayName": "RCU STATE",
        "id": "rcu-state"
    },
    {
        "displayName": "RCU FLAGS",
        "id": "rcu-flags"
    },
    {
        "displayName": "GLV BATT VOLTAGE",
        "id": "glv-batt-voltage"
    },
    {
        "displayName": "RCU BMS FAULT",
        "id": "rcu-bms-fault"
    },
    {
        "displayName": "RCU IMD FAULT",
        "id": "rcu-imd-fault"
    },
    {
        "displayName": "FCU STATE",
        "id" : "fcu-state"
    },
    {
        "displayName": "FCU FLAGS",
        "id": "fcu-flags"
    },
    {
        "displayName": "FCU START BUTTON ID",
        "id": "fcu-start-button-id"
    },
    {
        "displayName": "FCU BRAKE ACT",
        "id": "fcu-brake-act"
    },
    {
        "displayName": "FCU IMPLAUS ACCEL",
        "id": "fcu-implaus-accel"
    },
    {
        "displayName": "FCU IMPLAUS BRAKE",
        "id": "fcu-implaus-brake"
    },
    {
        "displayName": "FCU PEDAL ACCEL 1",
        "id": "fcu-pedal-accel1"
    },
    {
        "displayName": "FCU PEDAL ACCEL 2",
        "id": "fcu-pedal-accel2"
    },
    {
        "displayName": "FCU PEDAL BRAKE",
        "id": "fcu-pedal-brake"
    },
    {
        "displayName": "BMS VOLTAGE AVERAGE",
        "id": "bms-voltage-average"
    },
    {
        "displayName": "BMS VOLTAGE LOW",
        "id": "bms-voltage-low"
    },
    {
        "displayName": "BMS VOLTAGE HIGH",
        "id": "bms-voltage-high"
    },
    {
        "displayName": "BMS VOLTAGE TOTAL",
        "id": "bms-voltage-total"
    },
    {
        "displayName": "BMS AVERAGE TEMPERATURE",
        "id": "bms-average-temperature"
    },
    {
        "displayName": "BMS LOW TEMPERATURE",
        "id": "bms-low-temperature"
    },
    {
        "displayName": "BMS HIGH TEMPERATURE",
        "id": "bms-high-temperature"
    },
    {
        "displayName": "BMS STATE",
        "id": "bms-state"
    },
    {
        "displayName": "BMS ERROR FLAGS",
        "id": "bms-error-flags"
    },
    {
        "displayName": "BMS CURRENT",
        "id": "bms-current"
    }

]

@app.route('/hello/')
def hello():
    return render_template('hello2.html', data=data)

if __name__ == '__main__':
    app.run(debug=True)
