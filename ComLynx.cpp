/*
    Name: ComLynx   
*/
#include "ComLynx.h"

ComLynx::ComLynx(const byte RXD, const byte TXD)
{
  Serial2.begin(19200, SERIAL_8N1, RXD, TXD); // communication speed of the Comlynx protocol is 19200 baud only (8 data bits, no parity bits, 1 stop bit)
}

long ComLynx::GetInvData(const String InvAddr, String GetLocal){
  String RXData;
  TX_INV(InvAddr, GetLocal);
  RXData = RX_INV(22);
  if (RXData.length() >= 36+2 && RXData.substring(14, 22) == CAN_Reply_ok){
    return HexToDec(RXData.substring(36, 36 + 2) + RXData.substring(34, 34 + 2) + RXData.substring(32, 32 + 2) + RXData.substring(30, 30 + 2));
  } else {
    if (RXData.length()>0) Serial.println("ERR RX:" + String(RXData.length()) + " ");
  }
  return NOTVALID;
}

String ComLynx::GetInvString(const String InvAddr, String GetLocal){
  String RXData;
  String RXReturn = "";
  TX_INV(InvAddr, GetLocal);
  RXData = RX_INV(22);
  if (RXData.length() >= 36+2 && RXData.substring(14, 22) == CAN_Reply_ok){
    for (int i=6;i >= 0; i = i - 2) {
      RXReturn += String(char(HexToDec(RXData.substring(30 + i,32 + i))));
    }
    return RXReturn;
  } else {
    if (RXData.length()>0) Serial.println("ERR RX:" + String(RXData.length()) + " ");
  }
  return "";
}

String ComLynx::RX_INV(int RX_LENGTH) {
  const int RX_TIMEOUT=100; // could be up to 100 ms with some ULX inverters
  String RxBuffer;
  String RXData = "";
  unsigned long TimeNow = millis();
  
  delay(25);  // typical delay 51ms for full message. So wait for RX_LENGTH
  while(Serial2.available() < RX_LENGTH){
    delay(2);
    unsigned long t = millis() - TimeNow;
    if (t > RX_TIMEOUT) {
      Serial.println("ERR RX Timeout:" + String(t) + " ms.");
      if (Serial2.available() > 0) Serial.println(String(Serial2.available()) + " bytes available for reading.");
      break;
    }
  }
      
  while (Serial2.available() > 0) {
    RxBuffer = String(Serial2.read(), HEX);
    if (RxBuffer.length() == 1)  RxBuffer = "0" + RxBuffer;
    RXData = RXData + RxBuffer;
    delay(2);
  }
  RXData.toUpperCase();
  RXData.replace("7D5E", "7E"); // remove byte stuffing
  RXData.replace("7D5D", "7D"); // remove byte stuffing
  if (RXData != "") {
    Serial.print(">" + RXData + "<"); 
    String RXDataCRC = RXData.substring(2,RXData.length()-2);
    // Skip the first byte (2chars) Initially pFirstByte should point to the first byte to be included in the checksum calculation, 
    // which is the second byte in the message, the Address field (0xFF).
    // Example 7EFF03C6B100020A81C80D800A02453C00000090117E
    // last byte should also be skipped, payload ends after the 2 fsc bytes
    int fcs = pppfcs16(DataAsc,ChrHex2Asc(&RXDataCRC));
    if (fcs != PPPGOODFCS16){
      Serial.println("Checksum error :"+ String(fcs, HEX) + " Len" + String(RXData.length()));
      return "";
    }
  }
  return RXData;
}

void ComLynx::DiscoverInverters(std::vector<InverterConfigElement*> *_InverterList, boolean *Scanning, float *ScanPerc)
{
  String StrNet;
  String StrSubnet;
  String StrAdd;
  String PingReply;
  String NodeInformation;
  String NodeProductNumber;
  String NodeSerialNumber;
  String RS485_DstAddr;
  int InvId = 1;
  int idx;
  const int MaxInvCount = 64; // see comlynx documentation
  *Scanning = true;
  *ScanPerc = 0;
  Serial.println("Starting scanning bus.");
  TX_INV("","FFFF" + GetAddr);
  if (RX_INV(12) != "") {
    for (size_t i = 1; i < 15; i++) // slave networks are from 1 to 14
    {
      Serial.println("Scanning slave network : " + String(i));

      StrNet = String(i, HEX);
      TX_INV("",StrNet + "FFF" + GetAddr);
      if (RX_INV(12) != "") {
        for (size_t j = 0; j < 15; j++) // Subnet are from 0 to 14
        {
          Serial.println("Scanning subnet : " + String(j));
          StrSubnet = String(j, HEX);
          TX_INV("",StrNet + StrSubnet + "FF" + GetAddr);
          if (RX_INV(12) != "") {
            for (size_t k = 0; k < 255; k++) // nodes from 1 to 255
            {
              StrAdd = DecToHex(k);
              Serial.println("Scanning node : " + String(k)+ " with address :" + StrNet + StrSubnet + StrAdd);
              TX_INV("",StrNet + StrSubnet + StrAdd + GetAddr);
              PingReply = RX_INV(12);
              if (PingReply.substring(14, 18) == "0095") {
                TX_INV(StrNet + StrSubnet + StrAdd,"");
                NodeInformation = RX_INV(41);
                if (NodeInformation.substring(14, 18) == "1D93") {
                  Serial.println("Inv Id#                            " + String(InvId));
                  RS485_DstAddr = NodeInformation.substring(6, 10);
                  Serial.println("Inv Addr#                          " + RS485_DstAddr);
                  NodeProductNumber = "";
                  NodeSerialNumber = "";
                  for (size_t l = 0; l < 22; l = l + 2)
                  {
                    NodeProductNumber += String(char(HexToDec(NodeInformation.substring(18 + l ,20 + l))));
                    NodeSerialNumber += String(char(HexToDec(NodeInformation.substring(42 + l ,44 + l))));
                  }
                  NodeProductNumber.replace(" ", "");
                  Serial.println("ProductNumber#                          " + NodeProductNumber);
                  NodeSerialNumber.replace(" ", "");
                  Serial.println("SerialNumber#                          " + NodeSerialNumber);
                  if (NodeProductNumber == "A0025051201") {
                    _InverterList->push_back(new InverterConfigElement(InvId, "TLX", NodeProductNumber, NodeSerialNumber, RS485_DstAddr));
                  } else {
                    if (NodeProductNumber == "A0025008301") {
                      _InverterList->push_back(new InverterConfigElement(InvId, "ULX", NodeProductNumber, NodeSerialNumber, RS485_DstAddr));
                    } else {
                      Serial.println("Unknown ProductNumber# Inverter will not be stored " + NodeProductNumber);
                    }
                  }
                  InvId ++;
                  if (InvId > MaxInvCount) {
                    *Scanning = false;
                    return;
                  }
                }
              }
              *ScanPerc += 0.0018;
              vTaskDelay(10);
            }
          } else {
            *ScanPerc += 0.4761;
          }
          vTaskDelay(10);
        }
      } else {
        *ScanPerc += 7.1428;
      }
      vTaskDelay(10);
    }
  } else {
    Serial.println("Bus scanned without success no inverter found !");
  }
  *Scanning = false;
  *ScanPerc = 100;
}

