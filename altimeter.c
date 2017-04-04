#include <linux/i2c-dev.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/stat.h>


int file;
int adapter_nr= 2;
char filename[20];

uint8_t accelAddr = 0x19;
uint8_t magAddr = 0x1E;
uint8_t gyroAddr = 0x6B;
uint8_t altAddr = 0x77;

uint16_t altCalVals[11]

uint8_t acceleration[6];
uint8_t gyroscope[6];
uint8_t magnetic[6];


void initAccel();
void readAccel(uint8_t* data);
void initMag();
void readMag(uint8_t* data);
void initGyro();
void readGyro(uint8_t* data);
void initAltimeter();


int main()
{

	printf(filename, 19, "/dev/i2c-1", adapter_nr);
	file = open(filename, O_RDWR);

	if(file <0)
	{
		exit(1);
	}
	initAccel();
}

void initAccel()
{

	if(ioctl(file, I2C_SLAVE, accelAddr)<0)
	{
		exit(1);
	}

	//set register 0x20 to 0x77
	i2c_smbus_write_word_data(file, 0x20, 0x77); //enable accelerometer reading
}

void readAccel(uint8_t* data)
{
	if(ioctl(file, I2C_SLAVE, accelAddr)<0)
	{
		exit(1);
	}

	uint8_t readAddr = 0x28;

	int i = 0;
	for(i = 0; i < 6; i++)
	{
		data[i] = i2c_smbus_read_byte_data(file, (readAddr+i));
	}
}

void initMag()

{
	if(ioctl(file, I2C_SLAVE, magAddr)<0)
	{
		exit(1);
	}

	//set register 0x2 to 0x00
	//enable magnetometer to continuous conversion mode
	i2c_smbus_write_word_data(file, 0x02, 0x00);
}

void readMag(uint8_t* data)
{
	if(ioctl(file, I2C_SLAVE, magAddr)<0)
	{
		exit(1);
	}

	uint8_t readAddr = 0x03;

	int i = 0;
	for(i = 0; i < 6; i++)
	{
		data[i] = i2c_smbus_read_byte_data(file, (readAddr+i));
	}
}

void initGyro()
{
	if(ioctl(file, I2C_SLAVE, gyroAddr)<0)
	{
		exit(1);
	}

	//set register 0x20 to 0xFF
	//enable gyro and selects max bandwidth
	i2c_smbus_write_word_data(file, 0x20, 0xFF);
}

void readGyro(uint8_t* data)
{
	if(ioctl(file, I2C_SLAVE, gyroAddr)<0)
	{
		exit(1);
	}

	uint8_t readAddr = 0x28;

	int i = 0;
	for(i = 0; i < 6; i++)
	{
		data[i] = i2c_smbus_read_byte_data(file, (readAddr+i));
	}
}

void initAltimeter(uint16_t* data)
{
	if(ioctl(file, I2C_SLAVE, altAddr)<0)
	{
		exit(1);
	}

	//get calibration values (0xAA- 0xBF)


	for(i = 0; i < 11; i++)
	{
		data[i] = i2c_smbus_read_byte_data(file, (readAddr+2i));
		data[i] |=  (i2c_smbus_read_byte_data(file, (readAddr+2i+1)) << 8) ;
	}

}

void readTemp(uint8_t* data)
{
	if(ioctl(file, I2C_SLAVE, gyroAddr)<0)
	{
		exit(1);
	}

	//send conversion command to control register 0xF4
	//temperature 0x2E
	i2c_smbus_write_word_data(file, 0xF4, 0x2E);



	//read from 0xF6 and 0xF7 to get values
	uint8_t readAddr = 0xF6; // MSB


	data[0] = i2c_smbus_read_byte_data(file, readAddr);

	readAddr = 0xF7; //LSB

	data[1] = i2c_smbus_read_byte_data(file, readAddr);

}

void readPres(uint8_t* data)
{


	if(ioctl(file, I2C_SLAVE, gyroAddr)<0)
	{
		exit(1);
	}


	//send conversion command to control register 0xF4
	//pressure 0x34
	i2c_smbus_write_word_data(file, 0xF4, 0x34);


	uint8_t readAddr = 0xF6; // MSB


	data[0] = i2c_smbus_read_byte_data(file, readAddr);

	readAddr = 0xF7; // LSB

	data[1] = i2c_smbus_read_byte_data(file, readAddr);
}
