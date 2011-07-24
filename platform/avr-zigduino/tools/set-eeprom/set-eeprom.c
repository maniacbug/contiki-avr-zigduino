typedef unsigned int uint8_t __attribute__((__mode__(__QI__)));
uint8_t mac_address[8] __attribute__((section(".eeprom"))) = {0x02, 0x11, 0x22, 0xff, 0xfe, 0x33, 0x44, NODE};
uint8_t rf_channel[2] __attribute__((section(".eeprom"))) = {26, ~26};

