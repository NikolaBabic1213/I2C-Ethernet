#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <stdio.h>

#define PWM_ADDRESS 0x40
#define MODE1_REGISTER 0x00
#define LED0_ON_L 0x06
#define LED0_ON_H 0x07
#define LED0_OFF_L 0x08
#define LED0_OFF_H 0x09

int main()
{
	int fd;
	unsigned char rx_buffer[10];
    	unsigned char reg[] = {MODE1_REGISTER, 0x20};
    	unsigned char data[] = {LED0_ON_L, 0x99, 0x01, 0xcc, 0x04};
    	
	
	// TODO: Initialize array of structures struct i2c_msg to define I2C messages
	struct i2c_msg iomsgs[] = {
	[0] = {
		.addr = PWM_ADDRESS,	/* slave address */
		.flags = 0,		/* write access */
		.buf = &reg[0],	/* register address */
		.len = 2
	},
	
    	[1] = {
		.addr = PWM_ADDRESS,	/* slave address */
		.flags = 0,		/* write access */
		.buf = &data[0],	/* register address */
		.len = 5
	}
	};

	// TODO: Initialize array of structures struct i2c_rdwr_ioctl_data to define I2C transactions
	struct i2c_rdwr_ioctl_data msgset = {
		
		.msgs=&iomsgs[0], 
		.nmsgs=2

	};

	// Try to open I2C device
	fd = open("/dev/i2c-1", O_RDWR);
	
	// Check for any errors
	if (fd < 0)
	{
		printf("Error while trying to open i2c device.\n");
		return -1;
	}
	
	ioctl(fd, I2C_RDWR, &msgset);
	
	
   for (;;)
	{ 
	
	sleep(1);
	
	}

close(fd);
	
	return 0;
}
