#include <LUFA/Drivers/USB/USB.h>
#include <avr/pgmspace.h>
#include <stdint.h>
#include <uhid.h>


typedef struct
{
        USB_Descriptor_Configuration_Header_t Config;

        // Generic HID Interface
        USB_Descriptor_Interface_t            HID_Interface;
        USB_HID_Descriptor_HID_t              HID_GenericHID;
        USB_Descriptor_Endpoint_t             HID_ReportINEndpoint;
} USB_Descriptor_Configuration_t;

enum InterfaceDescriptors_t
{
        INTERFACE_ID_GenericHID = 0, /**< GenericHID interface descriptor ID */
};

enum StringDescriptors_t
{
        STRING_ID_Language     = 0, /**< Supported Languages string descriptor ID (must be zero) */
        STRING_ID_Manufacturer = 1, /**< Manufacturer string ID */
        STRING_ID_Product      = 2, /**< Product string ID */
        STRING_ID_Serial       = 3, /**< Product string ID */
};

        /* Macros: */
                /** Endpoint address of the Generic HID reporting IN endpoint. */
                #define GENERIC_IN_EPADDR         (ENDPOINT_DIR_IN  | 1)

                /** Endpoint address of the Generic HID reporting OUT endpoint. */
                #define GENERIC_OUT_EPADDR        (ENDPOINT_DIR_OUT | 2)

                /** Size in bytes of the Generic HID reporting endpoint. */
                #define GENERIC_EPSIZE            8

const USB_Descriptor_HIDReport_Datatype_t PROGMEM GenericReport[] =
{
        HID_RI_USAGE_PAGE(16, 0xFF00), /* Vendor Page 0 */
        HID_RI_USAGE(8, 0x01), /* Vendor Usage 1 */
        HID_RI_COLLECTION(8, 0x01), /* Vendor Usage 1 */
            HID_RI_USAGE(8, 0x02), /* Vendor Usage 2 */
            HID_RI_LOGICAL_MINIMUM(8, 0x00),
            HID_RI_LOGICAL_MAXIMUM(8, 0xFF),
            HID_RI_REPORT_SIZE(8, 0x08),

            HID_RI_REPORT_ID(8, 0x1),
            HID_RI_REPORT_COUNT(8, IOBUFLEN),
            HID_RI_USAGE(8, 0x00),
            HID_RI_FEATURE(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE | HID_IOF_BUFFERED_BYTES),

            HID_RI_REPORT_ID(8, 0x2),
            HID_RI_REPORT_COUNT(8, IOBUFLEN),
            HID_RI_USAGE(8, 0x00),
            HID_RI_FEATURE(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE | HID_IOF_BUFFERED_BYTES),

            HID_RI_REPORT_ID(8, 0x2),
            HID_RI_REPORT_COUNT(8, IOBUFLEN),
            HID_RI_USAGE(8, 0x00),
            HID_RI_FEATURE(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE | HID_IOF_BUFFERED_BYTES),

        HID_RI_END_COLLECTION(0),
};

/** Device descriptor structure. This descriptor, located in FLASH memory, describes the overall
 *  device characteristics, including the supported USB version, control endpoint size and the
 *  number of device configurations. The descriptor is read out by the USB host when the enumeration
 *  process begins.
 */
const USB_Descriptor_Device_t PROGMEM DeviceDescriptor =
{
        .Header                 = {.Size = sizeof(USB_Descriptor_Device_t), .Type = DTYPE_Device},

        .USBSpecification       = VERSION_BCD(1,1,0),
        .Class                  = USB_CSCP_NoDeviceClass,
        .SubClass               = USB_CSCP_NoDeviceSubclass,
        .Protocol               = USB_CSCP_NoDeviceProtocol,

        .Endpoint0Size          = FIXED_CONTROL_ENDPOINT_SIZE,

        .VendorID               = USB_CFG_VENDOR_ID,
        .ProductID              = USB_CFG_DEVICE_ID,
        .ReleaseNumber          = VERSION_BCD(0,0,1),

        .ManufacturerStrIndex   = STRING_ID_Manufacturer,
        .ProductStrIndex        = STRING_ID_Product,
        .SerialNumStrIndex      = STRING_ID_Serial,

        .NumberOfConfigurations = FIXED_NUM_CONFIGURATIONS
};

/** Configuration descriptor structure. This descriptor, located in FLASH memory, describes the usage
 *  of the device in one of its supported configurations, including information about any device interfaces
 *  and endpoints. The descriptor is read out by the USB host during the enumeration process when selecting
 *  a configuration so that the host may correctly communicate with the USB device.
 */
