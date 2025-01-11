/*
    Name: DanfossInverter 
*/

#include "DanfossInverter.h"

DanfossInverter::DanfossInverter(ComLynx *ComlynxInv, const String ComlynxAddr, String *InvSN, String *InvPN)
{
  _ComlynxInv = ComlynxInv;
  if (ComlynxAddr != "") {RS485_Addr = ComlynxAddr;}
  if (InvPN != nullptr) {ProductNumber = *InvPN;}
  if (InvSN != nullptr) {SerialNumber = *InvSN;}
}

void DanfossInverter::GetStatus(void)
{
  String GetParam;
  String _SerialNumber;
  String _ProductNumber;

  // Serial Number
  for (int j=3;j > 0; j--) {
    GetParam = DefaultModuleID + SourceModuleID + "460" + String(j);
    _SerialNumber = _ComlynxInv->GetInvString(RS485_Addr, GetParam);
  }
  _SerialNumber.replace(" ", "");
  if (_SerialNumber != "") {this->SerialNumber = _SerialNumber;} 
  Serial.println("Serial#                                 " + _SerialNumber);
  
  // Product Number
  for (int j=3;j > 0; j--) {
    GetParam = DefaultModuleID + SourceModuleID + "3C0" + String(j);
    _ProductNumber = _ComlynxInv->GetInvString(RS485_Addr, GetParam);
  }
  _ProductNumber.replace(" ", "");
  if (_ProductNumber != "") {this->ProductNumber = _ProductNumber;} 
  Serial.println("Product#                                " + _ProductNumber);
}

void DanfossInverter::GetName(void)
{
  String RXData;
  String GetParam;
  String _InverterName;

  for (int j=7;j > 3; j--) {
    GetParam = NameModuleID + SourceModuleID + "3C6" + String(j);
    _InverterName = _ComlynxInv->GetInvString(RS485_Addr, GetParam);
  }
  _InverterName.replace(" ", "");
  if (_InverterName != "") {this->Name = _InverterName;} 
  Serial.println("Inverter Name#                          " + _InverterName);
}

void DanfossInverter::SetSN(String *_SerialNumber)
{
  this->SerialNumber = *_SerialNumber;
  Serial.println("Inverter SN#                          " + SerialNumber);
}

String DanfossInverter::PrintType(void)
{
  return Type;
}

String DanfossInverter::PrintSN(void) {
  return SerialNumber;
}

String DanfossInverter::PrintProductNumber(void){
  return ProductNumber;
}

String DanfossInverter::PrintName(void){
  return Name;
}

String DanfossInverter::PrintOpMode(void){
  return OpModeTxt;
}

void DanfossInverter::GetRollingParameters(void){ // don't know if thi is usefull
  GetOneParameter(ParamIndex);
  ParamIndex++;
  if(ParamIndex >= *InvParamList.NbParam){
    ParamIndex = 0;
  }
}

void DanfossInverter::GetAllParameters(void){
  for (int i = 0; i < *InvParamList.NbParam; i++) {
    GetOneParameter(i);
  }
}

void DanfossInverter::GetOneParameter(String ParameterName){
  for (int i = 0; i < *InvParamList.NbParam; i++) {
    if (InvParamList.ParName[i] == ParameterName){
      GetOneParameter(i);
    }
  } 
}

