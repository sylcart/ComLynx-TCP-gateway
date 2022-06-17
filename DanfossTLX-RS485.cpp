/*
    Name: DanfossTLX-RS485   
    
*/

#include "Arduino.h"
#include "DanfossTLX-RS485.h"

DanfossTLX::DanfossTLX(const byte RXD, const byte TXD)
{
    Serial2.begin(19200, SERIAL_8N1, RXD, TXD);
    FindInvAddr();
    TLXGetStatus();
    TLXGetParameters();
}

long DanfossTLX::GetInvData(String GetLocal){
    String RXData;
    TX_TLX(GetLocal);
    RXData = RX_TLX();
    return HexToDec(RXData.substring(36, 36 + 2) + RXData.substring(34, 34 + 2) + RXData.substring(32, 32 + 2) + RXData.substring(30, 30 + 2));
}

String DanfossTLX::RX_TLX(void) {
    const int RX_TIMEOUT=50;
    const int RX_LENGTH =2;
    String RxBuffer;
    String RXData = "";
    unsigned long TimeNow = millis();
    
    delay(15);  // 25 was not enough. typical delay 28-31ms. So wait for RX_LENGTH;
    while(Serial2.available() < RX_LENGTH){
      delay(1);
      if(millis() - TimeNow >RX_TIMEOUT)break;
    }
    // delay(5);
    // Serial.print("RX time :" + String(millis()-TimeNow) + " ms " );   
     
    while (Serial2.available() > 0) {
      RxBuffer = String(Serial2.read(), HEX);
      if (RxBuffer.length() == 1)  RxBuffer = "0" + RxBuffer;
      RXData = RXData + RxBuffer;
    }
    // Serial.print(" Length :" + String(RxBuffer.length() ) + " ");   
    RXData.toUpperCase();
    RXData.replace("7D5E", "7E");
    RXData.replace("7D5D", "7D");
    //\\Serial.print(">" + RXData + "<");   
    //\\ If RXdata does not look right then set ERROR flag and try to reinitialize Serial2.
   
    return RXData;
}

void DanfossTLX::FindInvAddr(void) 
{
    delay(1000);
    TX_TLX(GetAddr);
    RS485_DstAddr = RX_TLX().substring(6, 6 + 4);
    Serial.println("RS485_DstAddr               :"+RS485_DstAddr);
}

void DanfossTLX::TX_TLX(String TXLocal) 
{
    int x;
    TXData = RS485_StartFrame + RS485_SrcAddr + RS485_DstAddr + PreGet + TXLocal + PostGet;
    TXData = RS485_StartStop + TXData + pppfcs16(DataAsc, ChrHex2Asc()) + RS485_StartStop;
//    Serial.println(TXData + " Send ");
    for (x = 0; x < TXData.length(); x = x + 2) {
     Serial2.write(HexToDec(TXData.substring(x, x + 2)));
    }
}

unsigned int DanfossTLX::ChrHex2Asc(void) 
{
    int y = 0;
    for (int x = 0; x < TXData.length(); x = x + 2) {
        DataAsc[y] = HexToDec(TXData.substring(x, x + 2));
        y++;
    }
    return y;
}

String DanfossTLX::pppfcs16(byte* pByte, int length)
{
    int fcs = 0xFFFF;
    while (length--)
        fcs = (fcs >> 8) ^ FCSTable[(fcs ^ *pByte++) & 0xff];
    fcs ^= 0xFFFF;
    String str_fcs = String(fcs, HEX);
    str_fcs = str_fcs.substring(2, 4) + str_fcs.substring(0, 2);
    return str_fcs;
}

unsigned int DanfossTLX::HexToDec(String hexString) 
{
    unsigned int decValue = 0;
    int nextInt;
    for (int i = 0; i < hexString.length(); i++) {
        nextInt = int(hexString.charAt(i));
        if (nextInt >= 48 && nextInt <= 57) nextInt = map(nextInt, 48, 57, 0, 9);
        if (nextInt >= 65 && nextInt <= 70) nextInt = map(nextInt, 65, 70, 10, 15);
        if (nextInt >= 97 && nextInt <= 102) nextInt = map(nextInt, 97, 102, 10, 15);
        nextInt = constrain(nextInt, 0, 15);
        decValue = (decValue * 16) + nextInt;
    }
    return decValue;
}

