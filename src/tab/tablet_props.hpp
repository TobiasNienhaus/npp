//
// Created by Tobias on 12/15/2020.
//

#ifndef NPP_TABLET_PROPS_HPP
#define NPP_TABLET_PROPS_HPP

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define MAGIC_ENUM_RANGE_MIN -63
#include <magic_enum.hpp>

namespace npp::props {

enum class Page { DIGITIZERS, NOT_IMPLEMENTED };

constexpr Page to_page(USHORT usagePage) {
	switch (usagePage) {
	case 0x0D:
		return Page::DIGITIZERS;
	default:
		return Page::NOT_IMPLEMENTED;
	}
}

enum class Usage {
	UNDEFINED,
	RESERVED,
	NOT_IMPLEMENTED,
	DIGITIZER,
	PEN,
	LIGHT_PEN,
	TOUCH_SCREEN,
	TOUCH_PAD,
	WHITEBOARD,
	COORDINATE_MEASURING_MACHINE,
	THREE_D_DIGITIZER,
	STEREO_PLOTTER,
	ARTICULATED_ARM,
	ARMATURE,
	MULTIPLE_POINT_DIGITIZER,
	FREE_SPACE_WAND,
	DEVICE_CONFIGURATION,
	CAPACITIVE_HEAT_MAP_DIGITIZER,
	STYLUS,
	PUCK,
	FINGER,
	DEVICE_SETTINGS,
	CHARACTER_GESTURE,
	TIP_PRESSURE,
	BARREL_PRESSURE,
	IN_RANGE,
	TOUCH,
	UNTOUCH,
	TAP,
	QUALITY,
	DATA_VALID,
	TRANSDUCER_INDEX,
	TABLET_FUNCTION_KEYS,
	PROGRAM_CHANGE_KEYS,
	BATTERY_STRENGTH,
	INVERT,
	X_TILT,
	Y_TILT,
	AZIMUTH,
	ALTITUDE,
	TWIST,
	TIP_SWITCH,
	SECONDARY_TIP_SWITCH,
	BARREL_SWITCH,
	ERASER,
	TABLET_PICK,
	TOUCH_VALID,
	WIDTH,
	HEIGHT,
	CONTACT_IDENTIFIER,
	DEVICE_MODE,
	DEVICE_IDENTIFIER,
	CONTACT_COUNT,
	CONTACT_COUNT_MAXIMUM,
	SCAN_TIME,
	SURFACE_SWITCH,
	BUTTON_SWITCH,
	PAD_TYPE,
	SECONDARY_BARREL_SWITCH,
	TRANSDUCER_SERIAL_NUMBER,
	PREFERRED_COLOR,
	PREFERRED_COLOR_IS_LOCKED,
	PREFERRED_LINE_WIDTH,
	PREFERRED_LINE_WIDTH_IS_LOCKED,
	LATENCY_MODE,
	GESTURE_CHARACTER_QUALITY,
	CHARACTER_GESTURE_DATA_LENGTH,
	CHARACTER_GESTURE_DATA,
	GESTURE_CHARACTER_ENCODING,
	UTF8_CHARACTER_GESTURE_ENCODING,
	UTF16_LITTLE_ENDIAN_CHARACTER_GESTURE_ENCODING,
	UTF16_BIG_ENDIAN_CHARACTER_GESTURE_ENCODING,
	UTF32_LITTLE_ENDIAN_CHARACTER_GESTURE_ENCODING,
	UTF32_BIG_ENDIAN_CHARACTER_GESTURE_ENCODING,
	CAPACITIVE_HEAT_MAP_PROTOCOL_VENDOR_ID,
	CAPACITIVE_HEAT_MAP_PROTOCOL_VERSION,
	CAPACITIVE_HEAT_MAP_FRAME_DATA,
	GESTURE_CHARACTER_ENABLE,
	PREFERRED_LINE_STYLE,
	PREFERRED_LINE_STYLE_IS_LOCKED,
	INK,
	PENCIL,
	HIGHLIGHTER,
	CHISEL_MARKER,
	BRUSH,
	NO_PREFERENCE,
	DIGITIZER_DIAGNOSTIC,
	DIGITIZER_ERROR,
	ERR_NORMAL_STATUS,
	ERR_TRANSDUCERS_EXCEEDED,
	ERR_FULL_TRANS_GESTURES_UNAVAILABLE,
	ERR_CHARGE_LOW,
	TRANSDUCER_SOFTWARE_INFO,
	TRANSDUCER_VENDOR_ID,
	TRANSDUCER_PRODUCT_ID,
	DEVICE_SUPPORTED_PROTOCOLS,
	TRANSDUCER_SUPPORTED_PROTOCOLS,
	NO_PROTOCOL,
	WACOM_AES_PROTOCOL,
	USI_PROTOCOL,
	MICROSOFT_PEN_PROTOCOL,
	SUPPORTED_REPORT_RATES,
	REPORT_RATE,
	TRANSDUCER_CONNECTED,
	SWITCH_DISABLED,
	SWITCH_UNIMPLEMENTED,
	TRANSDUCER_SWITCHES
};

constexpr Usage to_digitizer_usage(USHORT usageId) {
	switch (usageId) {
	case 0x00:
		return Usage::UNDEFINED;
	case 0x01:
		return Usage::DIGITIZER;
	case 0x02:
		return Usage::PEN;
	case 0x03:
		return Usage::LIGHT_PEN;
	case 0x04:
		return Usage::TOUCH_SCREEN;
	case 0x05:
		return Usage::TOUCH_PAD;
	case 0x06:
		return Usage::WHITEBOARD;
	case 0x07:
		return Usage::COORDINATE_MEASURING_MACHINE;
	case 0x08:
		return Usage::THREE_D_DIGITIZER;
	case 0x09:
		return Usage::STEREO_PLOTTER;
	case 0x0A:
		return Usage::ARTICULATED_ARM;
	case 0x0B:
		return Usage::ARMATURE;
	case 0x0C:
		return Usage::MULTIPLE_POINT_DIGITIZER;
	case 0x0D:
		return Usage::FREE_SPACE_WAND;
	case 0x0E:
		return Usage::DEVICE_CONFIGURATION;
	case 0x0F:
		return Usage::CAPACITIVE_HEAT_MAP_DIGITIZER;
	case 0x20:
		return Usage::STYLUS;
	case 0x21:
		return Usage::PUCK;
	case 0x22:
		return Usage::FINGER;
	case 0x23:
		return Usage::DEVICE_SETTINGS;
	case 0x24:
		return Usage::CHARACTER_GESTURE;
	case 0x30:
		return Usage::TIP_PRESSURE;
	case 0x31:
		return Usage::BARREL_PRESSURE;
	case 0x32:
		return Usage::IN_RANGE;
	case 0x33:
		return Usage::TOUCH;
	case 0x34:
		return Usage::UNTOUCH;
	case 0x35:
		return Usage::TAP;
	case 0x36:
		return Usage::QUALITY;
	case 0x37:
		return Usage::DATA_VALID;
	case 0x38:
		return Usage::TRANSDUCER_INDEX;
	case 0x39:
		return Usage::TABLET_FUNCTION_KEYS;
	case 0x3A:
		return Usage::PROGRAM_CHANGE_KEYS;
	case 0x3B:
		return Usage::BATTERY_STRENGTH;
	case 0x3C:
		return Usage::INVERT;
	case 0x3D:
		return Usage::X_TILT;
	case 0x3E:
		return Usage::Y_TILT;
	case 0x3F:
		return Usage::AZIMUTH;
	case 0x40:
		return Usage::ALTITUDE;
	case 0x41:
		return Usage::TWIST;
	case 0x42:
		return Usage::TIP_SWITCH;
	case 0x43:
		return Usage::SECONDARY_TIP_SWITCH;
	case 0x44:
		return Usage::BARREL_SWITCH;
	case 0x45:
		return Usage::ERASER;
	case 0x46:
		return Usage::TABLET_PICK;
	case 0x47:
		return Usage::TOUCH_VALID;
	case 0x48:
		return Usage::WIDTH;
	case 0x49:
		return Usage::HEIGHT;
	case 0x51:
		return Usage::CONTACT_IDENTIFIER;
	case 0x52:
		return Usage::DEVICE_MODE;
	case 0x53:
		return Usage::DEVICE_IDENTIFIER;
	case 0x54:
		return Usage::CONTACT_COUNT;
	case 0x55:
		return Usage::CONTACT_COUNT_MAXIMUM;
	case 0x56:
		return Usage::SCAN_TIME;
	case 0x57:
		return Usage::SURFACE_SWITCH;
	case 0x58:
		return Usage::BUTTON_SWITCH;
	case 0x59:
		return Usage::PAD_TYPE;
	case 0x5A:
		return Usage::SECONDARY_BARREL_SWITCH;
	case 0x5B:
		return Usage::TRANSDUCER_SERIAL_NUMBER;
	case 0x5C:
		return Usage::PREFERRED_COLOR;
	case 0x5D:
		return Usage::PREFERRED_COLOR_IS_LOCKED;
	case 0x5E:
		return Usage::PREFERRED_LINE_WIDTH;
	case 0x5F:
		return Usage::PREFERRED_LINE_WIDTH_IS_LOCKED;
	case 0x60:
		return Usage::LATENCY_MODE;
	case 0x61:
		return Usage::GESTURE_CHARACTER_QUALITY;
	case 0x62:
		return Usage::CHARACTER_GESTURE_DATA_LENGTH;
	case 0x63:
		return Usage::CHARACTER_GESTURE_DATA;
	case 0x64:
		return Usage::GESTURE_CHARACTER_ENCODING;
	case 0x65:
		return Usage::UTF8_CHARACTER_GESTURE_ENCODING;
	case 0x66:
		return Usage::UTF16_LITTLE_ENDIAN_CHARACTER_GESTURE_ENCODING;
	case 0x67:
		return Usage::UTF16_BIG_ENDIAN_CHARACTER_GESTURE_ENCODING;
	case 0x68:
		return Usage::UTF32_LITTLE_ENDIAN_CHARACTER_GESTURE_ENCODING;
	case 0x69:
		return Usage::UTF32_BIG_ENDIAN_CHARACTER_GESTURE_ENCODING;
	case 0x6A:
		return Usage::CAPACITIVE_HEAT_MAP_PROTOCOL_VENDOR_ID;
	case 0x6B:
		return Usage::CAPACITIVE_HEAT_MAP_PROTOCOL_VERSION;
	case 0x6C:
		return Usage::CAPACITIVE_HEAT_MAP_FRAME_DATA;
	case 0x6D:
		return Usage::GESTURE_CHARACTER_ENABLE;
	case 0x70:
		return Usage::PREFERRED_LINE_STYLE;
	case 0x71:
		return Usage::PREFERRED_LINE_STYLE_IS_LOCKED;
	case 0x72:
		return Usage::INK;
	case 0x73:
		return Usage::PENCIL;
	case 0x74:
		return Usage::HIGHLIGHTER;
	case 0x75:
		return Usage::CHISEL_MARKER;
	case 0x76:
		return Usage::BRUSH;
	case 0x77:
		return Usage::NO_PREFERENCE;
	case 0x80:
		return Usage::DIGITIZER_DIAGNOSTIC;
	case 0x81:
		return Usage::DIGITIZER_ERROR;
	case 0x82:
		return Usage::ERR_NORMAL_STATUS;
	case 0x83:
		return Usage::ERR_TRANSDUCERS_EXCEEDED;
	case 0x84:
		return Usage::ERR_FULL_TRANS_GESTURES_UNAVAILABLE;
	case 0x85:
		return Usage::ERR_CHARGE_LOW;
	case 0x90:
		return Usage::TRANSDUCER_SOFTWARE_INFO;
	case 0x91:
		return Usage::TRANSDUCER_VENDOR_ID;
	case 0x92:
		return Usage::TRANSDUCER_PRODUCT_ID;
	case 0x93:
		return Usage::DEVICE_SUPPORTED_PROTOCOLS;
	case 0x94:
		return Usage::TRANSDUCER_SUPPORTED_PROTOCOLS;
	case 0x95:
		return Usage::NO_PROTOCOL;
	case 0x96:
		return Usage::WACOM_AES_PROTOCOL;
	case 0x97:
		return Usage::USI_PROTOCOL;
	case 0x98:
		return Usage::MICROSOFT_PEN_PROTOCOL;
	case 0xA0:
		return Usage::SUPPORTED_REPORT_RATES;
	case 0xA1:
		return Usage::REPORT_RATE;
	case 0xA2:
		return Usage::TRANSDUCER_CONNECTED;
	case 0xA3:
		return Usage::SWITCH_DISABLED;
	case 0xA4:
		return Usage::SWITCH_UNIMPLEMENTED;
	case 0xA5:
		return Usage::TRANSDUCER_SWITCHES;
	default:
		return Usage::RESERVED;
	}
}

constexpr Usage to_usage(USHORT usagePageId, USHORT usageId) {
	switch(to_page(usagePageId)) {
	case Page::DIGITIZERS:
		return to_digitizer_usage(usageId);
	default:
		return Usage::UNDEFINED;
	}
}

constexpr std::string_view usage_as_string(USHORT usagePageId, USHORT usageId) {
	return magic_enum::enum_name<Usage>(to_usage(usagePageId, usageId));
}

} // namespace npp::props

#endif // NPP_TABLET_PROPS_HPP
