/*
    Name: DanfossTLX-RS485   
    
*/

#include "Arduino.h"
#include "DanfossTLX-RS485.h"

DanfossTLX::DanfossTLX(const byte RXD, const byte TXD)
{
    for (int i=0; i<DATA_ENUMS; i++){
      TLX.Meas[i] = 0.0f;
      TLX.Raw[i] = 0.0f;
    }
    Serial2.begin(19200, SERIAL_8N1, RXD, TXD);
    FindInvAddr();
    GetStatus();
    GetAllParameters();
}

long DanfossTLX::GetInvData(String GetLocal){
    String RXData;
    TX_TLX(GetLocal);
    RXData = RX_TLX();
    if (RXData.length() >= 36+2){
     return HexToDec(RXData.substring(36, 36 + 2) + RXData.substring(34, 34 + 2) + RXData.substring(32, 32 + 2) + RXData.substring(30, 30 + 2));
    }else{
      Serial.println("ERR RX:" + String(RXData.length()) + " " );
    }
    return NOTVALID;
}

String DanfossTLX::RX_TLX(void) {
    const int RX_TIMEOUT=75;
    const int RX_LENGTH = 22; // complete message from inverter
    String RxBuffer;
    String RXData = "";
    unsigned long TimeNow = millis();
    
    delay(25);  // typical delay 51ms for full message. So wait for RX_LENGTH
    while(Serial2.available() < RX_LENGTH){
      delay(2);
      unsigned long t = millis() - TimeNow;
      if(t > RX_TIMEOUT){
        Serial.println("ERR RX Timeout:" + String(t) + " ms "  + String(Serial2.available()) );   
        break;
      }
    }
    //Serial.println("RX time :" + String(millis()-TimeNow) + " ms " + String(Serial2.available()) );   
     
    while (Serial2.available() > 0) {
      RxBuffer = String(Serial2.read(), HEX);
      if (RxBuffer.length() == 1)  RxBuffer = "0" + RxBuffer;
      RXData = RXData + RxBuffer;
      delay(2);
    }
    // Serial.print(" L:" + String(RXData.length() ) + " ");   
    RXData.toUpperCase();
    RXData.replace("7D5E", "7E");
    RXData.replace("7D5D", "7D");
    // Serial.print(">" + RXData + "<");   
    
    
    //Checksum ERROR IN CALC Possible 8bit/16bit issues ir just not understood the package and check ...
    /*
    if(RXData.length() >= 44  && RXData.length() <100){
      char ByteBuf[100]; // Buffer
      sprintf(ByteBuf,"%s",RXData.substring(1).c_str() );
      byte *byteptr = (byte *)&ByteBuf[2];               // Skip the first byte (2chars) Initially pFirstByte should point to the first byte to be included in 
                                                         // the checksum calculation, which is the second byte in the message, the Address field (0xFF).
                                                         // Exampler 7EFF03C6B100020A81C80D800A02453C00000090117E
 
      String fcs = pppfcs16(byteptr,18);                 //  12 + (10(0x0A) or 29(0x1D))  - 4 =>  18 || 37
      
      if (fcs == PPPGOODFCS16){
        Serial.println("Checksum OK");
      }else{
        Serial.println("Checksum error :"+ fcs +" "+ String(ByteBuf) + " Len" + String(RXData.length()));
      }
    }*/
    
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

void DanfossTLX::PrintAll(void)
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

void DanfossTLX::GetStatus(void)
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
void DanfossTLX::GetParameters(void){
  static int i = TotalE;
  
  long TmpRaw = GetInvData(TLX.Cmd[i]);
  if (TmpRaw == NOTVALID){
    Serial.println(TLX.ParName[i] + " Raw Not Valid");
  }else{
    TLX.Raw[i] = TmpRaw;
    if(SanityCheck(i)){
      TLX.Meas[i] =float(TLX.Raw[i]) / TLX.Conv[i];
    }
  }

  if (i == OpMode){
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
  i++;
  if(i >= DATA_ENUMS){
    i = TotalE;
  }
}
void DanfossTLX::GetAllParameters(void){

  for (int i = 0; i <DATA_ENUMS; i++) {
    long TmpRaw = GetInvData(TLX.Cmd[i]);
    if (TmpRaw == NOTVALID){
      Serial.println(TLX.ParName[i] + " Raw Not Valid");
    }else{
      TLX.Raw[i] = TmpRaw;
      if(SanityCheck(i)){
        TLX.Meas[i] =float(TLX.Raw[i]) / TLX.Conv[i];
      }
    }
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

bool DanfossTLX::SanityCheck(int i){
    // Santity check on valves
    // Only GridDC? can be negative
    if ( !(i>=GridDC1 && i<=GridDC3)  && TLX.Raw[i] < 0 ){
      Serial.println("Neg. value" + TLX.ParName[i] + " Raw " + TLX.Raw[i]);
      TLX.Raw[i]= 0;
      return false;
    }                                 
    
    if (  i==TotalE  && float(TLX.Raw[i]) > 100000000.0 ){
      Serial.println("1)To Big " + TLX.ParName[i] + " Raw " + TLX.Raw[i]);
      TLX.Raw[i]= 0;
      return false;
    }
    
    if ( i==ProdTyear && float(TLX.Raw[i]) > 10000000.0 ){ //10.000,000 10.000kWh
      Serial.println("2)To Big " + TLX.ParName[i] + " Raw " + TLX.Raw[i]);
      TLX.Raw[i]= 0;
      return false;
    }
    if (  !(i==ProdTyear || i==TotalE) && float(TLX.Raw[i]) > 64000.0 ){ // i,e 64 kWh - big enough for all other val
      Serial.println("3)To Big " + TLX.ParName[i] + " Raw " + TLX.Raw[i]);
      TLX.Raw[i]= 0;
      return false;
    } 
  return true;
}
