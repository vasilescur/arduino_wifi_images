#ifndef CAMERA
#define CAMERA
#endif

// Public:
void init_camera();

// Private:
void test_i2c();
void reg_write(uint8_t address, uint8_t value);
uint8_t reg_read(uint8_t address);
