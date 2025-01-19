/*
    Name: DanfossInverter 
*/

#include "DanfossInverter.h"
#include <vector>

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
  if(ParamIndex >= InvParamList.NbParam){
    ParamIndex = 0;
  }
}

void DanfossInverter::GetAllParameters(void){
  for (int i = 0; i < InvParamList.NbParam; i++) {
    GetOneParameter(i);
  }
}

void DanfossInverter::GetOneParameter(String ParameterName){
  for (int i = 0; i < InvParamList.NbParam; i++) {
    if (InvParamList.ParName[i] == ParameterName){
      GetOneParameter(i);
    }
  } 
}

void DanfossInverter::GetOneParameter(int i) {
  if (i < 0 || i >= InvParamList.NbParam) return;

  String GetParam = InvParamList.Module[i] + SourceModuleID + InvParamList.Cmd[i];
  long TmpRaw = _ComlynxInv->GetInvData(RS485_Addr, GetParam);
  if (TmpRaw == _ComlynxInv->NOTVALID) {
    Serial.println(InvParamList.ParName[i] + " Raw Not Valid");
    return;
  }

  if (InvParamList.ParName[i] == "OpMode") {
    int Inv_pw_mode = NbINV_power_modes;
    if (Type == "TLX") {
      if (InvParamList.Raw[i] <= 9) Inv_pw_mode = 0;
      else if (InvParamList.Raw[i] <= 49) Inv_pw_mode = 1;
      else if (InvParamList.Raw[i] <= 59) Inv_pw_mode = 2;
      else if (InvParamList.Raw[i] <= 69) Inv_pw_mode = 3;
      else if (InvParamList.Raw[i] <= 79) Inv_pw_mode = 4;
      else if (InvParamList.Raw[i] <= 89) Inv_pw_mode = 5;
    } else if (Type == "ULX") {
      switch (InvParamList.Raw[i]) {
        case 0: Inv_pw_mode = 0; break;
        case 4: Inv_pw_mode = 1; break;
        case 5: Inv_pw_mode = 2; break;
        case 6: Inv_pw_mode = 3; break;
      }
    }
    OpModeTxt = (Inv_pw_mode < NbINV_power_modes) ? INV_modes_txt[Inv_pw_mode] : "Mode not known";
  } else {
    InvParamList.Raw[i] = TmpRaw;
    InvParamList.Meas[i] = float(InvParamList.Raw[i]) / InvParamList.Conv[i];
    Serial.println(String(InvParamList.ParName[i]) + " (Raw) : " + String(InvParamList.Raw[i]));
    Serial.println(String(InvParamList.ParName[i]) + " : " + String(InvParamList.Meas[i]));
  }
}

int DanfossInverter::PrintParametersCount(void){
  return InvParamList.NbParam;
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
  if (InvParamList.Unit[index] == String("V")  ) Meas = String(InvParamList.Meas[index],1);
  if (InvParamList.Unit[index] == String("A")  ) Meas = String(InvParamList.Meas[index],3);
  if (InvParamList.Unit[index] == String("mA") ) Meas = String(InvParamList.Meas[index],0);
  if (InvParamList.Unit[index] == String("W")  ) Meas = String(InvParamList.Meas[index],0);
  if (InvParamList.Unit[index] == String("kWh")) Meas = String(InvParamList.Meas[index],3);
  if (InvParamList.Unit[index] == String("Hz" )) Meas = String(InvParamList.Meas[index],1);
  return Meas;
}

