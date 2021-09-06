/*
 * @file			ms_os.h
 * @brief			WinUSB specific definitions for enumeration process
 * @author			SteDun00
 * @version			V1.0
 * @review date		2021-02-26
 * @note			reference to https://github.com/dmitrystu/libusb_stm32/blob/winusb/inc/ms_os.h
 * OMICRON electronics GmbH, Austria
 */

#ifndef USB_INC_MS_OS_H_
#define USB_INC_MS_OS_H_

///@brief	macro to create a unicode array
#define STRING_TO_WCHAR(s)		{CAT(u,s)}

///@brief	macro returns total byte length of string in case of unicode array
#define SIZE_OF_WSTRING(s)		sizeof(CAT(u,s))



///@brief	total byte length of a custom property name
///@note 	can be hard-coded at 42 for a 'DeviceInterfaceGUIDs' property name
#define MS_DEVICE_GUID_NAME_BLEN		(SIZE_OF_WSTRING("DeviceInterfaceGUID"))

///@brief	total byte length of a custom property value
///@note 	can be hard-coded at 80 for a 'DeviceInterfaceGUIDs' property name
#define MS_DEVICE_GUID_VALUE_BLEN		(SIZE_OF_WSTRING("{00000000-0000-0000-0000-000000000000}"))



/*
 * WinUSB extended compact ID OS feature descriptor.
 * Is capable to store one or more compatible ID(s) and subCompatible ID(s) in the device firmware
 * on a per-interface or per-function basis. WinUSB then uses these values like class and subclass values to select appropriate driver.
 * */
struct ms_compat_id_descriptor {
	uint32_t dwLength;
	uint16_t bcdVersion;
	uint16_t wIndex;
	uint8_t  bCount;
	uint8_t  _reservedA[7];
	uint8_t  bInterfaceNumber;
	uint8_t	 _reservedB[1];
	uint8_t  chCompatibleID[8];
	uint8_t  chSubCompatibleID[8];
	uint8_t  _reservedC[6];
} __attribute__((packed));


/*
 * WinUSB
 * This \bDeviceInterfaceGUIDs custom property section is used in \ref winusb_devguid_extprop_feature_descriptor.
 * */
struct winusb_devguid_property {
	uint32_t dwPropertyLength;
	uint32_t dwPropertyType;
	uint16_t wPropertyNameLength;
	uint16_t wPropertyName[MS_DEVICE_GUID_NAME_BLEN / 2];
	uint32_t dwPropertyDataLength;
	uint16_t wPropertyData[MS_DEVICE_GUID_VALUE_BLEN / 2];
} __attribute__((packed));


/*
 * WinUSB
 * This \bDeviceInterfaceGUIDs extended property feature descriptor contains a DeviceInterfaceGUIDs property section.
 * Store DeviceInterfaceGUIDs in firmware and use them on Windows to locate and open a device.
 * */
struct winusb_devguid_extprop_feature_descriptor {
	uint32_t dwLength;
	uint16_t bcdVersion;
	uint16_t wIndex;
	uint16_t wCount;
	struct winusb_devguid_property dev_guid_property;
} __attribute__((packed));



#endif /* USB_INC_MS_OS_H_ */
