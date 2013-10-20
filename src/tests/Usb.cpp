/*!
 * \brief  Пример приложения работы с потом USB.
 * \author Rostislav Velichko. rostislav.vel@gmail.com
 * \date   26.1.2013
 */


// g++ -std=c++0x Usb.cpp -lusb


#include <usb.h>
#include <iostream>
#include <string>
#include <cinttypes>


#define VERSION    "0.1.0"
#define VENDOR_ID  0x0925
#define PRODUCT_ID 0x7001
#define INTERFACE  0

const static int reqIntLen        = 2;
const static int endpoint_Int_in  = 0x81; /* endpoint 0x81 address for IN */
const static int endpoint_Int_out = 0x01; /* endpoint 1 address for OUT */
const static int timeout          = 5000; /* timeout in ms */


void bad(const char *why) {
    std::cerr << "Fatal error: " << why << "\n";
}

class Usb {
    typedef struct usb_bus Bus;
    typedef struct usb_device Dev;

    usb_dev_handle *_handle = NULL;

    std::string descriptorTypeName(uint8_t type) {
        switch (type) {
        case USB_DT_DEVICE:    return "LIBUSB_DT_DEVICE";    // Device descriptor.
        case USB_DT_CONFIG:    return "LIBUSB_DT_CONFIG";    // Configuration descriptor.
        case USB_DT_STRING:    return "LIBUSB_DT_STRING";    // String descriptor.
        case USB_DT_INTERFACE: return "LIBUSB_DT_INTERFACE"; // Interface descriptor.
        case USB_DT_ENDPOINT:  return "LIBUSB_DT_ENDPOINT";  // Endpoint descriptor.
        case USB_DT_HID:       return "LIBUSB_DT_HID";       // HID descriptor.
        case USB_DT_REPORT:    return "LIBUSB_DT_REPORT";    // HID report descriptor.
        case USB_DT_PHYSICAL:  return "LIBUSB_DT_PHYSICAL";  // Physical descriptor.
        case USB_DT_HUB:       return "LIBUSB_DT_HUB";       // Hub descriptor.
        }
        return "UNDEFINED";
    }


    std::string deviceClassName(uint8_t type) {
        switch (type) {
        case USB_CLASS_PER_INTERFACE: return "LIBUSB_CLASS_PER_INTERFACE";       // In the context of a device descriptor, this bDeviceClass value indicates that each interface specifies its own class information and all interfaces operate independently.
        case USB_CLASS_AUDIO:         return "LIBUSB_CLASS_AUDIO";               // Audio class.
        case USB_CLASS_COMM:          return "LIBUSB_CLASS_COMM";                // Communications class.
        case USB_CLASS_HID: 	      return "LIBUSB_CLASS_HID";                 // Human Interface Device class.
        case 5:                       return "LIBUSB_CLASS_PHYSICAL";            // Physical.
        case USB_CLASS_PRINTER:       return "LIBUSB_CLASS_PRINTER";             // Printer class.
        case USB_CLASS_PTP: 	      return "LIBUSB_CLASS_PTP";                 // Image class.
        case USB_CLASS_MASS_STORAGE:  return "LIBUSB_CLASS_MASS_STORAGE";        // Mass storage class.
        case USB_CLASS_HUB: 	      return "LIBUSB_CLASS_HUB";                 // Hub class.
        case USB_CLASS_DATA:          return "LIBUSB_CLASS_DATA";                // Data class.
        case 0x0b:                    return "LIBUSB_CLASS_SMART_CARD";          // Smart Card.
        case 0x0d:                    return "LIBUSB_CLASS_CONTENT_SECURITY";    // Content Security.
        case 0x0e:                    return "LIBUSB_CLASS_VIDEO";               // Video.
        case 0x0f:                    return "LIBUSB_CLASS_PERSONAL_HEALTHCARE"; // Personal Healthcare.
        case 0xdc:                    return "LIBUSB_CLASS_DIAGNOSTIC_DEVICE";   // Diagnostic Device.
        case 0xe0:                    return "LIBUSB_CLASS_WIRELESS";            // Wireless class.
        case 0xfe:                    return "LIBUSB_CLASS_APPLICATION";         // Application class.
        case USB_CLASS_VENDOR_SPEC:   return "LIBUSB_CLASS_VENDOR_SPEC";         // Class is vendor-specific.
        }
        return "UNDEFINED";
    }
public:
    Usb() {
        int retval;
        char dname[32] = {0};

#       ifdef DEBUG
        usb_set_debug(255);
#       endif

        usb_init();
        usb_find_busses();
        usb_find_devices();

        for (Bus *bus = usb_get_busses(); bus; bus = bus->next) {
            for (Dev *dev = bus->devices; dev; dev = dev->next) {
                std::cout << std::hex << "USB device: \n"
                    << "Descriptor type: `" << descriptorTypeName(dev->descriptor.bDescriptorType) << "`\n"
                    << "Specification release number in binary-coded decimal: " << dev->descriptor.bcdUSB << "\n"
                    << "Class code for the device: `" << deviceClassName(dev->descriptor.bDeviceClass) << "`\n"
//                (int)bDeviceSubClass
//                    << "USB-IF subclass code for the device, qualified by the bDeviceClass value."
//                (int)bDeviceProtocol
//                    << "USB-IF protocol code for the device, qualified by the bDeviceClass and bDeviceSubClass values."
//                (int)bMaxPacketSize0
//                    << "Maximum packet size for endpoint 0."
//                (int)idVendor
//                    << "USB-IF vendor ID."
//                (int)idProduct
//                    << "USB-IF product ID."
//                (int)bcdDevice
//                    << "Device release number in binary-coded decimal."
//                (int)iManufacturer
//                    << "Index of string descriptor describing manufacturer."
//                (int)iProduct
//                    << "Index of string descriptor describing product."
//                (int)iSerialNumber
//                    << "Index of string descriptor containing device serial number."
//                (int)bNumConfigurations
//                    << "Number of possible configurations."
                    << "\n";
                //_handle = usb_open(dev);
            }
        }
    }
};


void test_interrupt_transfer(usb_dev_handle *dev) {
    int r;
    int i;
    char answer[reqIntLen];
    char question[reqIntLen];

    for (i = 0; i < reqIntLen; i++) {
        question[i]=i;
    }
    r = usb_interrupt_write(dev, endpoint_Int_out, question, reqIntLen, timeout);

    if (r < 0) {
        bad("USB write failed");
        return;
    }
    r = usb_interrupt_read(dev, endpoint_Int_in, answer, reqIntLen, timeout);

    if (r not_eq reqIntLen) {
        bad("USB read failed");
        return;
    }

    for (i = 0; i < reqIntLen; i++) {
        std::cout << question[i] << ", " << answer[i] << "\n";
    }
    //   usb_set_altinterface(dev, 0);
}


int main( int argc, char **argv) {
    Usb usb;

//    if (handle) {
//        //  test_control_transfer(lvr_hid); //not implemented yet
//        test_interrupt_transfer(handle);
//        usb_release_interface(handle, 0);
//        usb_close(handle);

//        return 0;
//    }
    return 1;
}
