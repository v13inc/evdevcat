#include <string.h>
#include <stdio.h>
#include <linux/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <errno.h>

#include "evdevcat.h"

bool init_system_device(char* device_path) {
  uinput_file = open(device_path, O_WRONLY | O_NDELAY);

  if(uinput_file <= 0) {
    fprintf(stderr, "Error opening uinput device: %s\n", device_path);
    return false;
  }

  ioctl(uinput_file, UI_SET_EVBIT, EV_KEY);
  ioctl(uinput_file, UI_SET_EVBIT, EV_SYN);

  for(int i = 0; i < 256; i++) {
    if(ioctl(uinput_file, UI_SET_KEYBIT, i) < 0) {
      fprintf(stderr, "Error setting KEYBIT %d: %d\n", i, errno);
      return false;
    }
  }

  memset(&uinput_device, 0, sizeof(uinput_device));
  strncpy(uinput_device.name, "evdevcat", UINPUT_MAX_NAME_SIZE);
  uinput_device.id.bustype = BUS_USB;
  uinput_device.id.vendor = 0x1234;
  uinput_device.id.product = 0xfedc;
  uinput_device.id.version = 1;

  write(uinput_file, &uinput_device, sizeof(uinput_device));

  if(ioctl(uinput_file, UI_DEV_CREATE) < 0) {
    fprintf(stderr, "Error creating uinput device: %d\n", errno);
    return false;
  }

  return true;
}

void debug_network_event() {
	fprintf(stderr, "type: %04X\n", network_event.type);
	fprintf(stderr, "code: %04X\n", network_event.code);
	fprintf(stderr, "value: %04X\n\n", network_event.value);
}

void receive_network_events() {
  int bytes_read;
  while(true) {
    bytes_read = read(0, &network_event, sizeof(network_event));
    if(bytes_read <= 0) continue;

    //debug_network_event();

    system_event.type = network_event.type;
    system_event.code = network_event.code;
    system_event.value = network_event.value;

    write(uinput_file, &system_event, sizeof(system_event));
  }
}

void send_system_events() {
  int bytes_read;
  int bytes_written;
  while(true) {
    bytes_read = read(0, &system_event, sizeof(system_event));
    if(bytes_read <= 0) continue;

    network_event.type = system_event.type;
    network_event.code = system_event.code;
    network_event.value = system_event.value;

    //debug_network_event();

    write(1, &network_event, sizeof(network_event));
  }
}

int main(int argc, char* argv[]) {
  if(argc > 1) {
    if(!init_system_device(argv[1])) {
      return 1;
    }

    sleep(1);

    receive_network_events();
  } else {
    send_system_events();
  }

  return 0;
}