DanfossTLX::DanfossTLX(ComLynx *ComlynxInv, const String ComlynxAddr, String *InvSN, String *InvPN, byte *OptionByte) : DanfossInverter(ComlynxInv, ComlynxAddr, InvSN, InvPN)
{
  InvParamList.NbParam = 41;

  Type = "TLX";
  DefaultModuleID = "08";    // default Module ID to get status, serial ...
  NameModuleID    = "08";    // Module ID to get name

  NbINV_power_modes = 6;
  INV_modes_txt = {
    "Off Grid, OFF   ", // Off Grid The complete inverter is shut down
    "Connecting, Boot", // The inverter is booting,initializing itself etc.
    "Connecting      ", // The inverter is monitoring the grid, preparing to connect.
    "On Grid         ", // The inverter is connected to grid and is producing energy.
    "Fail safe       ", // The inverter is disconnected from grid because of an error situation.
    "Off Grid, Comm  "  // The inverter is shutdown (except for the user interface and the communication interfaces
  };

  InvParamList.ParName = { "TotalE",                 "PvV1",                "PvV2",                "PvV3",                "PvA1",                "PvA2",                "PvA3",                "PvPw1",            "PvPw2",            "PvPw3",            "PvE1",               "PvE2",               "PvE3",               "GridV1",           "GridV2",           "GridV3",           "GridV1Avg",                "GridV2Avg",                "GridV3Avg",                "GridVL1L2",             "GridVL2L3",             "GridVL3L1",             "GridA1",           "GridA2",           "GridA3",           "GridP1",         "GridP2",         "GridP3",         "GridP",       "GridEL1day",             "GridEL2day",            "GridEL3day",            "GridEday",          "GridDC1",             "GridDC2",             "GridDC3",             "HZ1",                "HZ2",                "HZ3",                "HZ",                  "OpMode" };
  InvParamList.Name =    { "Total Energy Production","PV Voltage, input 1", "PV Voltage, input 2", "PV Voltage, input 3", "PV Current, input 1", "PV Current, input 2", "PV Current, input 3", "PV Power input 1", "PV Power input 2", "PV Power input 3", "PV Energy, input 1", "PV Energy, input 2", "PV Energy, input 3", "Grid voltage, L1", "Grid voltage, L2", "Grid voltage, L3", "Grid voltage, L1 (10min)", "Grid voltage, L2 (10min)", "Grid voltage, L3 (10min)", "Grid voltage, L1 – L2", "Grid voltage, L2 – L3", "Grid voltage, L3 – L1", "Grid current, L1", "Grid current, L2", "Grid current, L3", "Grid power, L1", "Grid power, L2", "Grid power, L3", "Grid power",  "Grid Energy Today, L1 ", "Grid Energy Today, L2", "Grid Energy Today, L3", "Grid Energy Today", "Grid current, DC L1", "Grid current, DC L2", "Grid current, DC L3", "Grid frequency, L1", "Grid frequency, L2", "Grid frequency, L3", "Grid frequency, AVG", "Operating Mode"};
  InvParamList.Cmd =     { "0102",                   "0228",                "0229",                "022A",                "022D",                "022E",                "022F",                "0232",             "0233",             "0234",             "0237",               "0238",               "0239",               "023C",             "023D",             "023E",             "025B",                     "025C",                     "025D",                     "025E",                  "025F",                  "0260",                  "023F",             "0240",             "0241",             "0242",           "0243",           "0244",           "0246",        "0247",                   "0248",                  "0249",                  "024A",              "024C",                "024D",                "024E",                "0261",               "0262",               "0263",               "0250",                "0A02" };
  InvParamList.Module =  { "08",                     "08",                  "08",                  "08",                  "08",                  "08",                  "08",                  "08",               "08",               "08",               "08",                 "08",                 "08",                 "08",               "08",               "08",               "08",                       "08",                       "08",                       "08",                    "08",                    "08",                    "08",               "08",               "08",               "08",             "08",             "08",             "08",          "08",                     "08",                    "08",                    "08",                "08",                  "08",                  "08",                  "08",                 "08",                 "08",                 "08",                  "08" };
  InvParamList.Conv =    { 1000.0f,                  10.0f,                 10.0f,                 10.0f,                 1000.0f,               1000.0f,               1000.0f,               1000.0f,            1000.0f,            1000.0f,            1000.0f,              1000.0f,              1000.0f,              10.0f,              10.0f,              10.0f,              10.0f,                      10.0f,                      10.0f,                      10.0f,                   10.0f,                   10.0f,                   1000.0f,            1000.0f,            1000.0f,            1.0f,             1.0f,             1.0f,             1.0f,          1000.0f,                  1000.0f,                 1000.0f,                 1000.0f,             1.0f,                  1.0f,                  1.0f,                  1000.0f,              1000.0f,              1000.0f,              1000.0f,               1.0f };
  InvParamList.Unit =    { "kWh",                    "V",                   "V",                   "V",                   "A",                   "A",                   "A",                   "W",                "W",                "W",                "kWh",                "kWh",                "kWh",                "V",                "V",                "V",                "V",                        "V",                        "V",                        "V",                     "V",                     "V",                     "A",                "A",                "A",                "W",              "W",              "W",              "W",           "kWh",                    "kWh",                   "kWh",                   "kWh",               "mA",                  "mA",                  "mA",                  "Hz",                 "Hz",                 "Hz",                 "Hz",                  "" };

  if (OptionByte != nullptr) {
    if ((*OptionByte & 1) == 1) {
      HistData = true;
      InvParamList.NbParam += 4;
      InvParamList.ParName.insert(InvParamList.ParName.begin() + 1, {"ProdTday", "PThiWee", "PThiMth", "ProdTyear"});
      InvParamList.Name.insert(InvParamList.Name.begin() + 1, {"Inverter production today", "Production this Week", "Production this Month", "Inverter production this year"});
      InvParamList.Cmd.insert(InvParamList.Cmd.begin() + 1, {"780A", "7814", "781E", "7832"});
      InvParamList.Module.insert(InvParamList.Module.begin() + 1, {"08", "08", "08", "08"});
      InvParamList.Conv.insert(InvParamList.Conv.begin() + 1, {1000.0f, 1000.0f, 1000.0f, 1000.0f});
      InvParamList.Unit.insert(InvParamList.Unit.begin() + 1, {"kWh", "kWh", "kWh", "kWh"});
  // historical parameters could be added as an option in the future but i think that it should not be added to other parameters
  //   String ParName[HIST_ENUMS] = { "PLasMon",                "PLasTue",                 "PLasWed",                   "PLasThu",                  "PLasFri",                 "PLasSat",                  "PLasSun",                "PLasWee",              "P2Wago",                 "P3Wago",                 "P4Wago",                 "PLasJan",                 "PLasFeb",                  "PLasMar",               "PLasApr",               "PLasMay",             "PLasJun",              "PLasJul",              "PLasAug",                "PLasSep",                   "PLasOct",                 "PLasNov",                  "PLasDec",                  "PLasY",                "P2Yago",                 "P3Yago",                 "P4Yago",                 "P5Yago",                 "P6Yago",                 "P7Yago",                 "P8Yago",                 "P9Yago",                 "P10Yago",                 "P11Yago",                "P12Yago",                 "P13Yago",                 "P14Yago",                 "P15Yago",                 "P16Yago",                "P17Yago",                 "P18Yago",                 "P19Yago",                 "P20Yago",               };
  //   String Name[HIST_ENUMS]    = { "Production last Monday", "Production last Tuesday", "Production last Wednesday", "Production last Thursday", "Production last Friday",  "Production last Saturday", "Production last Sunday", "Production last Week", "Production 2 Weeks ago", "Production 3 Weeks ago", "Production 4 Weeks ago", "Production last January", "Production last February", "Production last March", "Production last April", "Production last May", "Production last June", "Production last July", "Production last August", "Production last September", "Production last October", "Production last November", "Production last December", "Production last Year", "Production 2 Years Ago", "Production 3 Years Ago", "Production 4 Years Ago", "Production 5 Years Ago", "Production 6 Years Ago", "Production 7 Years Ago", "Production 8 Years Ago", "Production 9 Years Ago", "Production 10 Years Ago", "Production 11 Years Ago","Production 12 Years Ago", "Production 13 Years Ago", "Production 14 Years Ago", "Production 15 Years Ago", "Production 16 Years Ago","Production 17 Years Ago", "Production 18 Years Ago", "Production 19 Years Ago", "Production 20 Years Ago"};
  //   String Cmd[HIST_ENUMS]     = { "780B",                   "780C",                    "780D",                      "780E",                     "780F",                    "7810",                     "7811",                   "7815",                 "7816",                   "7817",                   "7818",                   "781F",                    "7820",                     "7821",                  "7822",                  "7823",                "7824",                 "7825",                 "7826",                   "7827",                      "7828",                    "7829",                     "782A",                     "7833",                 "7834",                   "7835",                   "7836",                   "7837",                   "7838",                   "7839",                   "783A",                   "783B",                   "783C",                    "783D",                   "783E",                    "783F",                    "7840",                    "7841",                    "7842",                   "7843",                    "7844",                    "7845",                    "7846",                  };
  //   String Module[HIST_ENUMS]  = { "08",                     "08",                      "08",                        "08",                       "08",                      "08",                       "08",                     "08",                   "08",                     "08",                     "08",                     "08",                      "08",                       "08",                    "08",                    "08",                  "08",                   "08",                   "08",                     "08",                        "08",                      "08",                       "08",                       "08",                   "08",                     "08",                     "08",                     "08",                     "08",                     "08",                     "08",                     "08",                     "08",                      "08",                     "08",                      "08",                      "08",                      "08",                      "08",                     "08",                      "08",                      "08",                      "08",                    };
  //   float  Conv[HIST_ENUMS]    = { 1000.0f,                  1000.0f,                   1000.0f,                     1000.0f,                    1000.0f,                   1000.0f,                    1000.0f,                  1000.0f,                1000.0f,                  1000.0f,                  1000.0f,                  1000.0f,                   1000.0f,                    1000.0f,                 1000.0f,                 1000.0f,               1000.0f,                1000.0f,                1000.0f,                  1000.0f,                     1000.0f,                   1000.0f,                    1000.0f,                    1000.0f,                1000.0f,                  1000.0f,                  1000.0f,                  1000.0f,                  1000.0f,                  1000.0f,                  1000.0f,                  1000.0f,                  1000.0f,                   1000.0f,                  1000.0f,                   1000.0f,                   1000.0f,                   1000.0f,                   1000.0f,                  1000.0f,                   1000.0f,                   1000.0f,                   1000.0f,                 };
  //   String Unit[HIST_ENUMS]    = { "kWh",                    "kWh",                     "kWh",                       "kWh",                      "kWh",                     "kWh",                      "kWh",                    "kWh",                  "kWh",                    "kWh",                    "kWh",                    "kWh",                     "kWh",                      "kWh",                   "kWh",                   "kWh",                 "kWh",                  "kWh",                  "kWh",                    "kWh",                       "kWh",                     "kWh",                      "kWh",                      "kWh",                  "kWh",                    "kWh",                    "kWh",                    "kWh",                    "kWh",                    "kWh",                    "kWh",                    "kWh",                    "kWh",                     "kWh",                    "kWh",                     "kWh",                     "kWh",                     "kWh",                     "kWh",                    "kWh",                     "kWh",                     "kWh",                     "kWh",                   };
    }

    if ((*OptionByte & 2) == 2) {
      ModTempS = true;
      InvParamList.NbParam += 1;
      InvParamList.ParName.insert(InvParamList.ParName.begin() + 1, "PvArT");
      InvParamList.Name.insert(InvParamList.Name.begin() + 1, "PV array temperature");
      InvParamList.Cmd.insert(InvParamList.Cmd.begin() + 1, "0204");
      InvParamList.Module.insert(InvParamList.Module.begin() + 1, "08");
      InvParamList.Conv.insert(InvParamList.Conv.begin() + 1, 10.0f);
      InvParamList.Unit.insert(InvParamList.Unit.begin() + 1, "°C");
    }

    if ((*OptionByte & 4) == 4) {
      AmbTempS = true;
      InvParamList.NbParam += 1;
      InvParamList.ParName.insert(InvParamList.ParName.begin() + 1, "AmbT");
      InvParamList.Name.insert(InvParamList.Name.begin() + 1, "Ambient temperature");
      InvParamList.Cmd.insert(InvParamList.Cmd.begin() + 1, "0203");
      InvParamList.Module.insert(InvParamList.Module.begin() + 1, "08");
      InvParamList.Conv.insert(InvParamList.Conv.begin() + 1, 10.0f);
      InvParamList.Unit.insert(InvParamList.Unit.begin() + 1, "°C");
    }

    if ((*OptionByte & 8) == 8) {
      IRSTemp = true;
      InvParamList.NbParam += 1;
      InvParamList.ParName.insert(InvParamList.ParName.begin() + 1, "IRsensT");
      InvParamList.Name.insert(InvParamList.Name.begin() + 1, "Irradiance sensor temperature");
      InvParamList.Cmd.insert(InvParamList.Cmd.begin() + 1, "0205");
      InvParamList.Module.insert(InvParamList.Module.begin() + 1, "08");
      InvParamList.Conv.insert(InvParamList.Conv.begin() + 1, 10.0f);
      InvParamList.Unit.insert(InvParamList.Unit.begin() + 1, "°C");
    }

    if ((*OptionByte & 16) == 16) {
      IRS = true;
      InvParamList.NbParam += 2;
      InvParamList.ParName.insert(InvParamList.ParName.begin() + 1, {"GIR", "TotalGIR"});
      InvParamList.Name.insert(InvParamList.Name.begin() + 1, {"Global irradiance", "Total global irradiation"});
      InvParamList.Cmd.insert(InvParamList.Cmd.begin() + 1, {"0202", "0206"});
      InvParamList.Module.insert(InvParamList.Module.begin() + 1, {"08", "08"});
      InvParamList.Conv.insert(InvParamList.Conv.begin() + 1, {1000.0f, 1000.0f});
      InvParamList.Unit.insert(InvParamList.Unit.begin() + 1, {"W/m²", "Wh/m²"});
    }

    if ((*OptionByte & 32) == 32) {
      EM = true;
      InvParamList.NbParam += 1;
      InvParamList.ParName.insert(InvParamList.ParName.begin() + 1, "GridEdayEMet");
      InvParamList.Name.insert(InvParamList.Name.begin() + 1, "Grid Energy Today, ext meter");
      InvParamList.Cmd.insert(InvParamList.Cmd.begin() + 1, "024B");
      InvParamList.Module.insert(InvParamList.Module.begin() + 1, "08");
      InvParamList.Conv.insert(InvParamList.Conv.begin() + 1, 1000.0f);
      InvParamList.Unit.insert(InvParamList.Unit.begin() + 1, "kWh");
    }
  }

  InvParamList.Meas.resize(InvParamList.NbParam, 0.0f);
  InvParamList.Raw.resize(InvParamList.NbParam, 0);

  GetStatus();
  GetName();
}

