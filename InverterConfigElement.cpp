/*
    Name: Inverter Config element class
    
*/

#include "InverterConfigElement.h"

InverterConfigElement::InverterConfigElement(int _Id, String _InvType, String _ProductNumber, String _SerialNumber, String _Address, byte _OptionByte)
{
  Id = _Id;
  InvType = _InvType;
  ProductNumber = _ProductNumber;
  SerialNumber = _SerialNumber;
  Address = _Address;
  OptionByte = _OptionByte;
}
