#include "mpu.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"

#define FSR 2000
//#define GYRO_SENS       ( 131.0f * 250.f / (float)FSR )
#define GYRO_SENS       16.375f 
#define QUAT_SENS       1073741824.f //2^30

#define EPSILON         0.0001f
#define PI_2            1.57079632679489661923f

struct s_mympu mympu;

struct s_quat { float w, x, y, z; }; 

union u_quat {
	struct s_quat _f;
	long _l[4];
} q;
 
static int ret;
static short gyro[3];
static short accel[3];
static short magneto[3];
static short sensors;
static unsigned char fifoCount;

//#define MPU_DEBUG

//void initAK8963(float * destination)
//{
//  // First extract the factory calibration for each magnetometer axis
//  uint8_t rawData[3];  // x/y/z gyro calibration data stored here
//  writeByte(AK8963_ADDRESS, AK8963_CNTL, 0x00); // Power down magnetometer
//  delay(10);
//  writeByte(AK8963_ADDRESS, AK8963_CNTL, 0x0F); // Enter Fuse ROM access mode
//  delay(10);
//  readBytes(AK8963_ADDRESS, AK8963_ASAX, 3, &rawData[0]);  // Read the x-, y-, and z-axis calibration values
//  destination[0] =  (float)(rawData[0] - 128)/256. + 1.;   // Return x-axis sensitivity adjustment values, etc.
//  destination[1] =  (float)(rawData[1] - 128)/256. + 1.;
//  destination[2] =  (float)(rawData[2] - 128)/256. + 1.;
//  writeByte(AK8963_ADDRESS, AK8963_CNTL, 0x00); // Power down magnetometer
//  delay(10);
//  // Configure the magnetometer for continuous read and highest resolution
//  // set Mscale bit 4 to 1 (0) to enable 16 (14) bit resolution in CNTL register,
//  // and enable continuous mode data acquisition Mmode (bits [3:0]), 0010 for 8 Hz and 0110 for 100 Hz sample rates
//  writeByte(AK8963_ADDRESS, AK8963_CNTL, Mscale << 4 | Mmode); // Set magnetometer data resolution and sample ODR
//  delay(10);
//}

int mympu_open(unsigned int rate) {
  	mpu_select_device(0);
   	mpu_init_structures();

    ret = mpu_set_sensors(INV_XYZ_GYRO|INV_XYZ_ACCEL|INV_XYZ_COMPASS);
   printf("set_sens: %d\n",ret);
#ifdef MPU_DEBUG
    if (ret) return 20+ret;
#endif

	ret = mpu_init(NULL);
    printf("Inint: %d\r\n",ret);
#ifdef MPU_DEBUG
	if (ret) return 10+ret;
#endif

  //  mpu_test();
	


    ret = setup_compass();
    printf("Compass: ret %d\r\n",ret);
        ret = mpu_set_gyro_fsr(FSR);
#ifdef MPU_DEBUG
    if (ret) return 30+ret;
#endif

        ret = mpu_set_accel_fsr(8);
#ifdef MPU_DEBUG
    if (ret) return 40+ret;
#endif

        mpu_get_power_state((unsigned char *)&ret);
#ifdef MPU_DEBUG
    if (!ret) return 50+ret;
#endif

        ret = mpu_configure_fifo(INV_XYZ_GYRO|INV_XYZ_ACCEL);
        printf("FIFO: %d\n",ret);
#ifdef MPU_DEBUG
    if (ret) return 60+ret;
#endif

    dmp_select_device(0);
    dmp_init_structures();
//  mpu_test();
    ret = dmp_load_motion_driver_firmware();
  //  printf("load: %d\n",ret);
#ifdef MPU_DEBUG
    if (ret) return 80+ret;
#endif

    ret = dmp_set_fifo_rate(rate);
#ifdef MPU_DEBUG
    if (ret) return 90+ret;
#endif

    ret = mpu_set_dmp_state(1);
   // printf("state: %d\n",ret);
#ifdef MPU_DEBUG
    if (ret) return 100+ret;
#endif

    ret = dmp_enable_feature(DMP_FEATURE_6X_LP_QUAT|DMP_FEATURE_SEND_CAL_GYRO|DMP_FEATURE_GYRO_CAL);
//	ret = dmp_enable_feature(DMP_FEATURE_SEND_CAL_GYRO|DMP_FEATURE_GYRO_CAL);
#ifdef MPU_DEBUG
    if (ret) return 110+ret;
#endif

	return 0;
}

