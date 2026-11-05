/* ----------------------------------------------------------------------------------- */
#ifndef SPS30_H
#define SPS30_H

#include <Preferences.h>


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
    Preferences       _prefs;
    
    enum Stato_t { PRE_WAIT, WARM_READ, POST_WAIT };
    Stato_t _status   = PRE_WAIT;

    int _Print_Count = 0;

    const uint32_t _PRE_WAIT_MILLIS = 3000;    // 3 per SPS30, 20' per SCD41
    const uint32_t _WARM_DURATION   = 15000;   // 15 secondi

    void _print_Ser_Prodt();
    void _start_measure();    
    void _get_measures();
    void _stop_Measure();
    void _Clean_fan_manager();
    void _Clean_Count();
    void _PrintStr(String str);
};

/* --------------------------------------------------------------------------------- */
/*                                                                                   */
extern SPS30_Mngr Sps30_mngr; /*                                                     */
/*                                                                                   */       
/* --------------------------------------------------------------------------------- */
#endif