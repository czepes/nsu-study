#include <libusb-1.0/libusb.h>
#include <stdio.h>

void printdev(libusb_device *dev) {
  libusb_device_descriptor desc;
  libusb_config_descriptor *config;
  const libusb_interface *interface;
  const libusb_interface_descriptor *interdesc;
  const libusb_endpoint_descriptor *epdesc;

  libusb_get_device_descriptor(dev, &desc);
  libusb_get_config_descriptor(dev, 0, &config);

  printf("%.3d %.3d %.4x %.4x %.3d |   |   |  |  |  |\n",
         (int)desc.bNumConfigurations, (int)desc.bDeviceClass, desc.idVendor,
         desc.idProduct, (int)config->bNumInterfaces);

  for (int i = 0; i < config->bNumInterfaces; i++) {
    interface = &config->interface[i];
    printf("|   |   |    |    |   %.3d %.3d |  |  |  |\n",
           interface->num_altsetting, (int)desc.bDeviceClass);

    for (int j = 0; j < interface->num_altsetting; j++) {
      interdesc = &interface->altsetting[j];
      printf("|   |   |    |    |   |   |   %.2d %.2d |  |\n",
             (int)interdesc->bInterfaceNumber, (int)interdesc->bNumEndpoints);

      for (int k = 0; k < (int)interdesc->bNumEndpoints; k++) {
        epdesc = &interdesc->endpoint[k];
        printf("|   |   |    |    |   |   |   |  |  %.2d %.9d\n",
               (int)epdesc->bDescriptorType, (int)epdesc->bEndpointAddress);
      }
    }
  }

  libusb_free_config_descriptor(config);
};

int main(void) {
  libusb_device **devs;
  libusb_context *ctx{NULL};

  int code = libusb_init(&ctx);
  if (code < 0) {
    printf("Initialization failed. Error code - %d\n", code);
    return 1;
  }

  libusb_set_option(ctx, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_INFO);

  ssize_t cnt = libusb_get_device_list(ctx, &devs);
  if (cnt < 0) {
    printf("Failed to retrieve USB devices.");
    return 1;
  }

  printf("Devices found: %d\n", (int)cnt);
  printf("==============================================================\n");
  printf("* amount of possible configurations\n");
  printf("|   * device class\n");
  printf("|   |   * vendor id\n");
  printf("|   |   |    * device id\n");
  printf("|   |   |    |    * interface amount\n");
  printf("|   |   |    |    |   * amount of alternative options\n");
  printf("|   |   |    |    |   |   * device class\n");
  printf("|   |   |    |    |   |   |   * interface number\n");
  printf("|   |   |    |    |   |   |   |  * amount of endpoints\n");
  printf("|   |   |    |    |   |   |   |  |  * descriptor type\n");
  printf("|   |   |    |    |   |   |   |  |  |  * endpoint address\n");
  printf("+---+---+----+----+---+---+---+--+--+--+----------------------\n");

  for (ssize_t i = 0; i < cnt; i++) {
    printdev(devs[i]);
  }

  libusb_free_device_list(devs, 1);
  libusb_exit(ctx);

  return 0;
}
