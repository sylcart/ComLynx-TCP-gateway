/*
    Name: DanfossTLX-RS485   
    
    Based on TLX-ino by Torben https://github.com/TorbenNor/Danfoss-TLX

    Board LOLIN D32                                        
    https://www.aliexpress.com/item/32808551116.htm
      Pin 16  :RXD2 Seriel in
      Pin 17  :TXD2 Seriel out
    R411A01 mini 3.3V Auto RS485 to TTL232 Converter Board  
    https://www.aliexpress.com/item/32782552104.html
    
*/ 

class DanfossTLX {
  public:
    
    enum Par_e                      { TotalE,                   ProdTyear,                      ProdTday,                   PvV1,                 PvV2,                    PvV3,                    PvA1,                    PvA2,                    PvA3,                    HZ1,                     HZ2,                     HZ3,                     HZ,                      GridDC1,                 GridDC2,                 GridDC3,                 GridV1,                  GridV2,                  GridV3,                  GridV1Avg,               GridV2Avg,               GridV3Avg,               GridA1,                  GridA2,                  GridA3,                  GridP1,                  GridP2,                 GridP3,                  GridP,        GridEL1day,              GridEL2day,              GridEL3day,              GridEday,                OpMode,         DATA_ENUMS }p_e;
    struct TLX_s                                                                                  
    {                                                                                             
      String ParName[DATA_ENUMS] =  { "TotalE",                 "ProdTyear",                    "ProdTday",                 "PvV1",               "PvV2",                  "PvV3",                  "PvA1",                  "PvA2",                  "PvA3",                  "HZ1",                   "HZ2",                   "HZ3",                   "HZ",                    "GridDC1",               "GridDC2",               "GridDC3",               "GridV1",                "GridV2",                "GridV3",                "GridV1Avg",             "GridV2Avg",             "GridV3Avg",             "GridA1",                "GridA2",                "GridA3",                "GridP1",               "GridP2",               "GridP3",                "GridP",      "GridEL1day",            "GridEL2day",            "GridEL3day",              "GridEday",              "OpMode" };
      String Name[DATA_ENUMS] =     { "Total Energy Production","Inverter production this year","Inverter production today","PV Voltage, input 1","PV Voltage, input 2",   "PV Voltage, input 3",   "PV Current, input 1",   "PV Current, input 2",   "PV Current, input 2",   "Grid frequency, L1",    "Grid frequency, L2",   "Grid frequency, L3",     "Grid frequency, AVG",   "Grid current, DC L1",   "Grid current, DC L2",   "Grid current, DC L3",   "Grid voltage, L1",      "Grid voltage, L2",     "Grid voltage, L3","Grid voltage, L1 (10min)","Grid voltage, L2 (10min)","Grid voltage, L3 (10min)", "Grid current, L1",      "Grid current, L2",      "Grid current, L3",      "Grid power, L1",        "Grid power, L2", "Grid power, L3",               "Grid power","Grid Energy Today, L1 ", "Grid Energy Today, L2", "Grid Energy Today, L3", "Grid Energy Today",     "Operating Mode"};
      String Cmd[DATA_ENUMS] =      { "0102",                   "7832",                         "780A",                     "0228",               "0229",                  "022A",                  "022D",                  "022E",                  "022F",                  "0261",                  "0262",                 "0263",                   "0250",                  "024C",                  "024D",                  "024E",                  "023C",                  "023D",                  "023E",                  "025B",                  "025C",                  "025D",                  "023F",                  "0240",                  "0241",                  "0242",                  "0243",                  "0244",                  "0246",       "0247",                  "0248",                  "0249",                  "024A",                  "0A02" };
      float  Conv[DATA_ENUMS] =     { 1000.0f,                  1000.0f,                        1000.0f,                    10.0f,                10.0f,                   10.0f,                   1000.0f,                 1000.0f,                 1000.0f,                 1000.0f,                 1000.0f,                1000.0f,                  1000.0f,                  1.0f,                   1.0f,                    1.0f,                    10.0f,                   10.0f,                   10.0f,                   10.0f,                   10.0f,                   10.0f,                   1000.0f,                 1000.0f,                 1000.0f,                  1.0f,                   1.0f,                    1.0f,                    1.0f,         1000.0f,                 1000.0f,                 1000.0f,                 1000.0f,                 1.0f};
      String Unit[DATA_ENUMS] =     { "kWh",                    "kWh",                          "kWh",                      "V",                  "V",                     "V",                     "A",                     "A",                     "A",                     "Hz",                    "Hz",                   "Hz",                     "Hz",                    "mA",                    "mA",                    "mA",                    "V",                     "V",                     "V",                     "V",                     "V",                      "V",                    "A",                     "A",                     "A",                      "W",                    "W",                     "W",                     "W",          "kWh",                   "kWh",                   "kWh",                   "kWh",                    ""};
      float Meas[DATA_ENUMS];
      long  Raw[DATA_ENUMS];

      String ProductNumber = ">>NotInitialized<<";  
      String SerialNumber = ">>NotInitialized<<";  
      String OpModeTxt = ">>NotInitialized<<";
    }TLX;
    
    DanfossTLX(const byte RXD2, const byte TXD2);
    void TLXGetStatus(void);
    void TLXGetParameters(void);
    void TLXPrintAll(void);
    String LongString(Par_e ParEnum );
    String MeasString(Par_e ParEnum);

  private:
    const unsigned short FCSTable[256] =
      { 0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
        0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
        0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
        0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
        0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
        0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
        0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
        0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
        0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
        0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
        0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
        0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
        0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
        0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
        0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
        0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
        0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
        0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
        0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
        0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
        0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
        0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
        0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
        0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
        0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
        0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
        0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
        0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
        0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
        0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
        0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
        0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78 };

    String RS485_SrcAddr = "0002";      // Program adress
    String RS485_StartFrame = "FF03";
    String RS485_StartStop = "7E";
    String RS485_DstAddr = "FFFF";      // Inverter Adress - as reported by findaddr
    String RS485_CRC    = "0000";
    String PreGet       = "0A01C808D0"; // For ULX Change to "0A01C804D0"
    String PostGet      = "8000000000"; 
    String GetAddr      = "0015";       // Ping Size 0x00,Type 0x15 
    String TXData ;
    byte   DataAsc[100]; // Buffer
    
    long    GetInvData(String GetLocal);
    void    FindInvAddr(void);
    String  RX_TLX(void);
    void    TX_TLX(String TXLocal);  
    String  pppfcs16(byte* pByte, int length);
    unsigned int HexToDec(String hexString);
    unsigned int ChrHex2Asc(void);
};