void ComLynx::TX_INV(const String InvAddr, String TXLocal) 
{
  int x;
  int getmsgpadd;
  if (InvAddr != "" && TXLocal != "") {
    // this is a embedded CAN kingdom message
    TXData = RS485_StartFrame + RS485_SrcAddr + InvAddr + PreGet + TXLocal + PostGet;
  } else {
    if (InvAddr == "" && TXLocal != "") {
      // this is a ping message
      TXData = RS485_StartFrame + RS485_SrcAddr + TXLocal;
    } else {
      if (InvAddr != "" && TXLocal == "") {
        // this is a get node information message
        for (getmsgpadd=0; getmsgpadd < 29; getmsgpadd++) {
          GetMsg = GetMsg + "FF";
        }
        TXData = RS485_StartFrame + RS485_SrcAddr + InvAddr + GetMsg;
      } else {return;} // empty message nothing to do
    }
  }  
  TXData = TXData + fcsout(DataAsc, ChrHex2Asc(&TXData));
  TXData.toUpperCase();
  TXData = RS485_StartStop + OutStuffing(&TXData) + RS485_StartStop;
  Serial.println(TXData + " sent");
  for (x = 0; x < TXData.length(); x = x + 2) {
    Serial2.write(HexToDec(TXData.substring(x, x + 2)));
  }
}

unsigned int ComLynx::ChrHex2Asc(String *ChrString) 
{
  int y = 0;
  for (int x = 0; x < ChrString->length(); x = x + 2) {
      DataAsc[y] = HexToDec(ChrString->substring(x, x + 2));
      y++;
  }
  return y;
}

String ComLynx::OutStuffing(String *ChrStrStuff) 
{
  String Result = "";
  String StrByte;
  for (int x = 0; x < ChrStrStuff->length(); x = x + 2) {
    StrByte = ChrStrStuff->substring(x, x + 2);
    if (StrByte == "7E") {
        Result += "7D5E";
      } else {
        if (StrByte == "7D") {
          Result = Result + "7D5D";
        } else {
          Result = Result + StrByte;
        }
      }
    }
  return Result;
}

int ComLynx::pppfcs16(byte* pByte, int length)
{
  int fcs = 0xFFFF;
  while (length--) 
    fcs = (fcs >> 8) ^ FCSTable[(fcs ^ *pByte++) & 0xff];
  return fcs;
}

String ComLynx::fcsout(byte* pByte, int length)
{
  int fcs_out = pppfcs16(pByte,length);
  char dataString[4] = {0};
  fcs_out ^= 0xFFFF;
  String str_fcs;
  sprintf(dataString, "%04X",fcs_out);
  str_fcs = String(dataString[2]) + String(dataString[3] + String(dataString[0]) + String(dataString[1]));
  return str_fcs;
}

int ComLynx::HexToDec(String hexString) 
{
  int decValue = 0;
  int nextInt;
  for (int i = 0; i < hexString.length(); i++) {
      nextInt = int(hexString.charAt(i));
      if (nextInt >= 48 && nextInt <= 57) nextInt = map(nextInt, 48, 57, 0, 9);
      if (nextInt >= 65 && nextInt <= 70) nextInt = map(nextInt, 65, 70, 10, 15);
      if (nextInt >= 97 && nextInt <= 102) nextInt = map(nextInt, 97, 102, 10, 15);
      nextInt = constrain(nextInt, 0, 15);
      decValue = (decValue * 16) + nextInt;
  }
  if (decValue > 0x7FFFFFFF) {
    decValue -= 0x100000000;
  }
  return decValue;
}

String ComLynx::DecToHex(uint8_t decValue) 
{
  char buff[1] = {0};
  buff[0] = nibble_to_hex(decValue >> 4);
  buff[1] = nibble_to_hex(decValue);
  return String(buff[0]) + String(buff[1]);
}

char ComLynx::nibble_to_hex(uint8_t nibble) {  // convert a 4-bit nibble to a hexadecimal character
  nibble &= 0xF;
  return nibble > 9 ? nibble - 10 + 'A' : nibble + '0';
}