void DanfossInverter::GetOneParameter(int i){
  if (i >= 0 && i < *InvParamList.NbParam) {
    String GetParam = InvParamList.Module[i] + SourceModuleID + InvParamList.Cmd[i];
    long TmpRaw = _ComlynxInv->GetInvData(RS485_Addr, GetParam);
    if (TmpRaw == _ComlynxInv->NOTVALID) {
      Serial.println(InvParamList.ParName[i] + " Raw Not Valid");
    } else {
      if (InvParamList.ParName[i] == "OpMode") {
        int Inv_pw_mode = *NbINV_power_modes;
        if (Type == "TLX") {
          if (InvParamList.Raw[i] <= 9) {
            Inv_pw_mode = 0;
          } else if (InvParamList.Raw[i]>=10 && InvParamList.Raw[i]<=49) {
            Inv_pw_mode = 1;
          } else if (InvParamList.Raw[i]>=50 && InvParamList.Raw[i]<=59) {
            Inv_pw_mode = 2;
          } else if (InvParamList.Raw[i]>=60 && InvParamList.Raw[i]<=69) {
            Inv_pw_mode = 3;
          } else if (InvParamList.Raw[i]>=70 && InvParamList.Raw[i]<=79) {
            Inv_pw_mode = 4;
          } else if (InvParamList.Raw[i]>=80 && InvParamList.Raw[i]<=89) {
            Inv_pw_mode = 5;
          }
        } else {
          if (Type == "ULX") {
            if (InvParamList.Raw[i] == 0){
              Inv_pw_mode = 0;
            } else if (InvParamList.Raw[i]==4) {
              Inv_pw_mode = 1;
            }else if (InvParamList.Raw[i]==5) {
              Inv_pw_mode = 2;
            }else if (InvParamList.Raw[i]==6) {
              Inv_pw_mode = 3;
            }
          }
        }
        if (Inv_pw_mode < *NbINV_power_modes) {
          OpModeTxt = INV_modes_txt[Inv_pw_mode];
        }else {
          OpModeTxt = "Mode not known";
        }
      } else {
        InvParamList.Raw[i] = TmpRaw;
        InvParamList.Meas[i] = float(InvParamList.Raw[i]) / InvParamList.Conv[i];
        Serial.println(String(InvParamList.ParName[i]) + String(" (Raw) : ")+ String(InvParamList.Raw[i]));
        Serial.println(String(InvParamList.ParName[i]) + String(" : ")+ String(InvParamList.Meas[i]));
      }
    }
  }
}

int DanfossInverter::PrintParametersCount(void){
  return *InvParamList.NbParam;
}

String DanfossInverter::PrintNameParameter(int index){
  return InvParamList.Name[index];
}

String DanfossInverter::PrintParNameParameter(int index){
  return InvParamList.ParName[index];
}

String DanfossInverter::PrintUnitParameter(int index){
  return InvParamList.Unit[index];
}

float DanfossInverter::PrintMeasParameter(int index){
  return InvParamList.Meas[index];
}

String DanfossInverter::PrintFormatedMeasParameter(int index)
{
  // Print with "right" number of digits based one units. Could have used Conv[] enstead
  String Meas = String(InvParamList.Meas[index],0); //Default
  if( InvParamList.Unit[index] == String("V")  )Meas = String(InvParamList.Meas[index],1);
  if( InvParamList.Unit[index] == String("A")  )Meas = String(InvParamList.Meas[index],3);
  if( InvParamList.Unit[index] == String("mA") )Meas = String(InvParamList.Meas[index],0);
  if( InvParamList.Unit[index] == String("W")  )Meas = String(InvParamList.Meas[index],0);
  if( InvParamList.Unit[index] == String("kWh"))Meas = String(InvParamList.Meas[index],3);
  if( InvParamList.Unit[index] == String("Hz" ))Meas = String(InvParamList.Meas[index],1);
  return Meas;
}

