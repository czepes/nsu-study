#include <libusb-1.0/libusb.h>
#include <stdio.h>
#include <string>

using namespace std;

const char *get_class_name(uint8_t dev_class) {
  switch (dev_class) {
  case LIBUSB_CLASS_PER_INTERFACE:
    return "Independent Device";
  case LIBUSB_CLASS_AUDIO:
    return "Audio Device";
  case LIBUSB_CLASS_COMM:
    return "Communication Device";
  case LIBUSB_CLASS_HID:
    return "Human Interface Device";
  case LIBUSB_CLASS_PHYSICAL:
    return "Physical Device";
  case LIBUSB_CLASS_IMAGE:
    return "Image Device";
  case LIBUSB_CLASS_PRINTER:
    return "Printer Device";
  case LIBUSB_CLASS_MASS_STORAGE:
    return "Mass storage Device";
  case LIBUSB_CLASS_HUB:
    return "Hub Device";
  case LIBUSB_CLASS_DATA:
    return "Data Device";
  case LIBUSB_CLASS_SMART_CARD:
    return "Smart Card Device";
  case LIBUSB_CLASS_CONTENT_SECURITY:
    return "Content Security Device";
  case LIBUSB_CLASS_VIDEO:
    return "Video Device";
  case LIBUSB_CLASS_PERSONAL_HEALTHCARE:
    return "Personal Healthcare Device";
  case LIBUSB_CLASS_DIAGNOSTIC_DEVICE:
    return "Diagnostic Device Device";
  case LIBUSB_CLASS_WIRELESS:
    return "Wireless Device";
  case LIBUSB_CLASS_MISCELLANEOUS:
    return "Miscellaneous Device";
  case LIBUSB_CLASS_APPLICATION:
    return "Application Device";
  case LIBUSB_CLASS_VENDOR_SPEC:
    return "Vendor-Specific Device";
  default:
    return "Unknown Device";
  }
}

int get_device_svp(libusb_device *dev, libusb_device_descriptor *desc,
                   string *serial, string *vendor, string *product) {
  libusb_device_handle *handle;

  *serial = "No Serial";
  *vendor = "Unknown Vendor";
  *product = "Unknown Product";

  int ret = libusb_open(dev, &handle);
  if (ret != LIBUSB_SUCCESS) {
    return ret;
  }

  unsigned char buffer[256];

  if (desc->iSerialNumber > 0) {
    ret = libusb_get_string_descriptor_ascii(handle, desc->iSerialNumber,
                                             buffer, 256);
    if (ret > 0) {
      *serial = string((char *)buffer);
    }
  }

  if (desc->iManufacturer > 0) {
    ret = libusb_get_string_descriptor_ascii(handle, desc->iManufacturer,
                                             buffer, 256);
    if (ret > 0) {
      *vendor = string((char *)buffer);
    }
  }

  if (desc->iProduct > 0) {
    ret =
        libusb_get_string_descriptor_ascii(handle, desc->iProduct, buffer, 256);
    if (ret > 0) {
      *product = string((char *)buffer);
    }
  }

  libusb_close(handle);
  return LIBUSB_SUCCESS;
}

void printdev(libusb_device *dev, int id = 0) {
  libusb_device_descriptor desc;
  string serial;
  string product;
  string vendor;

  libusb_get_device_descriptor(dev, &desc);
  int ret = get_device_svp(dev, &desc, &serial, &vendor, &product);

  printf("%02d: ID %04x:%04x\n", id, desc.idVendor, desc.idProduct);

  if (ret == LIBUSB_SUCCESS) {
    printf("%02d:    %s | %s\n"
           "%02d:    %s | %s\n",
           id, vendor.c_str(), product.c_str(), id,
           get_class_name(desc.bDeviceClass), serial.c_str());
  }
};

int main(void) {
  libusb_device **devs;
  libusb_context *ctx{NULL};

  int ret = libusb_init(&ctx);
  if (ret < 0) {
    printf("Initialization failed. Error code - %d\n", ret);
    return 1;
  }

  libusb_set_option(ctx, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_INFO);

  ssize_t cnt = libusb_get_device_list(ctx, &devs);
  if (cnt < 0) {
    printf("Failed to retrieve USB devices.");
    return 1;
  }

  printf("No: ID 1234:5678\n"
         "No:    Vendor | Product\n"
         "No:    Class  | Serial\n");

  for (ssize_t i = 0; i < cnt; i++) {
    printdev(devs[i], i + 1);
  }

  libusb_free_device_list(devs, 1);
  libusb_exit(ctx);
  return 0;
}
