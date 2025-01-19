#ifndef DanfossInverter_h
#define DanfossInverter_h
/*
    Name: DanfossInverter
*/ 

#include "Arduino.h"
#include "ComLynx.h"
#include <vector>

class DanfossInverter {
  public:

    DanfossInverter(ComLynx *ComlynxInv, const String ComlynxAddr = "", String *InvSN = nullptr, String *InvPN = nullptr);
    void GetStatus(void);
    void GetName(void);
    void SetSN(String *_SerialNumber);
    String PrintType(void);
    void GetAllParameters(void);
    void GetRollingParameters(void);
    void GetOneParameter(String ParameterName);
    void GetOneParameter(int i);
    int PrintParametersCount(void);
    String PrintSN(void);
    String PrintProductNumber(void);
    String PrintName(void);
    String PrintOpMode(void);
    String PrintNameParameter(int index);
    String PrintParNameParameter(int index);
    String PrintUnitParameter(int index);
    float PrintMeasParameter(int index);
    String PrintFormatedMeasParameter(int index);

  protected:
    String RS485_Addr      = "FFFF";  // Inverter Adress - as reported by findaddr
    String SourceModuleID  = "D0";    // Source Module ID
    String DefaultModuleID = "00";    // default Module ID to get status, serial ...
    String NameModuleID    = "00";    // Module ID to get name
    ComLynx *_ComlynxInv;

    std::vector<String> INV_modes_txt;
    int NbINV_power_modes;

    struct Par_s                                                                                  
    {                                                                                             
      std::vector<String> ParName;
      std::vector<String> Name;
      std::vector<String> Cmd;
      std::vector<String> Module;
      std::vector<float> Conv;
      std::vector<String> Unit;
      std::vector<float> Meas;
      std::vector<long> Raw;
      int    NbParam;
    }InvParamList;

    String ProductNumber = ">>NotInitialized<<";  
    String SerialNumber = ">>NotInitialized<<";  
    String OpModeTxt = ">>NotInitialized<<";
    String Name = ">>NotInitialized<<";
    String Type = "Undefined";
    boolean HistData = false; // historical data enabled (will not be sended via MQTT ony rest API call) (TLX only)
    boolean ModTempS = false; // module temperature sensor (TLX only)
    boolean AmbTempS = false; // ambiant temperature sensor (TLX only)
    boolean IRSTemp  = false; // Irradiation sensor temperature (TLX only)
    boolean IRS      = false; // Irradiation sensor (TLX only)
    boolean EM       = false; // Energy Meter (TLX only)
    int ParamIndex = 0;
};

class DanfossTLX: public DanfossInverter {
  public:
    DanfossTLX(ComLynx *ComlynxInv, const String ComlynxAddr = "", String *InvSN = nullptr, String *InvPN = nullptr, byte *OptionByte = nullptr);
};

class DanfossULX : public DanfossInverter {
  public:
    DanfossULX(ComLynx *ComlynxInv, const String ComlynxAddr = "", String *InvSN = nullptr, String *InvPN = nullptr);
};

#endif