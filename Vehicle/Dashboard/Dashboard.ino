#include "Dashboard.h"
#include "DebouncedButton.h"
#include "HyTech_CAN.h"
#include "mcp_can.h"
#include "Metro.h"
#include "VariableLED.h"

// only send if receiving mcu status messages

// LED and buzzer values
bool is_mc_err = false;

// LED Variables
VariableLED variable_led_start(LED_START);
VariableLED variable_led_mode (LED_MODE);

// Button debouncing variables
DebouncedButton debounced_btn_mark;
DebouncedButton debounced_btn_mode;
DebouncedButton debounced_btn_mc_cycle;
DebouncedButton debounced_btn_start;
DebouncedButton debounced_btn_lc;

// CAN Variables
Metro timer_can_update = Metro(100);
MCP_CAN CAN(SPI_CS);

// CAN Messages
Dashboard_status dashboard_status{};
MCU_status mcu_status{};

Metro timer_mcu_heartbeat(0, 1);

inline void led_update();
inline void read_can();
inline void btn_update();
inline void mcu_status_received();

void setup() {
    debounced_btn_mark.begin(BTN_MARK, 100);
    debounced_btn_mode.begin(BTN_MODE, 100);
    debounced_btn_mc_cycle.begin(BTN_MC_CYCLE, 100);
    debounced_btn_start.begin(BTN_START, 100);
    debounced_btn_lc.begin(BTN_LC, 100);

    pinMode(BUZZER,     OUTPUT);
    pinMode(LED_AMS,    OUTPUT);
    pinMode(LED_IMD,    OUTPUT);
    pinMode(LED_MODE,   OUTPUT);
    pinMode(LED_MC_ERR, OUTPUT);
    pinMode(LED_START,  OUTPUT);

    //Initiallizes CAN
    while (CAN_OK != CAN.begin(CAN_500KBPS))              // init can bus : baudrate = 250K
    {
        delay(200);
    }
}

void loop() { 
    read_can();
    led_update();
    btn_update();

    static bool should_send = false;

    if (timer_mcu_heartbeat.check()){
        timer_mcu_heartbeat.interval(0);
        should_send = false;
    }
    else {
        should_send = true;
    }
    
    static uint8_t prev_buttons{}, curr_buttons{}, temp_buttons{};
    prev_buttons = curr_buttons;
    curr_buttons = dashboard_status.get_button_flags();
    temp_buttons = curr_buttons & (curr_buttons ^ prev_buttons);
    static bool prev_start_state;


    //Send CAN message
    //Timer to ensure dashboard isn't flooding data bus, also fires after a button is pressed
    // How does the check for button press work
    // the xor against previous buttons removes the button flags that were sent previously
    // the and enforces that only buttons that are currently pressed are allowed to be sent
    if(should_send && 
        (timer_can_update.check() || (temp_buttons) || (prev_start_state != dashboard_status.get_start_btn()))
      ){
        //create message to send
        uint8_t msg[8] = {0};
        dashboard_status.set_button_flags(temp_buttons);
        dashboard_status.write(msg);
        CAN.sendMsgBuf(ID_DASHBOARD_STATUS, 0, sizeof(dashboard_status), msg);
        //rest update timer
        timer_can_update.reset();
    }
    // clear buttons so they can be retoggled on in the loop
    dashboard_status.set_button_flags(0);
    prev_start_state = dashboard_status.get_start_btn();
}

inline void led_update(){
    variable_led_start.update();
    variable_led_mode.update();
}

inline void btn_update(){
    // this sets the button to be high: it is set low in send can
    if (debounced_btn_mark.isPressed())     { dashboard_status.toggle_mark_btn();     }
    if (debounced_btn_mode.isPressed())     { dashboard_status.toggle_mode_btn();     }
    if (debounced_btn_mc_cycle.isPressed()) { dashboard_status.toggle_mc_cycle_btn(); }
    if (debounced_btn_lc.isPressed())       { dashboard_status.toggle_launch_ctrl_btn();    }

    dashboard_status.set_start_btn(debounced_btn_start.isPressed());
}

inline void read_can(){
    //len is message length, buf is the actual data from the CAN message
    static unsigned char len = 0;
    static unsigned char buf[8] = {0};
    
    while(CAN_MSGAVAIL == CAN.checkReceive()){
        CAN.readMsgBuf(&len, buf);
        static unsigned long canID = {};
        canID = CAN.getCanId();

        switch(canID){
            case ID_MCU_STATUS:
                mcu_status.load(buf);
                timer_mcu_heartbeat.reset();
                timer_mcu_heartbeat.interval(MCU_HEARTBEAT_TIMEOUT);
                mcu_status_received();
                break;

            case ID_MC_FAULT_CODES:
                is_mc_err = false;
                for(int i = 0; i < 8; i++){
                    if(buf[i] != 0){
                        is_mc_err = true;
                        break;
                    }
                }
                //MC Error LED
                digitalWrite(LED_MC_ERR, is_mc_err);
                dashboard_status.set_mc_error_led(is_mc_err);
                break;
        }
    }
}

inline void mcu_status_received(){
    // control buzzer
    digitalWrite(BUZZER, mcu_status.get_activate_buzzer());

    //BMS/AMS LED (bms and ams are the same thing)
    digitalWrite(LED_AMS, !mcu_status.get_bms_ok_high()); //get_bms_ok_high outputs 1 if things are good.  We want light on when things are bad so negate 
    dashboard_status.set_ams_led(!mcu_status.get_bms_ok_high());
    
    //IMD LED
    digitalWrite(LED_IMD, !mcu_status.get_imd_ok_high());//get_imd_okhs_high outputs 1 if things are good.  We want light on when things are bad so negate
    dashboard_status.set_imd_led(!mcu_status.get_imd_ok_high());

    //Start LED
    switch(mcu_status.get_state()){
        case MCU_STATE::TRACTIVE_SYSTEM_NOT_ACTIVE:
            variable_led_start.setMode(BLINK_MODES::OFF);
            dashboard_status.set_start_led(static_cast<uint8_t>(BLINK_MODES::OFF));
            break;
        case MCU_STATE::TRACTIVE_SYSTEM_ACTIVE:
            variable_led_start.setMode(BLINK_MODES::FAST);
            dashboard_status.set_start_led(static_cast<uint8_t>(BLINK_MODES::FAST));
            break;
        case MCU_STATE::ENABLING_INVERTER:
            variable_led_start.setMode(BLINK_MODES::ON);
            dashboard_status.set_start_led(static_cast<uint8_t>(BLINK_MODES::ON));
            break;
    }

    // Mode LED
    switch(mcu_status.get_max_torque()){
        case 100:
            variable_led_mode.setMode(BLINK_MODES::FAST);
            dashboard_status.set_mode_led(static_cast<uint8_t>(BLINK_MODES::FAST));
            break;

        case 120:
            variable_led_mode.setMode(BLINK_MODES::ON);
            dashboard_status.set_mode_led(static_cast<uint8_t>(BLINK_MODES::ON));
            break;
        case 60:
        default:
            variable_led_mode.setMode(BLINK_MODES::OFF);
            dashboard_status.set_mode_led(static_cast<uint8_t>(BLINK_MODES::OFF));
            break;
    }
}
