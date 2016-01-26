#include <linux/input.h>
#include <linux/uinput.h>
#include <linux/types.h>
#include <stdbool.h>

#define EVDEVCAT_UINPUT "/dev/uinput"

int uinput_file;
struct uinput_user_dev uinput_device;

struct event_data {
	__u16 type;
	__u16 code;
	__s32 value;
};

struct input_event system_event;
struct event_data network_event; 

bool ioctl_set_ev(int val);
bool ioctl_set_key(int val);
bool init_system_device(char* device_path);
void copy_network_event();
void send_network_event();
void debug_network_event();
void receive_network_events();
void send_system_events();
int main(int argc, char* argv[]);
