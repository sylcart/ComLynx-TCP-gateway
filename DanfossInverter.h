#ifndef DanfossInverter_h
#define DanfossInverter_h
/*
    Name: DanfossInverter
*/ 

#include "Arduino.h"
#include "ComLynx.h"

class DanfossInverter {
  public:

    DanfossInverter(ComLynx *ComlynxInv);
    void GetStatus(void);
    void GetName(void);
    void SetSN(String *_SerialNumber);
    // void PrintAll(void);
    // String LongString(Par_e ParEnum );
    // String MeasString(Par_e ParEnum);
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
    // virtual bool SanityCheck(int i) = 0;

  protected:
    String RS485_Addr      = "FFFF";  // Inverter Adress - as reported by findaddr
    String SourceModuleID  = "D0";    // Source Module ID
    String DefaultModuleID = "00";    // default Module ID to get status, serial ...
    String NameModuleID    = "00";    // Module ID to get name
    ComLynx *_ComlynxInv;

    String *INV_modes_txt;
    int *NbINV_power_modes;

    struct Par_s                                                                                  
    {                                                                                             
      String *ParName;
      String *Name;
      String *Cmd;
      String *Module;
      float  *Conv;
      String *Unit;
      float  *Meas;
      long   *Raw;
      int    *NbParam;
    }InvParamList;

    String ProductNumber = ">>NotInitialized<<";  
    String SerialNumber = ">>NotInitialized<<";  
    String OpModeTxt = ">>NotInitialized<<";
    String Name = ">>NotInitialized<<";
    String Type = "Undefined";
    int ParamIndex = 0;
};

class DanfossTLX: public DanfossInverter {
  public:
    DanfossTLX(ComLynx *ComlynxInv, const String ComlynxAddr) ;
    DanfossTLX(ComLynx *ComlynxInv, const String ComlynxAddr, String *InvSN, String *InvPN);
};

class DanfossULX : public DanfossInverter {
  public:
    DanfossULX(ComLynx *ComlynxInv, const String ComlynxAddr);
    DanfossULX(ComLynx *ComlynxInv, const String ComlynxAddr, String *InvSN, String *InvPN);
};

#endif