void DanfossTLX::TLXPrintAll(void)
{
  Serial.println("\nProduct#                                " + TLX.ProductNumber);
  Serial.println("Serial#                                 " + TLX.SerialNumber);
  Serial.println("Operation mode                          " + TLX.OpModeTxt );
  
  #define PRINTBUF 36
  char Buffer[PRINTBUF];
  for (int i = 0; i < DATA_ENUMS; i++) {
    Serial.println(LongString((Par_e)i));
  }
}

String DanfossTLX::LongString(Par_e ParEnum)
{
  // Fixed length text decription
  #define PRINTBUF 36
  char Buffer[PRINTBUF];
  memset(Buffer, 0, PRINTBUF);
  sprintf(Buffer,"%s", TLX.Name[ParEnum].c_str());
  for(int c = 0; c < PRINTBUF-1; c++){
    if (Buffer[c] == 0)Buffer[c]=0x20;
  }
    
  String txt = String(Buffer) +"\t" +  MeasString(ParEnum) +" " + TLX.Unit[ParEnum];
  return txt;
}

String DanfossTLX::MeasString(Par_e ParEnum)
{
  // Print with "right" number of digits based one units. Could have used Conv[] enstead
  String Meas = String(TLX.Meas[ParEnum],0); //Default
  if( TLX.Unit[ParEnum] == String("V")  )Meas = String(TLX.Meas[ParEnum],1);
  if( TLX.Unit[ParEnum] == String("A")  )Meas = String(TLX.Meas[ParEnum],3);
  if( TLX.Unit[ParEnum] == String("mA") )Meas = String(TLX.Meas[ParEnum],0);
  if( TLX.Unit[ParEnum] == String("W")  )Meas = String(TLX.Meas[ParEnum],0);
  if( TLX.Unit[ParEnum] == String("kWh"))Meas = String(TLX.Meas[ParEnum],3);
  if( TLX.Unit[ParEnum] == String("Hz" ))Meas = String(TLX.Meas[ParEnum],1);
  return Meas;
}

void DanfossTLX::TLXGetStatus(void)
{
  String RXData;
  TX_TLX("4603");
  RXData = RX_TLX();
  //Serial.println(">SerialNumber <");
  TX_TLX("3C03");
  RXData = RX_TLX();
  //Serial.println(">" + RXData + "<");
  int idx=0;
  TLX.SerialNumber.setCharAt(idx++,HexToDec(RXData.substring(36,36+2)));
  TLX.SerialNumber.setCharAt(idx++,HexToDec(RXData.substring(34,34+2)));
  TLX.SerialNumber.setCharAt(idx++,HexToDec(RXData.substring(32,32+2)));
  TLX.SerialNumber.setCharAt(idx++,HexToDec(RXData.substring(30,30+2)));
  TX_TLX("4602");
  RXData = RX_TLX();
  //Serial.println(">" + RXData + "<");
  TLX.SerialNumber.setCharAt(idx++,HexToDec(RXData.substring(36,36+2)));
  TLX.SerialNumber.setCharAt(idx++,HexToDec(RXData.substring(34,34+2)));
  TLX.SerialNumber.setCharAt(idx++,HexToDec(RXData.substring(32,32+2)));
  TLX.SerialNumber.setCharAt(idx++,HexToDec(RXData.substring(30,30+2)));
  TX_TLX("4601");
  RXData = RX_TLX();
  //Serial.println(">" + RXData + "<");
  TLX.SerialNumber.setCharAt(idx++,HexToDec(RXData.substring(36,36+2)));
  TLX.SerialNumber.setCharAt(idx++,HexToDec(RXData.substring(34,34+2)));
  TLX.SerialNumber.setCharAt(idx++,HexToDec(RXData.substring(32,32+2)));
  TLX.SerialNumber.setCharAt(idx++,HexToDec(RXData.substring(30,30+2)));
  TLX.SerialNumber.remove(idx,  TLX.SerialNumber.length()-(idx-1));
  idx = 0;
  do{ // Remove " "
    if(TLX.SerialNumber.charAt(idx) == 32 ){
      TLX.SerialNumber.remove(idx,1);
      idx = 0;
    }else{
      idx++;
    }
  }while(idx < TLX.SerialNumber.length());

  // Productnumber <> Varenummer
  //Serial.println(">ProductNumber <");
  TX_TLX("3C03");
  RXData = RX_TLX();
  //Serial.println(">" + RXData + "<");
  idx=0;
  TLX.ProductNumber.setCharAt(idx++,HexToDec(RXData.substring(36,36+2)));
  TLX.ProductNumber.setCharAt(idx++,HexToDec(RXData.substring(34,34+2)));
  TLX.ProductNumber.setCharAt(idx++,HexToDec(RXData.substring(32,32+2)));
  TLX.ProductNumber.setCharAt(idx++,HexToDec(RXData.substring(30,30+2)));
  TX_TLX("3C02");
  RXData = RX_TLX();
  //Serial.println(">" + RXData + "<");
  TLX.ProductNumber.setCharAt(idx++,HexToDec(RXData.substring(36,36+2)));
  TLX.ProductNumber.setCharAt(idx++,HexToDec(RXData.substring(34,34+2)));
  TLX.ProductNumber.setCharAt(idx++,HexToDec(RXData.substring(32,32+2)));
  TLX.ProductNumber.setCharAt(idx++,HexToDec(RXData.substring(30,30+2)));
  TX_TLX("3C01");
  RXData = RX_TLX();
  //Serial.println(">" + RXData + "<");
  TLX.ProductNumber.setCharAt(idx++,HexToDec(RXData.substring(36,36+2)));
  TLX.ProductNumber.setCharAt(idx++,HexToDec(RXData.substring(34,34+2)));
  TLX.ProductNumber.setCharAt(idx++,HexToDec(RXData.substring(32,32+2)));
  TLX.ProductNumber.setCharAt(idx++,HexToDec(RXData.substring(30,30+2)));
  TLX.ProductNumber.remove(idx,  TLX.ProductNumber.length()-(idx-1));
  idx = 0;
  do{
    if(TLX.ProductNumber.charAt(idx) == 32 ){
      TLX.ProductNumber.remove(idx,1);
      idx = 0;
    }else{
      idx++;
    }
  }while(idx < TLX.ProductNumber.length());
  Serial.println("Product#                                " + TLX.ProductNumber);
  Serial.println("Serial#                                 " + TLX.SerialNumber);
}