DanfossTLX::DanfossTLX(ComLynx *ComlynxInv, const String ComlynxAddr, String *InvSN, String *InvPN, byte *OptionByte) : DanfossInverter(ComlynxInv, ComlynxAddr, InvSN, InvPN)
{
  InvParamList.NbParam  = new int;
  *InvParamList.NbParam = 41;

  Type = "TLX";
  DefaultModuleID = "08";    // default Module ID to get status, serial ...
  NameModuleID    = "08";    // Module ID to get name

  NbINV_power_modes = new int;
  *NbINV_power_modes = 6;
  INV_modes_txt = new String[*NbINV_power_modes] {
  "Off Grid, OFF   ", // Off Grid The complete inverter is shut down
  "Connecting, Boot", // The inverter is booting,initializing itself etc.
  "Connecting      ", // The inverter is monitoring the grid, preparing to connect.
  "On Grid         ", // The inverter is connected to grid and is producing energy.
  "Fail safe       ", // The inverter is disconnected from grid because of an error situation.
  "Off Grid, Comm  " };// The inverter is shutdown (except for theuser interface and the communication interfaces

  InvParamList.ParName = new String[*InvParamList.NbParam] { "TotalE",                 "PvV1",                "PvV2",                "PvV3",                "PvA1",                "PvA2",                "PvA3",                "PvPw1",            "PvPw2",            "PvPw3",            "PvE1",               "PvE2",               "PvE3",               "GridV1",           "GridV2",           "GridV3",           "GridV1Avg",                "GridV2Avg",                "GridV3Avg",                "GridVL1L2",             "GridVL2L3",             "GridVL3L1",             "GridA1",           "GridA2",           "GridA3",           "GridP1",         "GridP2",         "GridP3",         "GridP",       "GridEL1day",             "GridEL2day",            "GridEL3day",            "GridEday",          "GridDC1",             "GridDC2",             "GridDC3",             "HZ1",                "HZ2",                "HZ3",                "HZ",                  "OpMode" };
  InvParamList.Name = new String[*InvParamList.NbParam]    { "Total Energy Production","PV Voltage, input 1", "PV Voltage, input 2", "PV Voltage, input 3", "PV Current, input 1", "PV Current, input 2", "PV Current, input 3", "PV Power input 1", "PV Power input 2", "PV Power input 3", "PV Energy, input 1", "PV Energy, input 2", "PV Energy, input 3", "Grid voltage, L1", "Grid voltage, L2", "Grid voltage, L3", "Grid voltage, L1 (10min)", "Grid voltage, L2 (10min)", "Grid voltage, L3 (10min)", "Grid voltage, L1 – L2", "Grid voltage, L2 – L3", "Grid voltage, L3 – L1", "Grid current, L1", "Grid current, L2", "Grid current, L3", "Grid power, L1", "Grid power, L2", "Grid power, L3", "Grid power",  "Grid Energy Today, L1 ", "Grid Energy Today, L2", "Grid Energy Today, L3", "Grid Energy Today", "Grid current, DC L1", "Grid current, DC L2", "Grid current, DC L3", "Grid frequency, L1", "Grid frequency, L2", "Grid frequency, L3", "Grid frequency, AVG", "Operating Mode"};
  InvParamList.Cmd = new String[*InvParamList.NbParam]     { "0102",                   "0228",                "0229",                "022A",                "022D",                "022E",                "022F",                "0232",             "0233",             "0234",             "0237",               "0238",               "0239",               "023C",             "023D",             "023E",             "025B",                     "025C",                     "025D",                     "025E",                  "025F",                  "0260",                  "023F",             "0240",             "0241",             "0242",           "0243",           "0244",           "0246",        "0247",                   "0248",                  "0249",                  "024A",              "024C",                "024D",                "024E",                "0261",               "0262",               "0263",               "0250",                "0A02" };
  InvParamList.Module = new String[*InvParamList.NbParam]  { "08",                     "08",                  "08",                  "08",                  "08",                  "08",                  "08",                  "08",               "08",               "08",               "08",                 "08",                 "08",                 "08",               "08",               "08",               "08",                       "08",                       "08",                       "08",                    "08",                    "08",                    "08",               "08",               "08",               "08",             "08",             "08",             "08",          "08",                     "08",                    "08",                    "08",                "08",                  "08",                  "08",                  "08",                 "08",                 "08",                 "08",                  "08" };
  InvParamList.Conv = new float[*InvParamList.NbParam]     { 1000.0f,                  10.0f,                 10.0f,                 10.0f,                 1000.0f,               1000.0f,               1000.0f,               1000.0f,            1000.0f,            1000.0f,            1000.0f,              1000.0f,              1000.0f,              10.0f,              10.0f,              10.0f,              10.0f,                      10.0f,                      10.0f,                      10.0f,                   10.0f,                   10.0f,                   1000.0f,            1000.0f,            1000.0f,            1.0f,             1.0f,             1.0f,             1.0f,          1000.0f,                  1000.0f,                 1000.0f,                 1000.0f,             1.0f,                  1.0f,                  1.0f,                  1000.0f,              1000.0f,              1000.0f,              1000.0f,               1.0f };
  InvParamList.Unit = new String[*InvParamList.NbParam]    { "kWh",                    "V",                   "V",                   "V",                   "A",                   "A",                   "A",                   "W",                "W",                "W",                "kWh",                "kWh",                "kWh",                "V",                "V",                "V",                "V",                        "V",                        "V",                        "V",                     "V",                     "V",                     "A",                "A",                "A",                "W",              "W",              "W",              "W",           "kWh",                    "kWh",                   "kWh",                   "kWh",               "mA",                  "mA",                  "mA",                  "Hz",                 "Hz",                 "Hz",                 "Hz",                  "" };

  if (OptionByte != nullptr) {
    if ((*OptionByte & 1) == 1) {
      HistData = true;
      *InvParamList.NbParam += 4;
      String* ntParName = new String[*InvParamList.NbParam];
      String* ntName = new String[*InvParamList.NbParam];
      String* ntCmd = new String[*InvParamList.NbParam];
      String* ntModule = new String[*InvParamList.NbParam];
      float* ntConv = new float[*InvParamList.NbParam];
      String* ntUnit = new String[*InvParamList.NbParam];
      ntParName[0] = InvParamList.ParName[0];
      ntName[0]    = InvParamList.Name[0];
      ntCmd[0]     = InvParamList.Cmd[0];
      ntModule[0]  = InvParamList.Module[0]; 
      ntConv[0]    = InvParamList.Conv[0];
      ntUnit[0]    = InvParamList.Unit[0];
      ntParName[1] = "ProdTday";
      ntName[1]    = "Inverter production today";
      ntCmd[1]     = "780A";
      ntModule[1]  = "08"; 
      ntConv[1]    = 1000.0f;
      ntUnit[1]    = "kWh";
      ntParName[2] = "PThiWee";
      ntName[2]    = "Production this Week";
      ntCmd[2]     = "7814";
      ntModule[2]  = "08"; 
      ntConv[2]    = 1000.0f;
      ntUnit[2]    = "kWh";
      ntParName[3] = "PThiMth";
      ntName[3]    = "Production this Month";
      ntCmd[3]     = "781E";
      ntModule[3]  = "08"; 
      ntConv[3]    = 1000.0f;
      ntUnit[3]    = "kWh";
      ntParName[4] = "ProdTyear";
      ntName[4]    = "Inverter production this year";
      ntCmd[4]     = "7832";
      ntModule[4]  = "08"; 
      ntConv[4]    = 1000.0f;
      ntUnit[4]    = "kWh";
      for (int i = 5; i < *InvParamList.NbParam; i++) {
        ntParName[i] = InvParamList.ParName[i-4];
        ntName[i]    = InvParamList.Name[i-4];
        ntCmd[i]     = InvParamList.Cmd[i-4];
        ntModule[i]  = InvParamList.Module[i-4]; 
        ntConv[i]    = InvParamList.Conv[i-4];
        ntUnit[i]    = InvParamList.Unit[i-4];
      }
      delete[] InvParamList.ParName;
      delete[] InvParamList.Name;
      delete[] InvParamList.Cmd;
      delete[] InvParamList.Module; 
      delete[] InvParamList.Conv;
      delete[] InvParamList.Unit;
      InvParamList.ParName = ntParName;
      InvParamList.Name = ntName;
      InvParamList.Cmd = ntCmd;
      InvParamList.Module = ntModule; 
      InvParamList.Conv = ntConv;
      InvParamList.Unit = ntUnit;
    }
  // historical parameters could be added as an option in the future but i think that it should not be added to other parameters
  //   String ParName[HIST_ENUMS] = { "PLasMon",                "PLasTue",                 "PLasWed",                   "PLasThu",                  "PLasFri",                 "PLasSat",                  "PLasSun",                "PLasWee",              "P2Wago",                 "P3Wago",                 "P4Wago",                 "PLasJan",                 "PLasFeb",                  "PLasMar",               "PLasApr",               "PLasMay",             "PLasJun",              "PLasJul",              "PLasAug",                "PLasSep",                   "PLasOct",                 "PLasNov",                  "PLasDec",                  "PLasY",                "P2Yago",                 "P3Yago",                 "P4Yago",                 "P5Yago",                 "P6Yago",                 "P7Yago",                 "P8Yago",                 "P9Yago",                 "P10Yago",                 "P11Yago",                "P12Yago",                 "P13Yago",                 "P14Yago",                 "P15Yago",                 "P16Yago",                "P17Yago",                 "P18Yago",                 "P19Yago",                 "P20Yago",               };
  //   String Name[HIST_ENUMS]    = { "Production last Monday", "Production last Tuesday", "Production last Wednesday", "Production last Thursday", "Production last Friday",  "Production last Saturday", "Production last Sunday", "Production last Week", "Production 2 Weeks ago", "Production 3 Weeks ago", "Production 4 Weeks ago", "Production last January", "Production last February", "Production last March", "Production last April", "Production last May", "Production last June", "Production last July", "Production last August", "Production last September", "Production last October", "Production last November", "Production last December", "Production last Year", "Production 2 Years Ago", "Production 3 Years Ago", "Production 4 Years Ago", "Production 5 Years Ago", "Production 6 Years Ago", "Production 7 Years Ago", "Production 8 Years Ago", "Production 9 Years Ago", "Production 10 Years Ago", "Production 11 Years Ago","Production 12 Years Ago", "Production 13 Years Ago", "Production 14 Years Ago", "Production 15 Years Ago", "Production 16 Years Ago","Production 17 Years Ago", "Production 18 Years Ago", "Production 19 Years Ago", "Production 20 Years Ago"};
  //   String Cmd[HIST_ENUMS]     = { "780B",                   "780C",                    "780D",                      "780E",                     "780F",                    "7810",                     "7811",                   "7815",                 "7816",                   "7817",                   "7818",                   "781F",                    "7820",                     "7821",                  "7822",                  "7823",                "7824",                 "7825",                 "7826",                   "7827",                      "7828",                    "7829",                     "782A",                     "7833",                 "7834",                   "7835",                   "7836",                   "7837",                   "7838",                   "7839",                   "783A",                   "783B",                   "783C",                    "783D",                   "783E",                    "783F",                    "7840",                    "7841",                    "7842",                   "7843",                    "7844",                    "7845",                    "7846",                  };
  //   String Module[HIST_ENUMS]  = { "08",                     "08",                      "08",                        "08",                       "08",                      "08",                       "08",                     "08",                   "08",                     "08",                     "08",                     "08",                      "08",                       "08",                    "08",                    "08",                  "08",                   "08",                   "08",                     "08",                        "08",                      "08",                       "08",                       "08",                   "08",                     "08",                     "08",                     "08",                     "08",                     "08",                     "08",                     "08",                     "08",                      "08",                     "08",                      "08",                      "08",                      "08",                      "08",                     "08",                      "08",                      "08",                      "08",                    };
  //   float  Conv[HIST_ENUMS]    = { 1000.0f,                  1000.0f,                   1000.0f,                     1000.0f,                    1000.0f,                   1000.0f,                    1000.0f,                  1000.0f,                1000.0f,                  1000.0f,                  1000.0f,                  1000.0f,                   1000.0f,                    1000.0f,                 1000.0f,                 1000.0f,               1000.0f,                1000.0f,                1000.0f,                  1000.0f,                     1000.0f,                   1000.0f,                    1000.0f,                    1000.0f,                1000.0f,                  1000.0f,                  1000.0f,                  1000.0f,                  1000.0f,                  1000.0f,                  1000.0f,                  1000.0f,                  1000.0f,                   1000.0f,                  1000.0f,                   1000.0f,                   1000.0f,                   1000.0f,                   1000.0f,                  1000.0f,                   1000.0f,                   1000.0f,                   1000.0f,                 };
  //   String Unit[HIST_ENUMS]    = { "kWh",                    "kWh",                     "kWh",                       "kWh",                      "kWh",                     "kWh",                      "kWh",                    "kWh",                  "kWh",                    "kWh",                    "kWh",                    "kWh",                     "kWh",                      "kWh",                   "kWh",                   "kWh",                 "kWh",                  "kWh",                  "kWh",                    "kWh",                       "kWh",                     "kWh",                      "kWh",                      "kWh",                  "kWh",                    "kWh",                    "kWh",                    "kWh",                    "kWh",                    "kWh",                    "kWh",                    "kWh",                    "kWh",                     "kWh",                    "kWh",                     "kWh",                     "kWh",                     "kWh",                     "kWh",                    "kWh",                     "kWh",                     "kWh",                     "kWh",                   };

    if ((*OptionByte & 2) == 2) {
      ModTempS = true;
      *InvParamList.NbParam = *InvParamList.NbParam + 1;
      String* ntParName = new String[*InvParamList.NbParam];
      String* ntName = new String[*InvParamList.NbParam];
      String* ntCmd = new String[*InvParamList.NbParam];
      String* ntModule = new String[*InvParamList.NbParam];
      float* ntConv = new float[*InvParamList.NbParam];
      String* ntUnit = new String[*InvParamList.NbParam];
      ntParName[0] = InvParamList.ParName[0];
      ntName[0]    = InvParamList.Name[0];
      ntCmd[0]     = InvParamList.Cmd[0];
      ntModule[0]  = InvParamList.Module[0]; 
      ntConv[0]    = InvParamList.Conv[0];
      ntUnit[0]    = InvParamList.Unit[0];
      ntParName[1] = "PvArT";
      ntName[1]    = "PV array temperature";
      ntCmd[1]     = "0204";
      ntModule[1]  = "08"; 
      ntConv[1]    = 10.0f;
      ntUnit[1]    = "°C";
      for (int i = 2; i < *InvParamList.NbParam; i++) {
        ntParName[i] = InvParamList.ParName[i-1];
        ntName[i]    = InvParamList.Name[i-1];
        ntCmd[i]     = InvParamList.Cmd[i-1];
        ntModule[i]  = InvParamList.Module[i-1]; 
        ntConv[i]    = InvParamList.Conv[i-1];
        ntUnit[i]    = InvParamList.Unit[i-1];
      }
      delete[] InvParamList.ParName;
      delete[] InvParamList.Name;
      delete[] InvParamList.Cmd;
      delete[] InvParamList.Module; 
      delete[] InvParamList.Conv;
      delete[] InvParamList.Unit;
      InvParamList.ParName = ntParName;
      InvParamList.Name = ntName;
      InvParamList.Cmd = ntCmd;
      InvParamList.Module = ntModule; 
      InvParamList.Conv = ntConv;
      InvParamList.Unit = ntUnit;
    }

    if ((*OptionByte & 4) == 4) {
      AmbTempS = true;
      *InvParamList.NbParam += 1;
      String* ntParName = new String[*InvParamList.NbParam];
      String* ntName = new String[*InvParamList.NbParam];
      String* ntCmd = new String[*InvParamList.NbParam];
      String* ntModule = new String[*InvParamList.NbParam];
      float* ntConv = new float[*InvParamList.NbParam];
      String* ntUnit = new String[*InvParamList.NbParam];
      ntParName[0] = InvParamList.ParName[0];
      ntName[0]    = InvParamList.Name[0];
      ntCmd[0]     = InvParamList.Cmd[0];
      ntModule[0]  = InvParamList.Module[0]; 
      ntConv[0]    = InvParamList.Conv[0];
      ntUnit[0]    = InvParamList.Unit[0];
      ntParName[1] = "AmbT";
      ntName[1]    = "Ambient temperature";
      ntCmd[1]     = "0203";
      ntModule[1]  = "08"; 
      ntConv[1]    = 10.0f;
      ntUnit[1]    = "°C";
      for (int i = 2; i < *InvParamList.NbParam; i++) {
        ntParName[i] = InvParamList.ParName[i-1];
        ntName[i]    = InvParamList.Name[i-1];
        ntCmd[i]     = InvParamList.Cmd[i-1];
        ntModule[i]  = InvParamList.Module[i-1]; 
        ntConv[i]    = InvParamList.Conv[i-1];
        ntUnit[i]    = InvParamList.Unit[i-1];
      }
      delete[] InvParamList.ParName;
      delete[] InvParamList.Name;
      delete[] InvParamList.Cmd;
      delete[] InvParamList.Module; 
      delete[] InvParamList.Conv;
      delete[] InvParamList.Unit;
      InvParamList.ParName = ntParName;
      InvParamList.Name = ntName;
      InvParamList.Cmd = ntCmd;
      InvParamList.Module = ntModule; 
      InvParamList.Conv = ntConv;
      InvParamList.Unit = ntUnit;
      
    }

    if ((*OptionByte & 8) == 8) {
      IRSTemp = true;
      *InvParamList.NbParam += 1;
      String* ntParName = new String[*InvParamList.NbParam];
      String* ntName = new String[*InvParamList.NbParam];
      String* ntCmd = new String[*InvParamList.NbParam];
      String* ntModule = new String[*InvParamList.NbParam];
      float* ntConv = new float[*InvParamList.NbParam];
      String* ntUnit = new String[*InvParamList.NbParam];
      ntParName[0] = InvParamList.ParName[0];
      ntName[0]    = InvParamList.Name[0];
      ntCmd[0]     = InvParamList.Cmd[0];
      ntModule[0]  = InvParamList.Module[0]; 
      ntConv[0]    = InvParamList.Conv[0];
      ntUnit[0]    = InvParamList.Unit[0];
      ntParName[1] = "IRsensT";
      ntName[1]    = "Irradiance sensor temperature";
      ntCmd[1]     = "0205";
      ntModule[1]  = "08"; 
      ntConv[1]    = 10.0f;
      ntUnit[1]    = "°C";
      for (int i = 2; i < *InvParamList.NbParam; i++) {
        ntParName[i] = InvParamList.ParName[i-1];
        ntName[i]    = InvParamList.Name[i-1];
        ntCmd[i]     = InvParamList.Cmd[i-1];
        ntModule[i]  = InvParamList.Module[i-1]; 
        ntConv[i]    = InvParamList.Conv[i-1];
        ntUnit[i]    = InvParamList.Unit[i-1];
      }
      delete[] InvParamList.ParName;
      delete[] InvParamList.Name;
      delete[] InvParamList.Cmd;
      delete[] InvParamList.Module; 
      delete[] InvParamList.Conv;
      delete[] InvParamList.Unit;
      InvParamList.ParName = ntParName;
      InvParamList.Name = ntName;
      InvParamList.Cmd = ntCmd;
      InvParamList.Module = ntModule; 
      InvParamList.Conv = ntConv;
      InvParamList.Unit = ntUnit;
      
    }

    if ((*OptionByte & 16) == 16) {            
      IRS = true;
      *InvParamList.NbParam += 2;
      String* ntParName = new String[*InvParamList.NbParam];
      String* ntName = new String[*InvParamList.NbParam];
      String* ntCmd = new String[*InvParamList.NbParam];
      String* ntModule = new String[*InvParamList.NbParam];
      float* ntConv = new float[*InvParamList.NbParam];
      String* ntUnit = new String[*InvParamList.NbParam];
      ntParName[0] = InvParamList.ParName[0];
      ntName[0]    = InvParamList.Name[0];
      ntCmd[0]     = InvParamList.Cmd[0];
      ntModule[0]  = InvParamList.Module[0]; 
      ntConv[0]    = InvParamList.Conv[0];
      ntUnit[0]    = InvParamList.Unit[0];
      ntParName[1] = "GIR";
      ntName[1]    = "Global irradiance";
      ntCmd[1]     = "0202";
      ntModule[1]  = "08"; 
      ntConv[1]    = 1000.0f;
      ntUnit[1]    = "W/m²";
      ntParName[2] = "TotalGIR";
      ntName[2]    = "Total global irradiation";
      ntCmd[2]     = "0206";
      ntModule[2]  = "08"; 
      ntConv[2]    = 1000.0f;
      ntUnit[2]    = "Wh/m²";
      for (int i = 3; i < *InvParamList.NbParam; i++) {
        ntParName[i] = InvParamList.ParName[i-2];
        ntName[i]    = InvParamList.Name[i-2];
        ntCmd[i]     = InvParamList.Cmd[i-2];
        ntModule[i]  = InvParamList.Module[i-2]; 
        ntConv[i]    = InvParamList.Conv[i-2];
        ntUnit[i]    = InvParamList.Unit[i-2];
      }
      delete[] InvParamList.ParName;
      delete[] InvParamList.Name;
      delete[] InvParamList.Cmd;
      delete[] InvParamList.Module; 
      delete[] InvParamList.Conv;
      delete[] InvParamList.Unit;
      InvParamList.ParName = ntParName;
      InvParamList.Name = ntName;
      InvParamList.Cmd = ntCmd;
      InvParamList.Module = ntModule; 
      InvParamList.Conv = ntConv;
      InvParamList.Unit = ntUnit;
    }

    if ((*OptionByte & 32) == 32) {
      EM = true;
      *InvParamList.NbParam += 1;
      String* ntParName = new String[*InvParamList.NbParam];
      String* ntName = new String[*InvParamList.NbParam];
      String* ntCmd = new String[*InvParamList.NbParam];
      String* ntModule = new String[*InvParamList.NbParam];
      float* ntConv = new float[*InvParamList.NbParam];
      String* ntUnit = new String[*InvParamList.NbParam];
      ntParName[0] = InvParamList.ParName[0];
      ntName[0]    = InvParamList.Name[0];
      ntCmd[0]     = InvParamList.Cmd[0];
      ntModule[0]  = InvParamList.Module[0]; 
      ntConv[0]    = InvParamList.Conv[0];
      ntUnit[0]    = InvParamList.Unit[0];
      ntParName[1] = "GridEdayEMet";
      ntName[1]    = "Grid Energy Today, ext meter";
      ntCmd[1]     = "024B";
      ntModule[1]  = "08"; 
      ntConv[1]    = 1000.0f;
      ntUnit[1]    = "kWh";
      for (int i = 2; i < *InvParamList.NbParam; i++) {
        ntParName[i] = InvParamList.ParName[i-1];
        ntName[i]    = InvParamList.Name[i-1];
        ntCmd[i]     = InvParamList.Cmd[i-1];
        ntModule[i]  = InvParamList.Module[i-1]; 
        ntConv[i]    = InvParamList.Conv[i-1];
        ntUnit[i]    = InvParamList.Unit[i-1];
      }
      delete[] InvParamList.ParName;
      delete[] InvParamList.Name;
      delete[] InvParamList.Cmd;
      delete[] InvParamList.Module; 
      delete[] InvParamList.Conv;
      delete[] InvParamList.Unit;
      InvParamList.ParName = ntParName;
      InvParamList.Name = ntName;
      InvParamList.Cmd = ntCmd;
      InvParamList.Module = ntModule; 
      InvParamList.Conv = ntConv;
      InvParamList.Unit = ntUnit;
    }
  }
  InvParamList.Meas     = new float[*InvParamList.NbParam];
  InvParamList.Raw      = new long[*InvParamList.NbParam];
  for (int i=0; i<*InvParamList.NbParam; i++){
    InvParamList.Meas[i] = 0.0f;
    InvParamList.Raw[i] = 0.0f;
  }
  
  GetStatus();
  GetName();
}

