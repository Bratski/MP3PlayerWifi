#include "rtcdriver.h"

rtcdriver::rtcdriver(const unsigned int &SWITCH, const unsigned int &CLK, const unsigned int &DT)
    : _rotaryCounter(0)
{
    this->_pin1 = SWITCH;
    this->_pin2 = CLK;
    this->_pin3 = DT;
}

rtcdriver::~rtcdriver()
{
    gpiod_line_release(_line1);
    gpiod_line_release(_line2);
    gpiod_line_release(_line3);
    gpiod_chip_close(_chip);
}

bool rtcdriver::initialize()
{
    _chip = gpiod_chip_open_by_name(_chipname);

    if (!_chip)
    {
        std::cerr << "Error opening chip" << std::endl;
        return false;
    }

    _line1 = gpiod_chip_get_line(_chip, _pin1);
    _line2 = gpiod_chip_get_line(_chip, _pin2);
    _line3 = gpiod_chip_get_line(_chip, _pin3);

    if (!_line3 || !_line2 || !_line1)
    {
        std::cerr << "Error getting line" << std::endl;
        gpiod_chip_close(_chip);
        return false;
    }
    // Request lines as inputs with pull-up
    if (gpiod_line_request_input_flags(_line2, "rotary-encoder", GPIOD_LINE_REQUEST_FLAG_BIAS_PULL_UP) < 0 ||
        gpiod_line_request_input_flags(_line3, "rotary-encoder", GPIOD_LINE_REQUEST_FLAG_BIAS_PULL_UP) < 0 ||
        gpiod_line_request_rising_edge_events(_line1, "button-monitor") < 0)
    {
        std::cerr << "Error requesting GPIO lines" << std::endl;
        gpiod_line_release(_line1);
        gpiod_line_release(_line2);
        gpiod_line_release(_line3);
        gpiod_chip_close(_chip);
        return false;
    }

    return true;
}

void rtcdriver::run()
{
    int last_clk_state = gpiod_line_get_value(_line2);
    while (_running)
    {

        // Read switch state
        if (gpiod_line_event_wait(_line1, nullptr) > 0)
        {
            gpiod_line_event event;
            if (gpiod_line_event_read(_line1, &event) == 0)
            {
                _switchState = (event.event_type == GPIOD_LINE_EVENT_RISING_EDGE);
                std::cout << "Button state: " << _switchState << std::endl;
            }
        }

        // Read CLK state
        int current_clk = gpiod_line_get_value(_line2);

        if (current_clk != last_clk_state)
        {
            // CLK state changed, check DT pin to determine direction
            int current_dt = gpiod_line_get_value(_line3);

            if (current_clk == current_dt)
            {
                --_rotaryCounter;
                std::cout << "Rotation: Counter-clockwise | Count: " << _rotaryCounter << std::endl;
            }
            else
            {
                ++_rotaryCounter;
                std::cout << "Rotation: Clockwise | Count: " << _rotaryCounter << std::endl;
            }

            last_clk_state = current_clk;
        }

        // Small delay to reduce CPU usage
        usleep(1000); // 1ms
    }
}

void rtcdriver::setPins(const unsigned int &SWITCH, const unsigned int &CLK, const unsigned int &DT)
{
    this->_pin1 = SWITCH;
    this->_pin2 = CLK;
    this->_pin3 = DT;
}