void DanfossTLX::TLXGetParameters(void){
  enum Mode_e{ OFFGRIDOFF, BOOT, CONNECT, ONGRID, FAILSAFE, OFFGRID, MODES }TLXMode;
  const char* TLX_modes_txt[MODES] = {
    "Off Grid, OFF   ", // Off Grid The complete inverter is shut down
    "Connecting, Boot", // The inverter is booting,initializing itself etc.
    "Connecting      ", // The inverter is monitoring the grid, preparing to connect.
    "On Grid         ", // The inverter is connected to grid and is producing energy.
    "Fail safe       ", // The inverter is disconnected from grid because of an error situation.
    "Off Grid, Comm  " };// The inverter is shutdown (except for theuser interface and the communication interfaces

  for (int i = 0; i <DATA_ENUMS; i++) {
    TLX.Raw[i] = GetInvData(TLX.Cmd[i]);
    TLX.Meas[i] =float(TLX.Raw[i]) / TLX.Conv[i];
  }

  TLXMode = MODES;
  if (TLX.Raw[OpMode] <= 9){
    TLXMode = OFFGRIDOFF;
  }else if(TLX.Raw[OpMode]>=10 && TLX.Raw[OpMode]<=49){
    TLXMode = BOOT;
  }else if(TLX.Raw[OpMode]>=50 && TLX.Raw[OpMode]<=59){
    TLXMode = CONNECT;
  }else if(TLX.Raw[OpMode]>=60 && TLX.Raw[OpMode]<=69){
    TLXMode = ONGRID;
  }else if(TLX.Raw[OpMode]>=70 && TLX.Raw[OpMode]<=79){
    TLXMode = FAILSAFE;
  }else if(TLX.Raw[OpMode]>=80 && TLX.Raw[OpMode]<=89){
    TLXMode = OFFGRID;
  }
  if (TLXMode < MODES){
    TLX.OpModeTxt = TLX_modes_txt[TLXMode];
    //Serial.println("Operation mode              :" + String(TLX_modes_txt[TLXMode]) + String(TLX.Raw[OpMode]) );   
  }else {
    TLX.OpModeTxt = "Mode not known";
    //Serial.println("Operation mode              :" + String("ERROR")+ String(TLX.Raw[OpMode]) );
  }
}