const USB_Descriptor_Configuration_t PROGMEM ConfigurationDescriptor =
{
        .Config =
                {
                        .Header                 = {.Size = sizeof(USB_Descriptor_Configuration_Header_t), .Type = DTYPE_Configuration},

                        .TotalConfigurationSize = sizeof(USB_Descriptor_Configuration_t),
                        .TotalInterfaces        = 1,

                        .ConfigurationNumber    = 1,
                        .ConfigurationStrIndex  = NO_DESCRIPTOR,

                        .ConfigAttributes       = (USB_CONFIG_ATTR_RESERVED | USB_CONFIG_ATTR_SELFPOWERED),

                        .MaxPowerConsumption    = USB_CONFIG_POWER_MA(100)
                },

        .HID_Interface =
                {
                        .Header                 = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},

                        .InterfaceNumber        = INTERFACE_ID_GenericHID,
                        .AlternateSetting       = 0x00,

                        .TotalEndpoints         = 1,

                        .Class                  = HID_CSCP_HIDClass,
                        .SubClass               = HID_CSCP_NonBootSubclass,
                        .Protocol               = HID_CSCP_NonBootProtocol,

                        .InterfaceStrIndex      = NO_DESCRIPTOR
                },

        .HID_GenericHID =
                {
                        .Header                 = {.Size = sizeof(USB_HID_Descriptor_HID_t), .Type = HID_DTYPE_HID},

                        .HIDSpec                = VERSION_BCD(1,1,1),
                        .CountryCode            = 0x00,
                        .TotalReportDescriptors = 1,
                        .HIDReportType          = HID_DTYPE_Report,
                        .HIDReportLength        = sizeof(GenericReport)
                },

        .HID_ReportINEndpoint =
        		{
        			.Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

        			.EndpointAddress        = GENERIC_IN_EPADDR,
        			.Attributes             = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
        			.EndpointSize           = GENERIC_EPSIZE,
        			.PollingIntervalMS      = 0x05
        		},
};

const USB_Descriptor_String_t PROGMEM LanguageString = USB_STRING_DESCRIPTOR_ARRAY(LANGUAGE_ID_ENG);
const USB_Descriptor_String_t PROGMEM ManufacturerString = USB_STRING_DESCRIPTOR(USB_CFG_VENDOR_NAME);
const USB_Descriptor_String_t PROGMEM ProductString = USB_STRING_DESCRIPTOR(USB_CFG_DEVICE_NAME);
const USB_Descriptor_String_t PROGMEM SerialString = USB_STRING_DESCRIPTOR(USB_CFG_SERIAL_NUMBER);

uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
                                    const uint16_t wIndex,
                                    const void** const DescriptorAddress)
{
        const uint8_t  DescriptorType   = (wValue >> 8);
        const uint8_t  DescriptorNumber = (wValue & 0xFF);

        const void* Address = NULL;
        uint16_t    Size    = NO_DESCRIPTOR;

        switch (DescriptorType)
        {
                case DTYPE_Device:
                        Address = &DeviceDescriptor;
                        Size    = sizeof(USB_Descriptor_Device_t);
                        break;
                case DTYPE_Configuration:
                        Address = &ConfigurationDescriptor;
                        Size    = sizeof(USB_Descriptor_Configuration_t);
                        break;
                case DTYPE_String:
                        switch (DescriptorNumber)
                        {
                                case STRING_ID_Language:
                                        Address = &LanguageString;
                                        Size    = pgm_read_byte(&LanguageString.Header.Size);
                                        break;
                                case STRING_ID_Manufacturer:
                                        Address = &ManufacturerString;
                                        Size    = pgm_read_byte(&ManufacturerString.Header.Size);
                                        break;
                                case STRING_ID_Product:
                                        Address = &ProductString;
                                        Size    = pgm_read_byte(&ProductString.Header.Size);
                                        break;
                                case STRING_ID_Serial:
                                        Address = &SerialString;
                                        Size    = pgm_read_byte(&SerialString.Header.Size);
                                        break;
                        }
                        break;
                case HID_DTYPE_HID:
                        Address = &ConfigurationDescriptor.HID_GenericHID;
                        Size    = sizeof(USB_HID_Descriptor_HID_t);
                        break;
                case HID_DTYPE_Report:
                        Address = &GenericReport;
                        Size    = sizeof(GenericReport);
                        break;
        }

        *DescriptorAddress = Address;
        return Size;
}
