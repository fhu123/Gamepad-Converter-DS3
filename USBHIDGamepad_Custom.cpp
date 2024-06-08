// Copyright 2015-2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include "USBHID.h"

#define CONFIG_TINYUSB_HID_ENABLED 1
#if CONFIG_TINYUSB_HID_ENABLED

#include "USBHIDGamepad_Custom.h"

// Gamepad Report Descriptor Template
// with 32 buttons, 2 joysticks and 1 hat/dpad with following layout
// | X | Y | Z | Rz | Rx | Ry (1 byte each) | hat/DPAD (1 byte) | Button Map (4 bytes) |
#define CUSTOM_TUD_HID_REPORT_DESC_GAMEPAD(...)                                                          \
    HID_USAGE_PAGE(HID_USAGE_PAGE_DESKTOP),                                                              \
        HID_USAGE(HID_USAGE_DESKTOP_GAMEPAD),                                                            \
        HID_COLLECTION(HID_COLLECTION_APPLICATION), /* Report ID if any */                               \
        __VA_ARGS__                                 /* 8 bit X, Y, Z, Rz, Rx, Ry (min -127, max 127 ) */ \
        HID_USAGE_PAGE(HID_USAGE_PAGE_DESKTOP),                                                          \
        HID_USAGE(HID_USAGE_DESKTOP_X),                                                                  \
        HID_USAGE(HID_USAGE_DESKTOP_Y),                                                                  \
        HID_USAGE(HID_USAGE_DESKTOP_Z),                                                                  \
        HID_USAGE(HID_USAGE_DESKTOP_RZ),                                                                 \
        HID_USAGE(HID_USAGE_DESKTOP_RX),                                                                 \
        HID_USAGE(HID_USAGE_DESKTOP_RY),                                                                 \
        HID_USAGE(HID_USAGE_DESKTOP_SLIDER),                                                             \
        HID_USAGE(HID_USAGE_DESKTOP_SLIDER),                                                             \
        HID_USAGE(HID_USAGE_DESKTOP_SLIDER),                                                             \
        HID_USAGE(HID_USAGE_DESKTOP_SLIDER),                                                             \
        HID_LOGICAL_MIN(0x81),                                                                           \
        HID_LOGICAL_MAX(0x7f),                                                                           \
        HID_REPORT_COUNT(10),                                                                            \
        HID_REPORT_SIZE(8),                                                                              \
        HID_INPUT(HID_DATA | HID_VARIABLE | HID_ABSOLUTE), /* 8 bit DPad/Hat Button Map  */              \
        HID_USAGE_PAGE(HID_USAGE_PAGE_DESKTOP),                                                          \
        HID_USAGE(HID_USAGE_DESKTOP_HAT_SWITCH),                                                         \
        HID_LOGICAL_MIN(1),                                                                              \
        HID_LOGICAL_MAX(8),                                                                              \
        HID_PHYSICAL_MIN(0),                                                                             \
        HID_PHYSICAL_MAX_N(315, 2),                                                                      \
        HID_REPORT_COUNT(1),                                                                             \
        HID_REPORT_SIZE(8),                                                                              \
        HID_INPUT(HID_DATA | HID_VARIABLE | HID_ABSOLUTE), /* 32 bit Button Map */                       \
        HID_USAGE_PAGE(HID_USAGE_PAGE_BUTTON),                                                           \
        HID_USAGE_MIN(1),                                                                                \
        HID_USAGE_MAX(64),                                                                               \
        HID_LOGICAL_MIN(0),                                                                              \
        HID_LOGICAL_MAX(1),                                                                              \
        HID_REPORT_COUNT(64),                                                                            \
        HID_REPORT_SIZE(1),                                                                              \
        HID_INPUT(HID_DATA | HID_VARIABLE | HID_ABSOLUTE),                                               \
        HID_COLLECTION_END

