#ifndef InverterConfigElement_h
#define InverterConfigElement_h


#include "Arduino.h"

class InverterConfigElement {
  public:

    int Id;
    String InvType;
    String ProductNumber;
    String SerialNumber;
    String Address;
    byte   OptionByte;
    
    InverterConfigElement(int _Id, String _InvType, String _ProductNumber, String _SerialNumber, String _Address, byte _OptionByte = 0) ;
};

#endif