DanfossULX::DanfossULX(ComLynx *ComlynxInv, const String ComlynxAddr, String *InvSN, String *InvPN) : DanfossInverter(ComlynxInv, ComlynxAddr, InvSN, InvPN)
{
  InvParamList.NbParam  = new int;
  *InvParamList.NbParam = 14;
  InvParamList.Meas     = new float[*InvParamList.NbParam];
  InvParamList.Raw      = new long[*InvParamList.NbParam];
  for (int i=0; i<*InvParamList.NbParam; i++){
    InvParamList.Meas[i] = 0.0f;
    InvParamList.Raw[i] = 0.0f;
  }
  Type = "ULX";
  DefaultModuleID = "0D";    // default Module ID to get status, serial ...
  NameModuleID    = "04";    // Module ID to get name

  NbINV_power_modes = new int;
  *NbINV_power_modes = 4;
  INV_modes_txt = new String[*NbINV_power_modes] {
    "OFF             ", // The inverter is off
    "STANDBY         ", // There is insufficient irradiation to begin the grid connection process.
    "Connecting      ", // The inverter is monitoring the grid, preparing to connect.
    "Grid            " }; // The inverter is connected to grid and is producing energy.

  InvParamList.ParName = new String[*InvParamList.NbParam] { "InstantP",               "TotalE",                 "ProdTday",                "InstantE",                 "GridV",                "GridA",                   "HZ",                "PvV1",                "PvV2",                 "PvV3",                  "PvA1",                  "PvA2",                  "PvA3",                  "OpMode" };
  InvParamList.Name = new String[*InvParamList.NbParam]    { "Instant Power",          "Total Energy Production","Energy Production Today", "Instant Energy Production","Grid voltage",         "Grid current",            "Grid frequency",    "PV Voltage, input 1", "PV Voltage, input 2",  "PV Voltage, input 3",   "PV Current, input 1",   "PV Current, input 2",   "PV Current, input 3",   "Operating Mode"};
  InvParamList.Cmd = new String[*InvParamList.NbParam]     { "0101",                   "0102",                   "0104",                    "0201",                     "0214",                 "0215",                    "0216",              "0228",                "0229",                 "022A",                  "022D",                  "022E",                  "022F",                  "0A02" };
  InvParamList.Module = new String[*InvParamList.NbParam]  { "04",                     "04",                     "04",                      "0D",                       "0D",                   "0D",                      "0D",                "0D",                  "0D",                   "0D",                    "0D",                    "0D",                    "0D",                    "04" };
  InvParamList.Conv = new float[*InvParamList.NbParam]     { 1000.0f,                  1000.0f,                  1000.0f,                   1000.0f,                    10.0f,                  1000.0f,                   1000.0f,             10.0f,                 10.0f,                  10.0f,                   1000.0f,                 1000.0f,                 1000.0f,                 1.0f};
  InvParamList.Unit = new String[*InvParamList.NbParam]    { "W",                      "kWh",                    "kWh",                     "W",                        "V",                    "A",                       "Hz",                "V",                   "V",                    "V",                     "A",                     "A",                     "A",                     ""};
  GetStatus();
  GetName();
}