// HID Generic Input & Output
// - 1st parameter is report size (mandatory)
// - 2nd parameter is report id HID_REPORT_ID(n) (optional)
#define CUSTOM_TUD_HID_REPORT_DESC_GENERIC_INOUT(report_size, ...)         \
    HID_USAGE_PAGE_N(HID_USAGE_PAGE_VENDOR, 2),                            \
        HID_USAGE(0x01),                                                   \
        HID_COLLECTION(HID_COLLECTION_APPLICATION), /* Report ID if any */ \
        __VA_ARGS__                                 /* Input */            \
            HID_USAGE(0x02),                                               \
        HID_LOGICAL_MIN(0x00),                                             \
        HID_LOGICAL_MAX_N(0xff, 2),                                        \
        HID_REPORT_SIZE(8),                                                \
        HID_REPORT_COUNT(report_size),                                     \
        HID_INPUT(HID_DATA | HID_VARIABLE | HID_ABSOLUTE), /* Output */    \
        HID_USAGE(0x03),                                                   \
        HID_LOGICAL_MIN(0x00),                                             \
        HID_LOGICAL_MAX_N(0xff, 2),                                        \
        HID_REPORT_SIZE(8),                                                \
        HID_REPORT_COUNT(report_size),                                     \
        HID_OUTPUT(HID_DATA | HID_VARIABLE | HID_ABSOLUTE),                \
        HID_COLLECTION_END

static const uint8_t report_descriptor[] = {
    CUSTOM_TUD_HID_REPORT_DESC_GAMEPAD(HID_REPORT_ID(HID_REPORT_ID_GAMEPAD))};

USBHIDGamepad_Custom::USBHIDGamepad_Custom() : hid(), _x(0), _y(0), _z(0),
                                               _rz(0), _rx(0), _ry(0),
                                               _slider0(0), _slider1(0),
                                               _hat(0), _buttons(0)
{
    static bool initialized = false;
    if (!initialized)
    {
        initialized = true;
        hid.addDevice(this, sizeof(report_descriptor));
    }
}

uint16_t USBHIDGamepad_Custom::_onGetDescriptor(uint8_t *dst)
{
    memcpy(dst, report_descriptor, sizeof(report_descriptor));
    return sizeof(report_descriptor);
}

void USBHIDGamepad_Custom::begin()
{
    hid.begin();
}

void USBHIDGamepad_Custom::end()
{
}

bool USBHIDGamepad_Custom::write()
{
    custom_hid_gamepad_report_t report = {
        .x = _x,
        .y = _y,
        .z = _z,
        .rz = _rz,
        .rx = _rx,
        .ry = _ry,
        .slider0 = _slider0,
        .slider1 = _slider1,
        .slider2 = _slider2,
        .slider3 = _slider3,
        .hat = _hat,
        .buttons = _buttons};
    return hid.SendReport(HID_REPORT_ID_GAMEPAD, &report, sizeof(report));
}

bool USBHIDGamepad_Custom::leftStick(int8_t x, int8_t y)
{
    _x = x;
    _y = y;
    return write();
}

bool USBHIDGamepad_Custom::rightStick(int8_t z, int8_t rz)
{
    _z = z;
    _rz = rz;
    return write();
}

bool USBHIDGamepad_Custom::leftTrigger(int8_t rx)
{
    _rx = rx;
    return write();
}

bool USBHIDGamepad_Custom::rightTrigger(int8_t ry)
{
    _ry = ry;
    return write();
}

bool USBHIDGamepad_Custom::hat(uint8_t hat)
{
    if (hat > 9)
    {
        return false;
    }
    _hat = hat;
    return write();
}

bool USBHIDGamepad_Custom::pressButton(uint8_t button)
{
    if (button > 63)
    {
        return false;
    }
    _buttons |= (1ULL << button);
    return write();
}

bool USBHIDGamepad_Custom::releaseButton(uint8_t button)
{
    if (button > 63)
    {
        return false;
    }
    _buttons &= ~(1ULL << button);
    return write();
}

bool USBHIDGamepad_Custom::send(int8_t x, int8_t y, int8_t z,
                                int8_t rz, int8_t rx, int8_t ry,
                                int8_t s0, int8_t s1, int8_t s2, int8_t s3,
                                uint8_t hat, uint64_t buttons)
{

    _x = x;
    _y = y;
    _z = z;
    _rz = rz;
    _rx = rx;
    _ry = ry;
    _slider0 = s0;
    _slider1 = s1;
    _slider2 = s2;
    _slider3 = s3;
    if (hat < 9)
    {
        _hat = hat;
    }
    _buttons = buttons;
    return write();
}

#endif /* CONFIG_TINYUSB_HID_ENABLED */
