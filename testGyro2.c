/*
 * This file is an MPU6050 demonstration.
 * https://openest.io/en/2020/01/21/mpu6050-accelerometer-on-raspberry-pi/
 * Copyright (c) 2020 Julien Grossholtz - https://openest.io.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <math.h>

#define MPU6050_I2C_ADDR 0x68

#define REG_ACCEL_ZOUT_H 0x3F
#define REG_ACCEL_ZOUT_L 0x40
#define REG_PWR_MGMT_1 0x6B
#define REG_ACCEL_CONFIG 0x1C
#define REG_SMPRT_DIV 0x19
#define REG_CONFIG 0x1A
#define REG_FIFO_EN 0x23
#define REG_USER_CTRL 0x6A
#define REG_FIFO_COUNT_L 0x72
#define REG_FIFO_COUNT_H 0x73
#define REG_FIFO 0x74
#define REG_WHO_AM_I 0x75

#define MPU_ACCEL_XOUT1 0x3b
#define MPU_ACCEL_XOUT2 0x3c
#define MPU_ACCEL_YOUT1 0x3d
#define MPU_ACCEL_YOUT2 0x3e
#define MPU_ACCEL_ZOUT1 0x3f
#define MPU_ACCEL_ZOUT2 0x40

#define MPU_GYRO_XOUT1 0x43
#define MPU_GYRO_XOUT2 0x44
#define MPU_GYRO_YOUT1 0x45
#define MPU_GYRO_YOUT2 0x46
#define MPU_GYRO_ZOUT1 0x47
#define MPU_GYRO_ZOUT2 0x48


#define MPU_TEMP1 0x41
#define MPU_TEMP2 0x42


int file = -1;

// Please note, this is not the recommanded way to write data
// to i2c devices from user space.
void i2c_write(__u8 reg_address, __u8 val) {
	char buf[2];
	if(file < 0) {
		printf("Error, i2c bus is not available\n");
		exit(1);
	}

	buf[0] = reg_address;
	buf[1] = val;

	if (write(file, buf, 2) != 2) {
		printf("Error, unable to write to i2c device\n");
		exit(1);
	}

}

// Please note, this is not thre recommanded way to read data
// from i2c devices from user space.
char i2c_read(uint8_t reg_address) {
	char buf[1];
	if(file < 0) {
		printf("Error, i2c bus is not available\n");
		exit(1);
	}

	buf[0] = reg_address;

	if (write(file, buf, 1) != 1) {
		printf("Error, unable to write to i2c device\n");
		exit(1);
	}


	if (read(file, buf, 1) != 1) {
		printf("Error, unable to read from i2c device\n");
		exit(1);
	}

	return buf[0];

}

uint16_t merge_bytes( uint8_t LSB, uint8_t MSB) {
	return  (uint16_t) ((( LSB & 0xFF) << 8) | MSB);
}

// 16 bits data on the MPU6050 are in two registers,
// encoded in two complement. So we convert those to int16_t
int16_t two_complement_to_int( uint8_t LSB, uint8_t MSB) {
	int16_t signed_int = 0;
	uint16_t word;

	word = merge_bytes(LSB, MSB);

	if((word & 0x8000) == 0x8000) { // negative number
		signed_int = (int16_t) -(~word);
	} else {
		signed_int = (int16_t) (word & 0x7fff);
	}

	return signed_int;
}

double dist(double a, double b)
{
return sqrt((a*a) + (b*b));
}
 
double get_y_rotation(double x, double y, double z)
{
double radians;
radians = atan2(x, dist(y, z));
return -(radians * (180.0 / M_PI));
}
 
double get_x_rotation(double x, double y, double z)
{
double radians;
radians = atan2(y, dist(x, z));
return (radians * (180.0 / M_PI));
}

 
double get_z_rotation(double x, double y, double z)
{
double radians;
radians = atan2(z, dist(y, x));
return (radians * (180.0 / M_PI));
}
 
int main(int argc, char *argv[]) {
	//int adapter_nr = 1; /* probably dynamically determined */
	char bus_filename[250];



	snprintf(bus_filename, 250, "/dev/i2c-1");
	file = open(bus_filename, O_RDWR);
	if (file < 0) {
		/* ERROR HANDLING; you can check errno to see what went wrong */
		exit(1);
	}


	if (ioctl(file, I2C_SLAVE, MPU6050_I2C_ADDR) < 0) {
		/* ERROR HANDLING; you can check errno to see what went wrong */
		exit(1);
	}

	i2c_write(REG_PWR_MGMT_1, 0x00);

	while(1){
	
		double temp = two_complement_to_int(i2c_read( MPU_TEMP1),i2c_read( MPU_TEMP2));

       double xaccel = two_complement_to_int(i2c_read( MPU_ACCEL_XOUT1),i2c_read( MPU_ACCEL_XOUT2));
       double yaccel = two_complement_to_int(i2c_read( MPU_ACCEL_YOUT1),i2c_read( MPU_ACCEL_YOUT2));
       double zaccel = two_complement_to_int(i2c_read( MPU_ACCEL_ZOUT1),i2c_read( MPU_ACCEL_ZOUT2));

       double xgyro = two_complement_to_int(i2c_read( MPU_GYRO_XOUT1),i2c_read( MPU_GYRO_XOUT2))/131;
       double ygyro = two_complement_to_int(i2c_read( MPU_GYRO_YOUT1),i2c_read( MPU_GYRO_YOUT2))/131;
       double zgyro = two_complement_to_int(i2c_read( MPU_GYRO_ZOUT1),i2c_read( MPU_GYRO_ZOUT2))/131;
       
        double xs= xaccel/16384.0;
       double ys = yaccel/16384.0;
       double zs = zaccel/16384.0;
		printf("temp: %f\n",temp);
        printf("accel x,y,z: %f, %f, %f\n", xs, ys, zs);
        printf("gyro x,y,z: %f, %f, %f\n\n", xgyro, ygyro, zgyro);
        printf("------------\n");
        printf("X-Rotation: %f\n",get_x_rotation(xs,ys,zs));
        printf("Y-Rotation: %f\n",get_y_rotation(xs,ys,zs));
        printf("Z-Rotation: %f\n",get_z_rotation(xs,ys,zs));
			usleep(900000);
	

	}

	return 0;
}
