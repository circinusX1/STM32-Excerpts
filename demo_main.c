// optional code if we use a i2c mux


// your code
// each different sensor on different mux pin if the case
static struct ThpSensors Sensors[]=
{0x70,1,2,SHT35X_ADDRESS,false,
    &SHT35xx_init,&SHT35xx_read, &SHT35xx_Calibrate,
    {0,0,0}},

{0x70,2,2,BME280_ADDRESS,false,
    &BME280_init,&BME280_read, &BME280_Calibrate,
    {0,0,0}},

{0x70,3,2,HTU21D_ADDRESS,false,
    &HTU21_init,&HTU21_read, &HTU21_Calibrate,
    {0,0,0}},

{0x70,4,2,SI7013_ADDRESS,false,
    &SI7013_init,&SI7013_read, &SI7013_Calibrate,
    {0,0,0}},

{"", "",0,0,0,0,false,0,0,0,{0,0,0}}        // last 
};

// or all on same mux pin
static struct ThpSensors Sensors[]=
{{0x70,1,2,SHT35X_ADDRESS,false,
    &SHT35xx_init,&SHT35xx_read, &SHT35xx_Calibrate,
    {0,0,0}},

{0x70,1,2,BME280_ADDRESS,false,
    &BME280_init,&BME280_read, &BME280_Calibrate,
    {0,0,0}},

{0x70,1,2,HTU21D_ADDRESS,false,
    &HTU21_init,&HTU21_read, &HTU21_Calibrate,
    {0,0,0}},

{0x70,1,2,SI7013_ADDRESS,false,
    &SI7013_init,&SI7013_read, &SI7013_Calibrate,
    {0,0,0}},

{"", "",0,0,0,0,false,0,0,0,{0,0,0}}        // last 
};

// init code
