#include "i2c.h"
#include <util/delay.h>

void i2c_init() {
    TWBR = ((F_CPU / F_SCL) - 16) / 2;
}

/**
 * This function intiates the i2c start signal.
 * @param address - The slave address needed for communication
 * @param mode - The slave mode. Either TWI_READ or TWI_WRITE
 * @return - Returns the status the slave send back to the master device.
 */
uint8_t i2c_start(uint8_t address, uint8_t mode) {
    uint8_t status = 0;
    TWCR = 0;
    // transmit START condition
    TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);

    while(!(TWCR & (1<<TWINT)));

    status = TW_STATUS & 0xF8;
	if ( (status != TW_START) && (status != TW_REP_START)) return status;

    TWDR = (address << 1) | mode;
    TWCR = (1 << TWINT) | (1 << TWEN);

    while(!(TWCR & (1<<TWINT)));
    status = TW_STATUS & 0xF8;
    return status;

}

/**
 * This function writes data on the i2c signal
 * @param data - The data to transfer to the slave
 * @return - Returns the status of the write.
 */
uint8_t i2c_write(uint8_t data) {
    TWDR = data;

    TWCR = (1 << TWINT) | (1 << TWEN);
	uint8_t timeout = 0;
    while(!(TWCR & (1<<TWINT)) && timeout < 100){
		_delay_us(10);
		timeout++;
	};
    return TWSR & 0xF8;

}

/**
 * This function reads a acknowledge after a transmission ended.
 * @return - Returns the data read by after transmission.
 */
uint8_t i2c_read_ack() {
    TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
    while(!(TWCR & (1<<TWINT)));

    return TWDR;
}

/**
 * This functions reads a not-acknowledge after a transmission ended.
 * @return - Returns the data read by after transmission.
 */
uint8_t i2c_read_nack() {
    TWCR = (1<<TWINT) | (1<<TWEN);
    while(!(TWCR & (1<<TWINT)));

    return TWDR;
}

uint8_t i2c_stop() {
    TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
    // wait until stop condition is executed and bus released
    while(TWCR & (1<<TWSTO));
	return 0;
}

//address is aligned. should use 0-512, will multiple by 4
void fram_write(uint16_t address, uint32_t data){
	uint16_t base_address = address * 4;
	uint8_t target_address = (0b1010 << 3) | (base_address >> 8);

	i2c_start(target_address, TW_WRITE);

	i2c_write(base_address % 0x100);
	i2c_write((data >> 24) & 0xFF);
	i2c_write((data >> 16) & 0xFF);
	i2c_write((data >> 8) & 0xFF);
	i2c_write(data & 0xFF);

	i2c_stop();
}

uint32_t fram_read(uint16_t address){
	uint32_t data[4];
	uint16_t base_address = address * 4;
	uint8_t target_address = (0b1010 << 3) | (base_address >> 8);

	i2c_start(target_address, TW_WRITE);
	i2c_write(base_address % 0x100);

	i2c_start(target_address, TW_READ);
	data[0] = i2c_read_ack();
	data[1] = i2c_read_ack();
	data[2] = i2c_read_ack();
	data[3] = i2c_read_nack();
	i2c_stop();

	return (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
}