static inline float rad2deg( float rad )
{
        //return (180.f/PI) * rad;
	return 57.2957795131f * rad;
}

static float test, sqy,sqz,sqw;
static void quaternionToEuler( const struct s_quat *q, float* x, float* y, float* z )
{
        sqy = q->y * q->y;
        sqz = q->z * q->z;
        sqw = q->w * q->w;

        test = q->x * q->z - q->w * q->y;

        if( test > 0.5f - EPSILON )
        {
                *x = 2.f * atan2( q->y, q->w );
                *y = PI_2;
                *z = 0;
        }
        else if( test < -0.5f + EPSILON )
        {
                *x = -2.f * atan2( q->y, q->w );
                *y = -PI_2;
                *z = 0;
        }
        else
        {
                *x = atan2( 2.f * ( q->x * q->w + q->y * q->z ), 1.f - 2.f * ( sqz + sqw ) );
                *y = asin( 2.f * test );
                *z = atan2( 2.f * ( q->x * q->y - q->z * q->w ), 1.f - 2.f * ( sqy + sqz ) );
        }
}

static inline float wrap_180(float x) {
	return (x<-180.f?x+360.f:(x>180.f?x-180.f:x));
}

int mympu_update() {
    int cnt = 0;
 	do {
		ret = dmp_read_fifo(gyro,NULL,q._l,NULL,&sensors,&fifoCount);
		/* will return:
			0 - if ok
			1 - no packet available
			2 - if BIT_FIFO_OVERFLOWN is set
			3 - if frame corrupted
		       <0 - if error
		*/
        ++cnt;
		if (ret!=0) return ret; 
	} while (fifoCount>1);

    //printf("fifo count: %d\n",cnt);

	q._f.w = (float)q._l[0] / (float)QUAT_SENS;
	q._f.x = (float)q._l[1] / (float)QUAT_SENS;
	q._f.y = (float)q._l[2] / (float)QUAT_SENS;
	q._f.z = (float)q._l[3] / (float)QUAT_SENS;


	quaternionToEuler( &q._f, &mympu.ypr[2], &mympu.ypr[1], &mympu.ypr[0] );
	
	/* need to adjust signs and do the wraps depending on the MPU mount orientation */ 
	/* if axis is no centered around 0 but around i.e 90 degree due to mount orientation */
	/* then do:  mympu.ypr[x] = wrap_180(90.f+rad2deg(mympu.ypr[x])); */
	mympu.ypr[0] = rad2deg(mympu.ypr[0]);
	mympu.ypr[1] = rad2deg(mympu.ypr[1]);
	mympu.ypr[2] = rad2deg(mympu.ypr[2]);

	/* need to adjust signs depending on the MPU mount orientation */ 
	mympu.gyro[0] = -(float)gyro[2] / GYRO_SENS;
	mympu.gyro[1] = (float)gyro[1] / GYRO_SENS;
	mympu.gyro[2] = (float)gyro[0] / GYRO_SENS; 
	
	mpu_get_accel_reg(accel,NULL);
	
	mympu.accel[0] = (float)accel[0] / 16384;
	mympu.accel[1] = (float)accel[1] / 16384;
	mympu.accel[2] = (float)accel[2] / 16384;
	
    //Serial.println(mpu_get_compass_reg(magneto,NULL));
    //printf("compass: %d\r\n",mpu_get_compass_reg(magneto,NULL));
//	mympu.magneto[0] = (float)magneto[0] / 0.6;
//	mympu.magneto[1] = (float)magneto[1] / 0.6;
//	mympu.magneto[2] = (float)magneto[2] / 0.6;

	return 0;
}