DanfossULX::DanfossULX(ComLynx *ComlynxInv, const String ComlynxAddr, String *InvSN, String *InvPN) : DanfossInverter(ComlynxInv, ComlynxAddr, InvSN, InvPN)
{
  InvParamList.NbParam = 14;

  Type = "ULX";
  DefaultModuleID = "0D";    // default Module ID to get status, serial ...
  NameModuleID    = "04";    // Module ID to get name
  NbINV_power_modes = 4;

  INV_modes_txt = {
    "OFF             ", // The inverter is off
    "STANDBY         ", // There is insufficient irradiation to begin the grid connection process.
    "Connecting      ", // The inverter is monitoring the grid, preparing to connect.
    "Grid            " }; // The inverter is connected to grid and is producing energy.

  InvParamList.ParName = { "InstantP",      "TotalE",                 "ProdTday",                "InstantE",                  "GridV",        "GridA",        "HZ",             "PvV1",                "PvV2",                "PvV3",                "PvA1",                "PvA2",                "PvA3",                "OpMode" };
  InvParamList.Name =    { "Instant Power", "Total Energy Production","Energy Production Today", "Instant Energy Production", "Grid voltage", "Grid current", "Grid frequency", "PV Voltage, input 1", "PV Voltage, input 2", "PV Voltage, input 3", "PV Current, input 1", "PV Current, input 2", "PV Current, input 3", "Operating Mode"};
  InvParamList.Cmd =     { "0101",          "0102",                   "0104",                    "0201",                      "0214",         "0215",         "0216",           "0228",                "0229",                "022A",                "022D",                "022E",                "022F",                "0A02" };
  InvParamList.Module =  { "04",            "04",                     "04",                      "0D",                        "0D",           "0D",           "0D",             "0D",                  "0D",                  "0D",                  "0D",                  "0D",                  "0D",                  "04" };
  InvParamList.Conv =    { 1000.0f,         1000.0f,                  1000.0f,                   1000.0f,                     10.0f,          1000.0f,        1000.0f,          10.0f,                 10.0f,                 10.0f,                 1000.0f,               1000.0f,               1000.0f,               1.0f};
  InvParamList.Unit =    { "W",             "kWh",                    "kWh",                     "W",                         "V",            "A",            "Hz",             "V",                   "V",                   "V",                   "A",                   "A",                   "A",                   ""};
  
  InvParamList.Meas.resize(InvParamList.NbParam, 0.0f);
  InvParamList.Raw.resize(InvParamList.NbParam, 0);
  
  GetStatus();
  GetName();
}
