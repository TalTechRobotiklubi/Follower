#ifndef I2C_H_
#define I2C_H_

unsigned char I2CRead(unsigned char ack);
void I2CWrite(unsigned char  data);
void I2CStop(void);
void I2CStart(void);
void InitI2C(void);

#endif /* I2C_H_ */