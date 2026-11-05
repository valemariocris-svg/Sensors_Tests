
#define I2C_SCL            7
#define I2C_SDA            8
#define I2C_SPEED          100000

#define MSEC_BASE_TIME     3000    // 3"

#define SHT45_I2C_ADDRESS  0x44
#define BMP390_I2C_ADDRESS 0x77
#define SGP4x_I2C_ADDRESS  0x59
#define SCD41_I2C_ADDRESS  0x62
#define SPS30_I2C_ADDRESS  0x69
#define INA228_I2C_ADDRESS 0x40


/* =========================   dichiarazioni classe SPS30_Mngr   ===================== */
class SPS30_Mngr {
public:
    SPS30_Mngr();

    void Init(TwoWire &port);
    void Next_Step(uint32_t DeltaMillis);
    void Cycle_Restart();

private:
    bool _printEnabd  = true;
    char _buffer[128];
    bool _SPS30_Ready = true;
    
    SensirionI2cSps30 _Sensirion_sps30; 
    
    enum Stato_t { PRE_WAIT, WARM_READ, POST_WAIT };
    Stato_t _status   = PRE_WAIT;

    const uint32_t _PRE_WAIT_MILLIS = 3000;    // 3 per SPS30, 20' per SCD41
    const uint32_t _WARM_DURATION   = 15000;   // 15 secondi

    void _print_Ser_Prodt();
    void _start_measure();    
    void _get_measures();
    void _stop_Measure();
    void _PrintStr(String str);
};

/* --------------------------------------------------------------------------------- */
/*                                                                                   */
extern SPS30_Mngr Sps30_mngr; /*                                                     */
/*                                                                                   */       
/* --------------------------------------------------------------------------------- */

