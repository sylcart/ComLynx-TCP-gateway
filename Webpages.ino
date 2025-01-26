/*
    Name: ComLynx-TCP-gateway webserver code
    Using library ESPAsyncWebServer at version 3.6.0
*/

enum Menu_e
{
  SysInfo,
  WifiSet,
  TCPSet,
  CLSet,
  MQTTSet,
  MPAGE_ENUMS
} m_e;
struct Menu_s
{
  String MParName[MPAGE_ENUMS] = {"SysInfo", "WifiSet", "TCPSet", "CLSet", "MQTTSet"};
  String MenuName[MPAGE_ENUMS] = {"System Info", "Wifi Settings", "TCP/IP Settings", "COMLYNX Settings", "MQTT Settings"};
  String PageName[MPAGE_ENUMS] = {"systeminfo.html", "wifi.html", "tcp.html", "comlynx.html", "mqtt.html"};
  String ScriptName[MPAGE_ENUMS] = {"", "", "/settings/TCPSetJS", "/settings/CLSetJS", "/settings/MQTTSetJS"};
  String FunctionName[MPAGE_ENUMS] = {"", "", "LoadParameters()", "Init()", "LoadParameters()"};
} PageMenu;


//File: favicon.ico.gz, Size: 2489
#define favicon_ico_gz_len 2489
const uint8_t favicon_ico_gz[] PROGMEM = {
  0x1f, 0x8b, 0x08, 0x08, 0x2a, 0x26, 0xb5, 0x65, 0x00, 0x03, 0x66, 0x61,
  0x76, 0x69, 0x63, 0x6f, 0x6e, 0x2e, 0x69, 0x63, 0x6f, 0x00, 0xed, 0x5a,
  0x0b, 0x70, 0x54, 0xe5, 0x15, 0xbe, 0x55, 0x3b, 0xd3, 0x19, 0xc6, 0x0a,
  0x65, 0x77, 0x03, 0x68, 0x4b, 0x80, 0x4a, 0xd1, 0x16, 0xa5, 0x56, 0x01,
  0x2d, 0x55, 0x6c, 0xb5, 0xda, 0x22, 0x76, 0xfa, 0x60, 0xda, 0x8a, 0x28,
  0xd5, 0x4a, 0x29, 0x8f, 0xec, 0x26, 0x51, 0x0c, 0x10, 0xc9, 0x14, 0xc2,
  0x43, 0xa8, 0xc6, 0x84, 0xa7, 0x04, 0x62, 0x23, 0x60, 0x0b, 0x6a, 0xa0,
  0x88, 0xc5, 0x81, 0x10, 0x20, 0x51, 0x5e, 0x0a, 0x14, 0x91, 0xa7, 0x50,
  0x01, 0x03, 0x11, 0x50, 0x92, 0x40, 0xb2, 0xf7, 0x7d, 0x4f, 0xcf, 0x39,
  0x77, 0xf7, 0xee, 0xde, 0xdd, 0xcd, 0xcd, 0x26, 0xd9, 0x3c, 0x5a, 0xf7,
  0xcc, 0x9c, 0xd9, 0xc7, 0xff, 0xff, 0xf7, 0x7c, 0xff, 0x7f, 0xcf, 0x7f,
  0x1e, 0xff, 0x7f, 0x04, 0xe1, 0x2b, 0xc2, 0xd5, 0x42, 0xe7, 0xce, 0x02,
  0x7e, 0xa6, 0x0a, 0x63, 0xaf, 0x11, 0x84, 0x81, 0x82, 0x20, 0xa4, 0xa6,
  0x9a, 0xbf, 0xfb, 0x75, 0x11, 0x84, 0x22, 0xfc, 0x6f, 0xc0, 0x80, 0x40,
  0x7b, 0x5f, 0x41, 0xd8, 0xe1, 0x16, 0x84, 0x7e, 0xd8, 0xa7, 0x33, 0xf5,
  0x13, 0xcc, 0xff, 0x5b, 0x9b, 0xea, 0x27, 0x76, 0x1d, 0xee, 0xf7, 0xb9,
  0xf3, 0xe2, 0xe2, 0x34, 0x77, 0x16, 0x3c, 0x2e, 0x7c, 0x2d, 0x38, 0xb6,
  0x36, 0xcd, 0x75, 0xa3, 0x3f, 0xdd, 0xa3, 0xc9, 0xcb, 0x1e, 0x03, 0xfd,
  0x3f, 0xbb, 0x41, 0x2d, 0x5b, 0x04, 0x62, 0x7a, 0x0a, 0x68, 0x87, 0x36,
  0x83, 0xfe, 0xe9, 0x01, 0x90, 0xfe, 0xfa, 0x13, 0x90, 0x5f, 0xfe, 0x3d,
  0x7f, 0x57, 0x77, 0xad, 0xe2, 0x36, 0xc9, 0xe7, 0xc9, 0x09, 0x8e, 0xc7,
  0xe7, 0x0d, 0xe1, 0xff, 0x5e, 0x1a, 0x06, 0xea, 0xb6, 0x25, 0x20, 0xaf,
  0xce, 0xe4, 0x3e, 0xca, 0xbf, 0xe6, 0xf0, 0x6f, 0x71, 0xc6, 0x1d, 0x20,
  0xbd, 0x70, 0x3f, 0x7f, 0x57, 0xde, 0x9c, 0x1c, 0x18, 0xef, 0x5e, 0x12,
  0x39, 0x9e, 0xc7, 0x94, 0x16, 0x40, 0xf0, 0xbb, 0x98, 0x73, 0x2b, 0x3e,
  0xeb, 0x69, 0xeb, 0xb7, 0xba, 0x39, 0x1f, 0x3f, 0xbb, 0x39, 0x8e, 0x57,
  0x3f, 0x78, 0x83, 0x65, 0x30, 0x6f, 0x9c, 0x0b, 0xda, 0x47, 0x9b, 0xac,
  0xdf, 0xfa, 0xe9, 0x7d, 0x20, 0x66, 0x74, 0x77, 0x1c, 0x2f, 0xcd, 0x1c,
  0x8c, 0xf3, 0xf8, 0x39, 0xb3, 0xbc, 0x7c, 0x34, 0xa8, 0xef, 0x15, 0x5b,
  0xbf, 0x89, 0xad, 0x7e, 0x0d, 0x8c, 0xb7, 0xda, 0x9f, 0xbf, 0x07, 0xa4,
  0x82, 0xe1, 0x20, 0xcd, 0xbb, 0x17, 0xe4, 0xa5, 0x23, 0x21, 0xaa, 0xdd,
  0x69, 0x7c, 0x56, 0x1f, 0x90, 0x8b, 0x46, 0x83, 0xe1, 0xaf, 0x05, 0x43,
  0x95, 0x41, 0xfb, 0xf7, 0x7a, 0x10, 0x9f, 0xe9, 0x19, 0xf7, 0x78, 0xed,
  0xf4, 0x7e, 0x50, 0x77, 0xff, 0x9d, 0xdf, 0x9b, 0x88, 0xf3, 0x91, 0x8b,
  0x9f, 0x02, 0xe3, 0xf2, 0x45, 0x90, 0xe6, 0x0e, 0x75, 0x1e, 0x8f, 0xeb,
  0xad, 0x1d, 0x78, 0x1b, 0xc7, 0xdd, 0xc7, 0xf3, 0x36, 0xea, 0x6b, 0xc0,
  0x30, 0x0c, 0xd0, 0xcf, 0x1d, 0xc6, 0xb5, 0x78, 0x1c, 0x94, 0x75, 0x39,
  0x20, 0xbf, 0x36, 0xb1, 0xe1, 0xf1, 0x19, 0x3d, 0xf8, 0x1d, 0xd3, 0x38,
  0x75, 0xf3, 0x4b, 0x20, 0xe5, 0xfd, 0x0c, 0xa4, 0x39, 0x3f, 0xe2, 0xf9,
  0x6b, 0x47, 0xb7, 0x81, 0x7e, 0xe1, 0x24, 0x48, 0xb3, 0xee, 0x8c, 0x1a,
  0x5f, 0x9f, 0xd6, 0xf5, 0x0e, 0xdb, 0xdc, 0x10, 0x37, 0xe1, 0xd7, 0xbf,
  0x38, 0x83, 0x6b, 0x50, 0x03, 0x7a, 0xe5, 0x41, 0x50, 0x36, 0xcc, 0x04,
  0xf1, 0xd9, 0x5e, 0x56, 0x1f, 0xd4, 0xe3, 0xfc, 0xe0, 0x78, 0xc0, 0x2d,
  0xe6, 0xf7, 0x7a, 0x16, 0xa0, 0x0e, 0x1f, 0x8e, 0x87, 0x45, 0x9f, 0x67,
  0x9b, 0x38, 0xe1, 0xba, 0x5e, 0xcd, 0xdf, 0x6d, 0x1d, 0x87, 0xc8, 0xce,
  0xa4, 0x22, 0x0f, 0x15, 0xda, 0xc6, 0xce, 0xfc, 0x3f, 0xd1, 0xd6, 0x7b,
  0x84, 0x6b, 0xfc, 0xe3, 0x3a, 0xf7, 0x4c, 0x24, 0xd7, 0x8f, 0xef, 0xda,
  0x23, 0x1e, 0xd9, 0x52, 0x9a, 0xeb, 0x21, 0xd4, 0xc3, 0xca, 0x48, 0xbb,
  0xd0, 0x52, 0xf6, 0xfb, 0x52, 0x0c, 0xd1, 0xeb, 0xd9, 0x8d, 0xcf, 0xee,
  0xdd, 0x90, 0xec, 0x6a, 0xd4, 0x7d, 0xbf, 0xcf, 0xe3, 0xb7, 0x6c, 0x6e,
  0xc9, 0x54, 0x50, 0xb7, 0x2c, 0xb0, 0x58, 0xca, 0x1f, 0x6e, 0x3d, 0x4f,
  0x79, 0x6b, 0x06, 0xa8, 0x3b, 0x57, 0x5a, 0x2c, 0x2f, 0x7d, 0x24, 0xd4,
  0x56, 0x92, 0x6d, 0x6f, 0x2b, 0xfa, 0x43, 0x08, 0x8b, 0xcf, 0x7d, 0x00,
  0xf7, 0xe5, 0x55, 0x31, 0xe7, 0x9e, 0xee, 0xc9, 0x0c, 0xc7, 0xac, 0x7f,
  0xfa, 0x21, 0x84, 0x13, 0xe1, 0xb1, 0xda, 0x4e, 0xee, 0xb2, 0xb7, 0xbd,
  0x3d, 0x2b, 0x64, 0xe7, 0x0e, 0x97, 0xda, 0xda, 0xd4, 0xd2, 0x7c, 0xdb,
  0x5a, 0xc8, 0xe9, 0x9e, 0xfe, 0xb1, 0xe4, 0xfb, 0xbd, 0xee, 0xc9, 0x6d,
  0x22, 0xdf, 0xe7, 0xba, 0x2d, 0x1e, 0xf9, 0xe2, 0xb4, 0xef, 0x81, 0xf8,
  0x97, 0xef, 0x87, 0x38, 0xab, 0x77, 0xa8, 0xed, 0xb9, 0xef, 0x46, 0xb4,
  0xf5, 0x09, 0xb5, 0x65, 0xdf, 0x6c, 0x6f, 0x9b, 0x7c, 0x63, 0xf3, 0xe4,
  0xb3, 0xcd, 0x45, 0x9b, 0xbf, 0xb7, 0x24, 0xa6, 0x4e, 0x91, 0x0d, 0x57,
  0xdf, 0x5f, 0x13, 0xbb, 0x6d, 0xee, 0xbd, 0xa0, 0xed, 0x5b, 0x6b, 0xf9,
  0xd5, 0xe6, 0xca, 0x97, 0x0b, 0x47, 0x82, 0xa1, 0xeb, 0x31, 0x65, 0xc8,
  0x0b, 0x7e, 0x09, 0x86, 0x22, 0xc5, 0x6e, 0x5b, 0xf2, 0x5b, 0x5e, 0x7b,
  0x31, 0xf3, 0xfa, 0xff, 0x79, 0xf9, 0x40, 0xbe, 0xf6, 0xc2, 0x89, 0x68,
  0xae, 0x3e, 0x6b, 0xfa, 0xe1, 0x98, 0x6d, 0xe7, 0x12, 0x22, 0x5f, 0xc2,
  0x18, 0x4d, 0x59, 0xfb, 0x5c, 0x4c, 0x56, 0x2b, 0x8a, 0xc0, 0xd0, 0xd4,
  0x06, 0xdb, 0x89, 0x5b, 0xfa, 0xfe, 0x9d, 0xd8, 0x69, 0xfd, 0x1d, 0xc7,
  0x25, 0x48, 0x3e, 0xc5, 0x2c, 0x6a, 0xf9, 0xf2, 0xb6, 0x95, 0x9f, 0xd9,
  0x03, 0x63, 0xb1, 0x0d, 0xe6, 0xde, 0xc2, 0x77, 0x63, 0xee, 0xc3, 0x21,
  0xa0, 0x1d, 0x29, 0x03, 0x65, 0xfd, 0xf4, 0xd6, 0x97, 0x9f, 0xdd, 0x0f,
  0x63, 0x4f, 0x89, 0x75, 0xcb, 0x30, 0x74, 0x30, 0xae, 0x5c, 0xe4, 0x4f,
  0x22, 0xfd, 0xcc, 0xfe, 0xd6, 0x93, 0x8f, 0x31, 0x9e, 0x34, 0xe7, 0x6e,
  0x73, 0xbe, 0xf9, 0xc3, 0x40, 0xff, 0x64, 0x0f, 0xef, 0x8b, 0x00, 0x10,
  0xd0, 0x8f, 0x57, 0x70, 0x8c, 0xce, 0xed, 0xb9, 0x83, 0x12, 0x2e, 0x5f,
  0x3b, 0xb8, 0x91, 0xf7, 0x9a, 0x5a, 0x5e, 0x08, 0xe2, 0xa4, 0x54, 0xf3,
  0x7f, 0xb4, 0xad, 0xd2, 0xfc, 0x5f, 0xe0, 0xe7, 0x6d, 0xe6, 0x6f, 0xb4,
  0xcf, 0xf4, 0x6e, 0xd8, 0x1f, 0xbc, 0x3e, 0x29, 0xa1, 0xf2, 0x95, 0x77,
  0xe6, 0x59, 0xf3, 0xd5, 0xab, 0x2b, 0xd9, 0xff, 0x52, 0x0e, 0x41, 0xb6,
  0x5e, 0x9a, 0x39, 0x08, 0xf3, 0x98, 0x2c, 0xd0, 0x2f, 0x55, 0x9a, 0xcb,
  0xa1, 0x29, 0x20, 0x2f, 0x1b, 0xd5, 0x72, 0xf9, 0x53, 0xfa, 0x9a, 0x7e,
  0x1f, 0xe3, 0x75, 0x1e, 0xf7, 0xea, 0x58, 0xcc, 0x17, 0x2e, 0x80, 0x13,
  0x19, 0xb5, 0xe7, 0x39, 0x27, 0xe4, 0xfe, 0x85, 0xa3, 0x30, 0x2f, 0x48,
  0x6b, 0xb6, 0x7c, 0x79, 0xc5, 0x9f, 0xcd, 0x67, 0xa2, 0xbe, 0x99, 0xbe,
  0x17, 0x73, 0xbc, 0x67, 0x7b, 0xe3, 0xf7, 0x29, 0x98, 0x8f, 0xee, 0xe1,
  0x1c, 0x88, 0xdb, 0xd1, 0x3e, 0x53, 0x2e, 0xa2, 0x6c, 0x40, 0x1f, 0x3c,
  0xf9, 0xdb, 0xdc, 0x4f, 0xd9, 0x38, 0x2f, 0xa0, 0x97, 0x06, 0x88, 0xd3,
  0x6f, 0x6f, 0xde, 0xfc, 0xd1, 0xa7, 0x6a, 0xc7, 0xb6, 0x5b, 0x73, 0x23,
  0x9d, 0x93, 0x17, 0xfe, 0x2a, 0x6c, 0x2f, 0xa2, 0x6d, 0xcd, 0xbe, 0xc9,
  0x96, 0x83, 0x51, 0x2e, 0xa5, 0x7d, 0xfc, 0x5e, 0xc8, 0xf7, 0xef, 0x78,
  0xb5, 0x65, 0xef, 0x1f, 0xf7, 0x3b, 0xe5, 0xcb, 0x64, 0xe7, 0x2c, 0x1c,
  0x9f, 0x1d, 0xe3, 0x9c, 0x57, 0x7e, 0xe5, 0x49, 0x90, 0x17, 0x8f, 0xe0,
  0xf5, 0x26, 0x5b, 0x4b, 0x58, 0x69, 0x2d, 0x82, 0x3a, 0x40, 0x79, 0x59,
  0xa2, 0xf4, 0x4f, 0xca, 0x1d, 0xc8, 0xbe, 0x9e, 0x9e, 0xeb, 0xf8, 0xfe,
  0x51, 0x47, 0xb5, 0xa3, 0x5b, 0x39, 0xcf, 0x6d, 0xb2, 0xfe, 0xf9, 0xdc,
  0xa3, 0x1a, 0xb5, 0x1f, 0xd3, 0xfa, 0x83, 0xf2, 0x46, 0x16, 0x68, 0xfb,
  0xd7, 0x83, 0x5e, 0x75, 0x14, 0x0c, 0xf4, 0x81, 0xfa, 0xe7, 0xa7, 0x40,
  0x3b, 0x5e, 0x0e, 0xea, 0xa6, 0x3c, 0x5b, 0x2e, 0x1b, 0x8b, 0x31, 0xbe,
  0x55, 0xeb, 0x7c, 0xae, 0xee, 0xb1, 0xe4, 0x7f, 0x30, 0x46, 0xf8, 0x2a,
  0xc6, 0xa7, 0xbb, 0xe2, 0xb1, 0x61, 0xcd, 0xe5, 0x7a, 0x9f, 0x6b, 0x56,
  0x2c, 0xd9, 0x41, 0x32, 0x32, 0x53, 0x3a, 0x21, 0xc6, 0x1c, 0x5c, 0x8b,
  0x92, 0x44, 0x32, 0xc6, 0xfd, 0x6b, 0xf0, 0xb9, 0x23, 0x9d, 0x64, 0x27,
  0x29, 0x49, 0x5f, 0x36, 0xa2, 0x73, 0x92, 0x01, 0xc8, 0xa3, 0x85, 0xe4,
  0x39, 0x49, 0x92, 0xda, 0x8f, 0x20, 0x47, 0xb8, 0xca, 0xc8, 0x14, 0x3a,
  0xb5, 0x07, 0x57, 0x3d, 0x2a, 0x74, 0x82, 0x11, 0xc2, 0xd5, 0xcd, 0xc2,
  0x8d, 0xdb, 0x46, 0x4e, 0xf3, 0xdc, 0x82, 0x3e, 0x26, 0x0b, 0xfd, 0x4b,
  0x51, 0xfb, 0xb0, 0x7b, 0xb9, 0xdf, 0xeb, 0x99, 0x56, 0xef, 0x4d, 0x19,
  0xd4, 0x54, 0xfc, 0x7c, 0x46, 0xed, 0xf3, 0x9c, 0xa6, 0xb8, 0xa0, 0x35,
  0xfd, 0xbe, 0x73, 0x4c, 0x92, 0x62, 0xa0, 0x7c, 0x0d, 0x63, 0x8f, 0x0b,
  0x62, 0x9a, 0x67, 0x6c, 0xbc, 0xd8, 0xeb, 0x9e, 0x72, 0x75, 0x47, 0xec,
  0x67, 0x6d, 0xcf, 0xc3, 0xfc, 0x4c, 0x2a, 0x78, 0x08, 0x24, 0xcc, 0x8d,
  0x6d, 0x4c, 0x67, 0x77, 0xc1, 0xbc, 0x25, 0xc8, 0x79, 0x0f, 0x82, 0xbc,
  0x72, 0x7c, 0x14, 0x4b, 0xcb, 0x1e, 0xb3, 0xf7, 0xc3, 0x18, 0x57, 0x7a,
  0xe5, 0x89, 0x98, 0x7d, 0xf9, 0xac, 0x29, 0xbc, 0xaf, 0xd7, 0xf3, 0x85,
  0xdf, 0xeb, 0xba, 0x3b, 0x1e, 0xfc, 0xa2, 0xcf, 0x35, 0x37, 0x2a, 0x36,
  0x44, 0xd9, 0x46, 0xdd, 0xa5, 0xe8, 0xb8, 0xb2, 0xbe, 0x1a, 0xa4, 0xd9,
  0x43, 0x6c, 0x7d, 0xd5, 0x77, 0x8b, 0x62, 0xc6, 0xa0, 0x14, 0x1f, 0xdb,
  0x62, 0x57, 0x9c, 0xa7, 0x11, 0x38, 0x0f, 0x89, 0x24, 0xba, 0x8b, 0x89,
  0x88, 0x0f, 0x75, 0x7c, 0x0f, 0x79, 0xe1, 0xf7, 0x60, 0x0d, 0xe2, 0x4f,
  0x77, 0x97, 0x77, 0x34, 0xfc, 0x26, 0xbb, 0xd7, 0xd5, 0xa4, 0x7f, 0xfd,
  0x1b, 0x8d, 0xe2, 0xf7, 0x7a, 0xde, 0xed, 0x88, 0xf8, 0x71, 0x2f, 0xaf,
  0x37, 0x9a, 0x89, 0x5f, 0x9a, 0xf7, 0x63, 0x50, 0xb7, 0x17, 0xf2, 0x7d,
  0xa4, 0x8d, 0x29, 0x6f, 0xcf, 0xb9, 0xc5, 0x3e, 0xd7, 0x95, 0xe3, 0xa2,
  0xfb, 0x21, 0x2b, 0x9b, 0x5e, 0xb4, 0x63, 0xa2, 0x33, 0xb0, 0xb2, 0x45,
  0x31, 0xfb, 0x8a, 0xb9, 0x83, 0x13, 0x8a, 0x9f, 0xcf, 0xcb, 0x9e, 0xfe,
  0x26, 0xe6, 0x94, 0x11, 0x4c, 0xff, 0x05, 0xee, 0x38, 0x2d, 0xc6, 0xdc,
  0x30, 0xaa, 0x1f, 0xf7, 0xbd, 0x21, 0x02, 0x53, 0x37, 0xf3, 0xbf, 0x58,
  0x7d, 0x23, 0x9f, 0xd9, 0x52, 0xfc, 0x41, 0x7e, 0xe6, 0x5b, 0x7c, 0x5e,
  0x20, 0x97, 0x64, 0x37, 0x6a, 0xff, 0xe8, 0xac, 0x98, 0xee, 0x0c, 0xa4,
  0x17, 0x1f, 0x70, 0xee, 0x4b, 0xcf, 0x2c, 0x1e, 0xd3, 0xe0, 0xb9, 0x62,
  0x42, 0xf1, 0x4f, 0xed, 0xc7, 0xe7, 0x59, 0x74, 0xbe, 0xd9, 0x18, 0x7e,
  0x65, 0xcd, 0x24, 0xce, 0xbb, 0x95, 0xb7, 0x72, 0x9d, 0xfb, 0x4e, 0xf9,
  0x0e, 0xe8, 0x9f, 0xbc, 0xcf, 0x7a, 0x4f, 0x73, 0x49, 0xe2, 0xff, 0x12,
  0xe0, 0xd7, 0x35, 0x90, 0x57, 0x8c, 0x75, 0x64, 0x75, 0xd7, 0x6b, 0x7c,
  0x56, 0xa5, 0xee, 0x5b, 0xeb, 0xdc, 0x77, 0x75, 0x06, 0xdf, 0x95, 0xb7,
  0x25, 0x7e, 0x3a, 0x73, 0x34, 0x64, 0xbf, 0x33, 0xab, 0x32, 0x9f, 0xfb,
  0xd0, 0xd9, 0x90, 0x63, 0x3f, 0xc5, 0x6f, 0x9d, 0x4f, 0xb5, 0x3a, 0xfe,
  0xac, 0x3e, 0xec, 0x07, 0xb4, 0xc3, 0x5b, 0x1a, 0x65, 0xbd, 0xea, 0x88,
  0x79, 0x8f, 0x72, 0xfe, 0x44, 0x5c, 0xfd, 0x89, 0xc5, 0xcc, 0x48, 0x1b,
  0x9b, 0x60, 0xfc, 0x4d, 0xe0, 0xb8, 0xf5, 0x3f, 0x4e, 0x4e, 0xe2, 0x6f,
  0x1a, 0xcb, 0xc5, 0x7f, 0xc2, 0x98, 0xe7, 0xb8, 0x55, 0xdf, 0xd4, 0xe1,
  0xf1, 0xd3, 0x5d, 0xca, 0xa2, 0xdf, 0x30, 0x73, 0x5c, 0x31, 0xa9, 0x27,
  0x9f, 0xab, 0x06, 0xeb, 0x68, 0xa4, 0xd9, 0x3f, 0x04, 0x89, 0xce, 0xb9,
  0x23, 0xf3, 0x85, 0x0e, 0x81, 0xbf, 0x1b, 0xfb, 0x7f, 0x8a, 0x51, 0x29,
  0x26, 0x55, 0x77, 0xae, 0x0a, 0x9d, 0xef, 0x23, 0x5e, 0xf9, 0x75, 0xd4,
  0xa5, 0xcb, 0xe7, 0xb9, 0x3e, 0x28, 0xfc, 0x4e, 0xbe, 0xc3, 0xe0, 0xc7,
  0xf5, 0x56, 0x77, 0x14, 0xb3, 0xbd, 0xb1, 0x62, 0x6b, 0xf4, 0x71, 0xfa,
  0xc5, 0x53, 0xa0, 0x5f, 0xf9, 0xdc, 0xba, 0x4b, 0x23, 0x62, 0x3b, 0xd3,
  0xd1, 0xf0, 0x93, 0xbe, 0x2f, 0x1e, 0xc1, 0xb5, 0x57, 0x0d, 0x12, 0xf9,
  0x82, 0xea, 0xb3, 0x1c, 0xff, 0x75, 0x08, 0xfc, 0x99, 0xd7, 0x83, 0xb4,
  0xe4, 0x77, 0xa1, 0x3a, 0xbd, 0x8c, 0x6e, 0x5c, 0x3f, 0xa1, 0xfc, 0x33,
  0x07, 0x8c, 0x9a, 0xaa, 0xd0, 0x3d, 0x22, 0x27, 0x2f, 0x3a, 0x68, 0x47,
  0xb6, 0x82, 0xf4, 0xc2, 0x4f, 0xcd, 0xf8, 0x9a, 0xfa, 0x53, 0x9d, 0x62,
  0xc1, 0xc3, 0x10, 0x2b, 0x56, 0x6e, 0x0b, 0xfc, 0x74, 0x87, 0x46, 0xf6,
  0x91, 0x58, 0xdd, 0xba, 0xd8, 0xac, 0xfd, 0x08, 0x62, 0xc9, 0xea, 0x0d,
  0xd2, 0xfc, 0x87, 0x41, 0xfe, 0x47, 0x06, 0xc8, 0x7f, 0xfb, 0x23, 0xe6,
  0x23, 0x03, 0x43, 0x63, 0xd1, 0xc7, 0x4a, 0x85, 0x8f, 0x82, 0x51, 0xfb,
  0x19, 0x4f, 0x2d, 0x58, 0xa3, 0xd9, 0xd6, 0xf8, 0xd5, 0xb2, 0x85, 0x36,
  0x5d, 0xa7, 0xbb, 0x6a, 0x8a, 0xe3, 0x23, 0xeb, 0x52, 0xc2, 0xf7, 0xb8,
  0x34, 0xfb, 0x2e, 0x50, 0x4a, 0xe7, 0x73, 0xad, 0xa3, 0xf5, 0x6a, 0xaa,
  0x8e, 0xc6, 0xf5, 0x0e, 0x12, 0xae, 0x3f, 0xb8, 0xa6, 0x7a, 0xe5, 0x21,
  0x4b, 0x4f, 0x38, 0xd6, 0x91, 0xea, 0x38, 0x5e, 0x50, 0xb6, 0x2c, 0x00,
  0x79, 0xe1, 0xaf, 0xb9, 0x9e, 0x52, 0x9c, 0x75, 0x17, 0xd7, 0x6b, 0xd0,
  0x9d, 0x9f, 0x5e, 0x5b, 0xc5, 0xb1, 0x9f, 0x35, 0x6f, 0xf1, 0x32, 0xdf,
  0xd7, 0xb5, 0xc9, 0xfa, 0xa3, 0x1d, 0xe4, 0x1a, 0xd3, 0xfc, 0x61, 0xf8,
  0x3d, 0x50, 0x1b, 0x99, 0x3b, 0x08, 0xd4, 0x83, 0xef, 0x98, 0xeb, 0x19,
  0xae, 0xef, 0x8d, 0x10, 0xcd, 0x81, 0xee, 0x56, 0xd9, 0x57, 0x04, 0xec,
  0x97, 0x84, 0xf3, 0x94, 0x9e, 0x1f, 0x6a, 0xaf, 0x47, 0x4a, 0x20, 0x7e,
  0xf2, 0xa3, 0x1c, 0x53, 0x22, 0x56, 0x75, 0xfb, 0x52, 0xae, 0x3f, 0xe5,
  0x36, 0xf4, 0x4f, 0xf2, 0x8a, 0x71, 0x5c, 0x7f, 0x4c, 0xeb, 0xef, 0x0c,
  0xdc, 0xe0, 0x1a, 0x18, 0xb5, 0x62, 0xb9, 0x79, 0x27, 0x48, 0x7b, 0x1e,
  0xb1, 0xd3, 0xdd, 0x21, 0xfd, 0x4f, 0xe3, 0x39, 0x8f, 0x8c, 0xca, 0x95,
  0x5b, 0x8e, 0x9f, 0x64, 0x86, 0xaf, 0x9f, 0x76, 0xee, 0x48, 0xc0, 0x7e,
  0x04, 0xfa, 0x50, 0xad, 0x16, 0xe5, 0xbc, 0x9b, 0xf2, 0x50, 0xaf, 0x3e,
  0xc2, 0xb9, 0x86, 0xee, 0x54, 0xe9, 0xbb, 0x7e, 0x6a, 0x2f, 0xdf, 0x71,
  0x8b, 0x33, 0x6e, 0x0f, 0xe4, 0xfd, 0x29, 0x9c, 0xef, 0x2a, 0x18, 0xcf,
  0x1a, 0x72, 0x7d, 0xc0, 0x77, 0x18, 0xa0, 0xee, 0x59, 0x1d, 0xf3, 0x1d,
  0xb4, 0x58, 0x7f, 0x72, 0x07, 0x83, 0x7e, 0xf6, 0x10, 0xef, 0x55, 0x0b,
  0x17, 0xfa, 0x5b, 0xaa, 0x9b, 0x96, 0xc2, 0x31, 0x59, 0xfa, 0xd6, 0xcb,
  0xf4, 0xc1, 0x54, 0x6b, 0x16, 0xd9, 0x46, 0x36, 0x08, 0xf5, 0x50, 0x3b,
  0xb1, 0xc3, 0xfe, 0x7a, 0xc8, 0x3f, 0xa0, 0x6d, 0x6a, 0xb5, 0xfd, 0x3b,
  0xfd, 0x07, 0xbc, 0x3e, 0x94, 0x73, 0xd8, 0x74, 0x02, 0xe3, 0x34, 0xb5,
  0xb4, 0x00, 0xe4, 0x97, 0x1f, 0x31, 0xef, 0xee, 0x1b, 0x1a, 0x9f, 0x7d,
  0x33, 0xd7, 0x34, 0x2a, 0xe5, 0xcb, 0x6c, 0x3e, 0x82, 0xf3, 0x84, 0x33,
  0x07, 0xcc, 0x7a, 0xc7, 0x06, 0xce, 0x20, 0x12, 0x66, 0x7f, 0x48, 0xdf,
  0xd1, 0x4e, 0x52, 0x4d, 0x45, 0xe4, 0x9e, 0x24, 0x7b, 0x62, 0x5c, 0xaa,
  0xe4, 0x18, 0x81, 0x6c, 0x90, 0xb2, 0x21, 0xd7, 0x64, 0xd4, 0x29, 0xaa,
  0x55, 0xe1, 0x1a, 0x69, 0xea, 0x13, 0x1e, 0x67, 0xe0, 0x38, 0x75, 0xef,
  0x9b, 0xbc, 0x36, 0x6d, 0x6a, 0x3f, 0xa7, 0xde, 0xc4, 0x75, 0xe3, 0x54,
  0x9f, 0x12, 0x6e, 0x13, 0xe3, 0x21, 0xb6, 0xb5, 0xaa, 0xc4, 0x79, 0xaf,
  0xbc, 0x6e, 0x5a, 0x54, 0xbd, 0x7b, 0x4b, 0xf0, 0xfb, 0x7d, 0xee, 0xb2,
  0xb8, 0xf0, 0xb3, 0xdd, 0xbb, 0x81, 0xcf, 0x31, 0xa9, 0x0e, 0x47, 0x3b,
  0xb9, 0x13, 0xed, 0xc7, 0x95, 0xc6, 0xb1, 0xa3, 0x8d, 0xa1, 0x5a, 0x10,
  0xaa, 0xd5, 0x11, 0x67, 0xde, 0x19, 0x9f, 0x1c, 0x62, 0x9f, 0x7b, 0x6d,
  0xcd, 0x93, 0x8d, 0xe3, 0xaf, 0xf3, 0xba, 0xa6, 0xc5, 0xfd, 0xcc, 0x20,
  0x93, 0xce, 0x4e, 0xe9, 0x8b, 0xb6, 0x65, 0x20, 0xdb, 0x52, 0xaa, 0x5f,
  0xd3, 0x8e, 0x55, 0x70, 0xad, 0x2b, 0xb1, 0x76, 0xbc, 0x82, 0xe3, 0x53,
  0x79, 0xd5, 0x04, 0xb3, 0x26, 0x9f, 0x7c, 0xb4, 0xc3, 0x59, 0x5b, 0xd4,
  0xda, 0xd3, 0xf9, 0xb9, 0xd7, 0x3d, 0x3d, 0x9e, 0xfb, 0xa4, 0x8b, 0x4f,
  0x08, 0xd7, 0xe2, 0x5c, 0x3f, 0xc4, 0x31, 0x46, 0x93, 0xe7, 0xd1, 0x4a,
  0x8c, 0x58, 0x3e, 0xae, 0x9b, 0xe0, 0xbe, 0xb5, 0x31, 0xec, 0x41, 0x92,
  0xd3, 0xbb, 0xf4, 0xf7, 0x7b, 0xdd, 0x9b, 0x71, 0x9c, 0xdc, 0xae, 0xb8,
  0xd3, 0x3d, 0x8a, 0xe8, 0xf3, 0x54, 0xd4, 0x79, 0xdd, 0x0f, 0xd0, 0x9d,
  0x5c, 0xbc, 0xf8, 0x89, 0x2e, 0x8d, 0xe9, 0x72, 0x9d, 0xea, 0xf3, 0xdc,
  0x57, 0xef, 0x73, 0xa5, 0xb7, 0x07, 0x4b, 0x3e, 0x97, 0x57, 0x9c, 0xe8,
  0x7e, 0xb0, 0x76, 0xfc, 0xb5, 0x5d, 0x9b, 0x82, 0x3b, 0x49, 0x49, 0x4a,
  0x52, 0x92, 0x92, 0x94, 0xa4, 0x8e, 0x4e, 0xff, 0x05, 0xd9, 0x57, 0x4d,
  0x2d, 0x2e, 0x3c, 0x00, 0x00
};

const char *HTMLInverterBody = R"====(
  <!DOCTYPE html>
  <html>
    <head>
      <meta http-equiv="refresh" content=60>
      <title>Danfoss Inverter</title>
      </head>
      <style>tr:nth-child(even) {background-color: #f3f3f3;}</style>
      <body>
        <h1>Danfoss Inverter</h1>
        <table>
          <tr>
            <th>Description</th>
            <th>Value</th>
            <th>Unit</th>
            <th>Index name</th>
          </tr>
)====";

const char *HTMLCommonHead = R"====(
  <!DOCTYPE html>
  <html>
    <head>
      <title>ComLynx &rArr; TCP Gateway (MQTT & REST API)</title>
        <style>
        body,.m{padding:1px;margin:0;font-family:sans-serif}h1,h4{padding:10px}h1,
        .m,h4{background:#0067AC;margin:1px}.m,
        .c{height:calc(100vh - 71px)}.m{min-width:20%}.c{flex-grow:1;overflow-y:auto}.w,
        .r{display:flex}a,
        h1,
        h4{color:white;text-decoration:none}.c 
        h4{padding-left:30%;margin-bottom:20px}.r{margin:4px}.boldT{text-align:left;padding-left:5%;font-weight:bold}
        label{width:30%;text-align:right;margin-right:2px}input,button,
        select{margin-top:-2px}.s{text-align:right}.s>
        option{direction:rtl}.i{text-align:center;width:3ch;color:black}.n{width:8ch}
        table {padding-left:20px; width: 80%; }
        table, th, td { 
            border: solid 2px #DDD;
            border-collapse: collapse; 
            padding: 2px 3px; 
            text-align: center;
        }
        .buttonRem {
          background-color: #f44336;
          border: none;
          color: white;
          padding: 10px 20px;
          text-align: center;
          text-decoration: none;
          display: inline-block;
          font-size: 12px;
          margin: 4px 2px;
          cursor: pointer;
          border-radius: 12px;
        }
        .buttonAdd {
          background-color: #04AA6D;
          border: none;
          color: white;
          padding: 8px 16px;
          text-align: center;
          text-decoration: none;
          display: inline-block;
          font-size: 12px;
          margin: 4px 2px;
          cursor: pointer;
          border-radius: 10px;
        }
        .button {
          background-color: #e7e7e7; 
          border: none;
          color: black;
          padding: 8px 16px;
          text-align: center;
          text-decoration: none;
          display: inline-block;
          font-size: 12px;
          margin: 4px 2px;
          cursor: pointer;
          border-radius: 10px;
        }

        #progress-container {
          width: 100%;
          max-width: 500px;
          margin: 20px auto;
          height: 100%;
          line-height: 100%;
        }
        progress {
          width: 100%;
          height: 30px;
        }
        </style>
        <script src="/settings/commonJS"></script>
  )====";

//***************************************************
// HTML page for TCP/IP settings
//***************************************************
const char *HTMLContentTCP = R"====(
    <div class='boldT'>TCP/IP</div>
      <div class='r'>
        <label for='dhcp'>DHCP On : </label>
        <input type='checkbox' name='dhcp' id='dhcp' style='width:25px;' onclick='checkDisabled();'>
      </div>
      <div class='r'>
        <label for='IPaddr'>Static IP Address : <br><span class='fsize10'></span></label>
        <input type='text' name='IPaddr' id='IPaddr' >
      </div>
      <div class='r'>
        <label for='gateway'>Gateway IP Address :  <br><span class='fsize10'></span></label>
        <input type='text' name='gateway' id='gateway' >
      </div>
      <div class='r'>
        <label for='mask'>Subnet Mask : </label>
        <input type='text' name='netmask' id='netmask' >
      </div>
      <div class='r'>
        <label for='primaryDNS'>primary DNS server :  <br><span class='fsize10'></span></label>
        <input type='text' name='primaryDNS' id='primaryDNS' >
      </div>
    <br>
    <div class='boldT'><br>HTTP Server</div>
      <div class='r'>
        <label for='HTTPPort'>port : </label>
        <input type='number' name='HTTPPort' id='HTTPPort' >
      </div>
      <div class='r'>
        <label for='RestAPI'>REST API Enabled : </label>
        <input type='checkbox' name='RestAPIOn' id='RestAPIOn' style='width:25px;' onclick='checkDisabled();'>
      </div>
    <div style='text-align:center;color:#FF6347;'> Restart gateway to apply settings</div>
    <div style='text-align:center;padding-top:20px;' id='FooterButtons'>
      <input type='button' onclick='Restart();' value='Restart Gateway' >
      <input type='button' onclick='SendValues();' value='Save' >
    </div>
)====";

//***************************************************
// HTML page for wifi settings
//***************************************************
const char *HTMLContentWifi = R"====(
    <form action='' method='get'>
    <div class='r'>
      <label for='ssid'> enter SSID (or Wifi network name) : </label>
      <input type='text' name='ssid' id='ssid' required>
    </div>
    <div class='r'>
      <label for='password'>password : </label>
      <input type='password' name='wifipassword' id='wifipassword' required>
    </div>
    <div style='text-align:center;padding-top:20px;'>
      <input type='submit' value='Submit' >
    </div>
    </form>
  )====";

const char *HTMLContentMQTT = R"====(
      <div class='boldT'>
        <br>MQTT server parameters
      </div>
      <div class='r'>
        <label for='MQTTOn'>MQTT On : </label>
        <input type='checkbox' name='MQTTOn' id='MQTTOn' style='width:25px;' onclick='checkDisabled();'>
      </div>
      <div class='r'>
        <label for='MQTTBroker'>MQTT broker's URL or IP address : </label>
        <input type='text' name='MQTTBroker' id='MQTTBroker' >
      </div>
      <div class='r'>
        <label for='MQTTTLS'>MQTT use TLS protocol : </label>
        <input type='checkbox' name='MQTTTLS' id='MQTTTLS' style='width:25px;' onclick='ChangePort();'>
      </div>
      <div> Please note that if using TLS encryption, broker's certificate will be trusted without verification, Man In the Middle attack will be possible !</div>
      <div class='r'>
        <label for='MQTTPort'> port : </label>
        <input type='number' name='MQTTPort' id='MQTTPort' >
      </div>
      <div class='r'>
        <label for='MQTTUser'>MQTT User name : </label>
        <input type='text' name='MQTTUser' id='MQTTUser' >
      </div>
      <div class='r'>
        <label for='MQTTPwd'>MQTT password : </label>
        <input type='password' name='MQTTPwd' id='MQTTPwd' >
      </div>
      <div class='r'>
        <label for='MQTTPrefix'>MQTT Prefix :  </label>
        <input type='text' name='MQTTPrefix' id='MQTTPrefix' >
        <div style='font-size: smaller;'>  (one word only)  </div>
      </div>
      <div class='r'>
        <label for='MQTTDiscOn'>Enable MQTT autodiscovery : </label>
        <input type='checkbox' name='MQTTDiscOn' id='MQTTDiscOn' style='width:25px;'>
      </div>
      <div class='r'>
        <label for='MQTTdeviceName'>MQTT Device Name :  </label>
        <input type='text' name='MQTTdeviceName' id='MQTTdeviceName' >
        <div style='font-size: smaller;'>  (one word only)  </div>
      </div>
      <div style='text-align:center;color:#FF6347;'> Restart gateway to apply settings</div>
      <div style='text-align:center;padding-top:20px;' id='FooterButtons'>
        <input type='button' onclick='Restart();' value='Restart Gateway' >
        <input type='button' onclick='SendValues();' value='Save' >
      </div>
      <br>
  )====";

const char *HTMLContentCLX = R"====(
      <div class='boldT'>
        <br>Comlynx Configuration
      </div>
      <div class='r'>
        <label for='ClxInterval'>Inverters query interval : </label>
        <input type='number' name='Inverters query interval' id='ClxInterval' >
        <div style='font-size: smaller;'> ms </div>
      </div>
      <div class='boldT'>
        <br>Inverters
      </div>
      <div style='text-align:center;'> For TLX inverters only, optional parameters can be activated : </div>
      <div style='text-align:center;'> HistData : historical datas like production of the day, week, month and year.</div>
      <div style='text-align:center;'> If these sensors are connected to the inverter :</div>
      <div style='text-align:center;'> ModTempS : modules temperature, AmbTempS : ambiant temperature, IRSTemp : IR sensor temperature, IRS : Irradiance, EM : external Energy Meter</div>
      <br>
      <p>
        <div style='padding-left:30px;' id="cont"></div>   <!--the container to add the table.-->
      </p>
      <div style='text-align:center;color:#FF6347;'> Restart gateway to apply settings</div>
      <div style='text-align:center;padding-top:20px;' id='FooterButtons'>
        <input type='button' class="button" onclick='ScanRS485();' value='Scan bus' >
        <input type="button" class="buttonAdd" onclick='addRow();' value='Add New Row' >
        <input type='button' class="button" onclick='Save();' value='Save' >
        <input type='button' class="button" onclick='Restart();' value='Restart Gateway' >
      </div>
      <br>
  )====";

const char *HTMLRoot = R"====(
  <!DOCTYPE html>
  <html>
    <head>
      <meta http-equiv="refresh" content=60>
      <title>ComLynx &rArr; TCP Gateway (MQTT & REST API)</title>
    </head>
    <body>
      <div style='text-align:center;font-weight:bold'>
        <br>Welcome to the ComLynx &rArr; TCP Gateway web Interface
      </div>
      <div style='text-align:center'>
      <br>You can enable a <a href=/settings/tcp.html>REST API</a> to get inverters values
      <br><br><a href=/settings/mqtt.html>enable MQTT</a> to send value to a MQTT broker
      <br><br>But first <a href=/settings/comlynx.html>discover/configure inverters</a> to get inverter parameters
      </div>
    </body>
  </html>
  )====";

const char *commonJS = R"====(
  function GID(id) { return document.getElementById(id); }
  function checkIP(id){
    var S=GID(id).value;
    var Table=S.split(".");
    var valide=true;
    if (Table.length!=4) {
      valide=false;
    }else{
      for (var i=0;i<Table.length;i++){
        if (Table[i]>255 || Table[i]<0) valide=false;
      }
    }
    if (valide){
      GID(id).style.color="black";
    } else {
      GID(id).style.color="red";
    }
    return valide;
  }
 
  function Restart(){
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() { 
      if (this.readyState == 4 && this.status == 200) {
        GID('FooterButtons').innerHTML=this.responseText;
        setTimeout(location.reload(),3000);
      }         
    };
    xhttp.open('POST', '/gw?action=restart', true);
    xhttp.send();
  }

  function Resetsettings(){
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() { 
      if (this.readyState == 4 && this.status == 200) {
        setTimeout(location.reload(),300);
      }         
    };
    xhttp.open('POST', '/settings?action=reset', true);
    xhttp.setRequestHeader('Content-Type', 'text/plain');
    xhttp.send('reset');
  }
)====";

// for MQTT settings webpage

const char *MQTTSetJS = R"====(
  function LoadParameters() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        var myObj = JSON.parse(this.responseText);
        GID("MQTTOn").checked = myObj.MQTTOn;
        GID("MQTTBroker").value = myObj.MQTTBroker;
        GID("MQTTTLS").checked = myObj.MQTTTLS;
        GID("MQTTPort").value = myObj.MQTTPort;
        GID("MQTTUser").value = myObj.MQTTUser;
        GID("MQTTPwd").value = myObj.MQTTPwd;
        GID("MQTTPrefix").value = myObj.MQTTPrefix;
        GID("MQTTDiscOn").checked = myObj.MQTTDiscOn;
        GID("MQTTdeviceName").value = myObj.MQTTdeviceName;
        checkDisabled();
      }         
    };
    xhttp.open('GET', '/settings/Param', true);
    xhttp.send();
  }

  function SendValues(){
    var MQTTOn = GID("MQTTOn").checked;
    var MQTTBroker = GID("MQTTBroker").value;
    var MQTTTLS = GID("MQTTTLS").checked;
    var MQTTPort = GID("MQTTPort").value;
    var MQTTUser = GID("MQTTUser").value.trim();
    var MQTTPwd = GID("MQTTPwd").value.trim();
    var MQTTPrefix = GID("MQTTPrefix").value.trim();
    var MQTTDiscOn = GID("MQTTDiscOn").checked;
    var MQTTdeviceName = GID("MQTTdeviceName").value.trim();
         
    var jsonFormInfo = JSON.stringify({
        MQTTOn:MQTTOn,
        MQTTBroker:MQTTBroker,
        MQTTTLS:MQTTTLS,
        MQTTPort:MQTTPort,
        MQTTUser:MQTTUser,
        MQTTPwd:MQTTPwd,
        MQTTPrefix:MQTTPrefix,
        MQTTDiscOn:MQTTDiscOn,
        MQTTdeviceName:MQTTdeviceName
    });

    var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() { 
        if (this.readyState == 4 && this.status == 200) {
          var retour=this.responseText;
          location.reload();
        }         
      };
      xhttp.open('POST', '/settings' , true);
      xhttp.setRequestHeader('Content-Type', 'application/json');
      xhttp.send(jsonFormInfo);
  }

  function checkDisabled(){
    GID("MQTTBroker").disabled=!GID("MQTTOn").checked;
    GID("MQTTTLS").disabled=!GID("MQTTOn").checked;
    GID("MQTTPort").disabled=!GID("MQTTOn").checked;
    GID("MQTTUser").disabled=!GID("MQTTOn").checked;
    GID("MQTTPwd").disabled=!GID("MQTTOn").checked; 
    GID("MQTTPrefix").disabled=!GID("MQTTOn").checked;
    GID("MQTTDiscOn").disabled=!GID("MQTTOn").checked;
    GID("MQTTdeviceName").disabled=!GID("MQTTOn").checked; 
  }

  function ChangePort(){
    if (GID("MQTTTLS").checked){
      GID("MQTTPort").value=8883;
    } else {
      GID("MQTTPort").value=1883;
    }
  }
)====";

// for TCP/IP settings webpage
const char *TCPSetJS = R"====(
  function LoadParameters() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        var myObj = JSON.parse(this.responseText);
        GID("dhcp").checked = myObj.dhcpOn;
        GID("IPaddr").value = myObj.StaticIP;
        GID("gateway").value = myObj.Gateway;
        GID("netmask").value = myObj.Netmask;
        GID("primaryDNS").value = myObj.primaryDNS;
        GID("RestAPIOn").checked = myObj.RestAPIOn;
        GID("HTTPPort").value = myObj.HTTPPort;
                  
        checkDisabled();
      }
    };
    xhttp.open('GET', '/settings/Param', true);
    xhttp.send();
  }

  function SendValues(){

    var dhcpOn=GID("dhcp").checked;
    var StaticIP=GID("IPaddr").value;
    var Gateway=GID("gateway").value;
    var Netmask=GID("netmask").value;
    var primaryDNS=GID("primaryDNS").value;
    var RestAPIOn=GID("RestAPIOn").checked;
    var HTTPPort=GID("HTTPPort").value;
         
    var jsonFormInfo = JSON.stringify({
        dhcpOn:dhcpOn,
        StaticIP:StaticIP,
        Gateway:Gateway,
        Netmask:Netmask,
        primaryDNS:primaryDNS,
        RestAPIOn:RestAPIOn,
        HTTPPort:HTTPPort
    });
    if ( GID("dhcp").checked || (checkIP("IPaddr") && checkIP("gateway") && checkIP("netmask") && checkIP("primaryDNS")) ){
      var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() { 
          if (this.readyState == 4 && this.status == 200) {
            var retour=this.responseText;
            location.reload();
          }         
        };
        xhttp.open('POST', '/settings' , true);
        xhttp.setRequestHeader('Content-Type', 'application/json');
        xhttp.send(jsonFormInfo);
    }
  }

  function checkDisabled(){ 
    GID("IPaddr").disabled=GID("dhcp").checked;
    GID("gateway").disabled=GID("dhcp").checked;
    GID("netmask").disabled=GID("dhcp").checked;
    GID("primaryDNS").disabled=GID("dhcp").checked;
  }

)====";

// for Comlynx webpage

const char *CLSetJS = R"====(

  let col = [];

  function Init(){
    LoadParameters();
    GetInverter();
  }

  let Save = () => {
    SendValues();
    PostInverters();
  }

  async function GetInverter() {
    const response = await fetch('/inverters', {
        method: 'GET',
        headers: {'Accept': 'application/json'},
      });
    const Inverters = await response.json();
    tableFromJson(Inverters);
  }

  let tableFromJson = (myJsonObj) => {

    if(myJsonObj != null && Object.keys(myJsonObj).length > 0) {
      for (let i = 0; i < myJsonObj.length; i++) {
        for (let key in myJsonObj[i]) {
          if (col.indexOf(key) === -1) {
            col.push(key);
          }
        }
      }
    } else {
      col = ['Id', 'InvType', 'ProductNumber', 'SerialNumber', 'Address'];  
    }

    var InvTypes = ['TLX', 'ULX'];
    var dataList = document.createElement('datalist');
    dataList.id = 'InvType_list';
    InvTypes.forEach(value => {
      var option = document.createElement('option');
      option.innerHTML = value;
      option.value = value;
      dataList.appendChild(option);
    })
    document.body.appendChild(dataList);

    let invTable = document.createElement("table");
    invTable.setAttribute('id', 'invTable');  // table id.

    let tr = invTable.insertRow(-1);

    for (let i = 0; i < col.length; i++) {
      let th = document.createElement("th");
      th.innerHTML = col[i];
      tr.appendChild(th);
    }

    let th = document.createElement("th");
    th.innerHTML = "";
    tr.appendChild(th);

    if(myJsonObj != null && Object.keys(myJsonObj).length > 0) {
      for (let i = 0; i < myJsonObj.length; i++) {

        tr = invTable.insertRow(-1);

        for (let j = 0; j < col.length; j++) {
          let tabCell = tr.insertCell(-1);
          if(col[j] == "Id") {
            tabCell.innerHTML = myJsonObj[i][col[j]];
          } else {
            const inputCell = document.createElement('input');
            if (col[j] == "HistData" || col[j] == "ModTempS" || col[j] == "AmbTempS" || col[j] == "IRSTemp" || col[j] == "IRS" || col[j] == "EM") {
              inputCell.setAttribute('type', 'checkbox');
              if (myJsonObj[i][col[j]] == 1) {
                inputCell.setAttribute('checked', '');
              } else {
                inputCell.removeAttribute('checked');
              }
              if (myJsonObj[i]['InvType'] != "TLX" ) {
                inputCell.setAttribute('disabled', true);
              }
            } else {
              inputCell.setAttribute('type', 'text');
              if (col[j] == "ProductNumber" || col[j] == "SerialNumber") {
                inputCell.setAttribute('maxlength', 11);
                inputCell.setAttribute('size', 11);
              }
              if (col[j] == "Address" ) {
                inputCell.setAttribute('maxlength', 4);
                inputCell.setAttribute('size', 4);
              }
              if (col[j] == "InvType") {
                inputCell.setAttribute('maxlength', 3);
                inputCell.setAttribute('size', 3);
                inputCell.setAttribute('list', "InvType_list");
                inputCell.addEventListener('input', checkDisabled);
              }
              inputCell.setAttribute('value', myJsonObj[i][col[j]]);
            }
            inputCell.setAttribute('id',col[j]);
            tabCell.appendChild(inputCell);
          }
        }

        let tabCell = tr.insertCell(-1);
        const button = document.createElement('input');
        button.setAttribute('type', 'button');
        button.setAttribute('value', 'Remove');
        button.setAttribute('class', 'buttonRem');
        button.setAttribute('onclick', 'removeRow(this)');
        tabCell.appendChild(button);
      }
    }
    let div = document.getElementById('cont');
    div.appendChild(invTable);
  }

  // function to add new row.
  let addRow = () => {
    let invTab = document.getElementById('invTable');
    let rowCnt = invTab.rows.length;
    let tr = invTab.insertRow(rowCnt);

    for (let c = 0; c < col.length; c++) {
      let td = document.createElement('td');
      td = tr.insertCell(c);

      if (col[c] == "Id") { 
          td.innerHTML = rowCnt;
      }
      else {
        // the 2nd, 3rd and 4th column, will have textbox.
        const ele = document.createElement('input');
        if (col[c] == "HistData" || col[c] == "ModTempS" || col[c] == "AmbTempS" || col[c] == "IRSTemp" || col[c] == "IRS" || col[c] == "EM") {
          ele.setAttribute('type', 'checkbox');
          ele.setAttribute('disabled', true);
        } else {
          ele.setAttribute('type', 'text');
          if (col[c] == "Address") {
            ele.setAttribute('value', '223F');
            ele.setAttribute('maxlength', 4);
            ele.setAttribute('size', 4);
          } else {
            ele.setAttribute('value', '');
          }
          if (col[c] == "ProductNumber" || col[c] == "SerialNumber") {
            ele.setAttribute('maxlength', 11);
            ele.setAttribute('size', 11);
          }
          if (col[c] == "InvType") {
            ele.setAttribute('maxlength', 3);
            ele.setAttribute('size', 3);
            ele.setAttribute('list', "InvType_list");
            ele.addEventListener('input', checkDisabled);
          }
        }
        ele.setAttribute('id',col[c]);
        td.appendChild(ele);
      }
    }
    let tabCell = tr.insertCell(-1);
    // Add remove button
    const button = document.createElement('input');
    // set the attributes.
    button.setAttribute('type', 'button');
    button.setAttribute('value', 'Remove');
    button.setAttribute('class', 'buttonRem');
    // add button's "onclick" event.
    button.setAttribute('onclick', 'removeRow(this)');
    tabCell.appendChild(button);
  }

  // function to delete a row.
  let removeRow = (oButton) => {
    let invTab = document.getElementById('invTable');
    invTab.deleteRow(oButton.parentNode.parentNode.rowIndex); // buttton -> td -> tr
    let rowCnt = invTab.rows.length;    // get the number of rows.
    for (let r = 0; r < rowCnt; r++) {
      for (let c = 0; c < col.length; c++) { 
        let tabCell = invTab.rows[r].cells[c];
        if(col[c] == "Id" && tabCell.innerHTML != "Id") {
          tabCell.innerHTML = r;
        }
      }
    }
  }

  function checkDisabled(event) {
    let row = event.target.closest('tr');
    let type = event.target.value
    if (type == "TLX") {
      row.querySelector(`[id="HistData"]`).disabled = false;
      row.querySelector(`[id="ModTempS"]`).disabled = false;
      row.querySelector(`[id="AmbTempS"]`).disabled = false;
      row.querySelector(`[id="IRSTemp"]`).disabled = false;
      row.querySelector(`[id="IRS"]`).disabled = false;
      row.querySelector(`[id="EM"]`).disabled = false;
    } else {
      row.querySelector(`[id="HistData"]`).disabled = true;
      row.querySelector(`[id="ModTempS"]`).disabled = true;
      row.querySelector(`[id="AmbTempS"]`).disabled = true;
      row.querySelector(`[id="IRSTemp"]`).disabled = true;
      row.querySelector(`[id="IRS"]`).disabled = true;
      row.querySelector(`[id="EM"]`).disabled = true;
    }
  }

  function LoadParameters() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        var myObj = JSON.parse(this.responseText);
        GID("ClxInterval").value = myObj.ClxInterval;
      }         
    };
    xhttp.open('GET', '/settings/Param', true);
    xhttp.send();
  }

  function SendValues(){
    var ClxInterval = GID("ClxInterval").value;
    var jsonFormInfo = JSON.stringify({
        ClxInterval:ClxInterval
    });
    var xhttp = new XMLHttpRequest();

    xhttp.onreadystatechange = function() { 
      if (this.readyState == 4 && this.status == 200) {
        var retour=this.responseText;
        location.reload();
      }         
    };
    xhttp.open('POST', '/settings' , true);
    xhttp.setRequestHeader('Content-Type', 'application/json');
    xhttp.send(jsonFormInfo);
  }

  async function PostInverters(){
    let myTab = document.getElementById('invTable');
    let arrValues = new Array();
    var remelement = document.getElementById('Post_Output');
    remelement?.remove();

    // loop through each row of the table.
    for (let row of myTab.rows) {
      let obj = {};
      for (let cell of row.cells) {
        if (cell.childNodes[0] != undefined)
        {
          if (cell.childNodes[0].nodeName == 'INPUT' && cell.childNodes[0].type == 'text') {
            if (cell.childNodes[0].value != '') obj[col[cell.cellIndex]] = cell.childNodes[0].value;
          } else {
            if (cell.childNodes[0].nodeName == 'INPUT' && cell.childNodes[0].type == 'checkbox') {
              if (cell.childNodes[0].checked) {
                obj[col[cell.cellIndex]] = 1;
              } else {
                obj[col[cell.cellIndex]] = 0;
              }
            } else {
              if (col[cell.cellIndex] == "Id" && cell.innerHTML != "Id") obj[col[cell.cellIndex]] = Number(cell.innerHTML);
            }
          }
        }
      }
      if (Object.keys(obj).length > 0) arrValues.push(obj);
    }
    if (arrValues.length > 0) {
      fetch('/inverters', {
        method: 'POST',
        headers: {'Content-Type': 'application/json'},
        body: JSON.stringify(arrValues)
      })
      .then(response => {
        if (response.ok) {
          var output = document.createElement('div');
          output.id = 'Post_Output';
          output.appendChild(document.createElement('br'));  // table id.
          output.style.fontWeight = 'bold';
          output.style.textAlign = 'center';
          let div = document.getElementById('cont');
          div.appendChild(output);
        } else {
          return response.text();
        }

      })
      .then(text => {
        if(text) {
          var output = document.createElement('div');
          output.id = 'Post_Output';
          output.appendChild(document.createElement('br'));
          output.appendChild(document.createTextNode(text));
          output.style.color = 'red';
          output.style.fontWeight = 'bold';
          output.style.textAlign = 'center';
          let div = document.getElementById('cont');
          div.appendChild(output);
        }
      })
      .catch(error => {
        console.log(error);
      })  
    }
  }

  function ScanRS485() {
    let pollingInterval;
    let container = document.getElementById('cont');
    let ScanProgressB = document.createElement('div');
    ScanProgressB.setAttribute('id', 'progress-container');
    ScanProgressB.setAttribute('class', 'progress-bar');
    let ScanProgress = document.createElement('progress');
    ScanProgress.setAttribute('id', 'progressBar');
    ScanProgress.value = 0;
    ScanProgress.max = 100;
    ScanProgressB.appendChild(ScanProgress);
    var scanstatus = '';
    container.replaceChildren(ScanProgressB);
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() { 
      if (this.readyState == 4 && this.status != 202) {
        setTimeout(location.reload(),3000);
      }       
    };
    xhttp.open('POST', '/inverters/scan/start', true);
    xhttp.send();

    setTimeout(EndTimeout, 1800000); // after 30 min, page will be reloaded

    pollingInterval = setInterval(pollStateApi, 1800); //each 1.8s API will be called to update progressbar value
  }

  async function pollStateApi() {
    let ScanProgress = document.getElementById('progressBar');
    try {
      let response = await fetch('/inverters/scan/state', {
        method: 'GET',
        headers: {
          'Content-Type': 'application/json',
        }
      });

      if (!response.ok) {
        throw new Error('API call error');
      }

      if (response.status == 200) {
        var myObj = await response.json();
        scanstatus = myObj.status;
        ScanProgress.value = myObj.progresspercent;
      }
      if (response.status == 303) {
        scanstatus = 'finished';
        ScanProgress.value = 100.00;
        clearInterval(pollingInterval);
        var output = document.createElement('div');
        output.id = 'Post_Output';
        output.appendChild(document.createElement('br'));
        output.appendChild(document.createTextNode('scan ended successfully, page will be reloaded to edit discovered inverters !'));
        output.style.color = 'green';
        output.style.fontWeight = 'bold';
        output.style.textAlign = 'center';
        container.appendChild(output);
        setTimeout(location.reload(),3000);
      }
    } catch (error) {
      console.error('API polling error : ', error);
      scanstatus = 'error';
      let container = document.getElementById('cont');
      let ScanProgressB = document.getElementById('progress-container');
      container.removeChild(ScanProgressB);
      clearInterval(pollingInterval);
      var output = document.createElement('div');
      output.id = 'Post_Output';
      output.appendChild(document.createElement('br'));
      output.appendChild(document.createTextNode('scan error, something went wrong, page will be reloaded !'));
      output.style.color = 'red';
      output.style.fontWeight = 'bold';
      output.style.textAlign = 'center';
      container.appendChild(output);
      setTimeout(location.reload(),3000);
    }
  }

  function EndTimeout() {
    console.error('Timeout error');
    clearInterval(pollingInterval);
    scanstatus = 'error';
    var output = document.createElement('div');
    output.id = 'Post_Output';
    output.appendChild(document.createElement('br'));
    output.appendChild(document.createTextNode('scan too long, page will be reloaded !'));
    output.style.color = 'red';
    output.style.fontWeight = 'bold';
    output.style.textAlign = 'center';
    container.appendChild(output);
    setTimeout(location.reload(),3000);
  }

)====";

String JsonParam()
{
  ParamsJson.clear();
  ParamsJson["dhcpOn"] = dhcpOn;
  ParamsJson["StaticIP"] = StaticIP;
  ParamsJson["Gateway"] = Gateway;
  ParamsJson["Netmask"] = Netmask;
  ParamsJson["primaryDNS"] = primaryDNS;
  ParamsJson["MQTTOn"] = MQTTOn;
  ParamsJson["RestAPIOn"] = RestAPIOn;
  ParamsJson["ClxInterval"] = ClxInterval;
  ParamsJson["MQTTBroker"] = MQTTBroker;
  ParamsJson["MQTTTLS"] = MQTTTLS;
  ParamsJson["MQTTPort"] = MQTTPort;
  ParamsJson["MQTTUser"] = String(MQTTUser);
  ParamsJson["MQTTPwd"] = String(MQTTPwd);
  ParamsJson["MQTTPrefix"] = String(MQTTPrefix);
  ParamsJson["MQTTDiscOn"] = MQTTDiscOn;
  ParamsJson["MQTTdeviceName"] = String(MQTTdeviceName);
  ParamsJson["HTTPPort"] = HTTPPort;

  String jsonString;
  serializeJson(ParamsJson, jsonString);
  return jsonString;
}

void HandlePostSettings(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
  String bodyString = (const char *)data;
  const char *ip;

  Serial.println(bodyString);

  if (request->args() == 1)
  {
    if (request->argName((int) 0) == "action" && request->arg((int) 0) == "reset" )
    {
      ResetConfigParameters();
      GetConfigParameters();
      request->send(202, "text/plain", "Accepted");
    }
    else
    {
      request->send(405, "text/plain", "Method Not Allowed");
    }
  }
  else
  {
    ParamsJson.clear();
    DeserializationError error = deserializeJson(ParamsJson, bodyString);

    if (error)
    {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      return;
    }

    if (ParamsJson.containsKey("dhcpOn"))
    {
      dhcpOn = ParamsJson["dhcpOn"];
    }
    if (ParamsJson.containsKey("StaticIP"))
    {
      ip = ParamsJson["StaticIP"];
      if (ip != "")
      {
        StaticIP.fromString(ip);
      }
    }
    if (ParamsJson.containsKey("Gateway"))
    {
      ip = ParamsJson["Gateway"];
      if (ip != "")
      {
        Gateway.fromString(ip);
      }
    }
    if (ParamsJson.containsKey("Netmask"))
    {
      ip = ParamsJson["Netmask"];
      if (ip != "")
      {
        Netmask.fromString(ip);
      }
    }
    if (ParamsJson.containsKey("primaryDNS"))
    {
      ip = ParamsJson["primaryDNS"];
      if (ip != "")
      {
        primaryDNS.fromString(ip);
      }
    }
    if (ParamsJson.containsKey("HTTPPort"))
    {
      HTTPPort = (unsigned int)ParamsJson["HTTPPort"];
    }
    if (ParamsJson.containsKey("RestAPIOn"))
    {
      RestAPIOn = ParamsJson["RestAPIOn"];
    }
    if (ParamsJson.containsKey("MQTTOn"))
    {
      MQTTOn = ParamsJson["MQTTOn"];
    }
    if (ParamsJson.containsKey("ClxInterval"))
    {
      ClxInterval = (unsigned long)ParamsJson["ClxInterval"];
    }
    if (ParamsJson.containsKey("MQTTTLS"))
    {
      MQTTTLS = ParamsJson["MQTTTLS"];
    }
    if (ParamsJson.containsKey("MQTTBroker"))
    {
      MQTTBroker = ParamsJson["MQTTBroker"].as<String>();
    }
    if (ParamsJson.containsKey("MQTTPort"))
    {
      MQTTPort = (unsigned int)ParamsJson["MQTTPort"];
    }
    if (ParamsJson.containsKey("MQTTUser"))
    {
      MQTTUser = ParamsJson["MQTTUser"].as<String>();
    }
    if (ParamsJson.containsKey("MQTTPwd"))
    {
      MQTTPwd = ParamsJson["MQTTPwd"].as<String>();
    }
    if (ParamsJson.containsKey("MQTTPrefix"))
    {
      MQTTPrefix = ParamsJson["MQTTPrefix"].as<String>();
    }
    if (ParamsJson.containsKey("MQTTDiscOn"))
    {
      MQTTDiscOn = ParamsJson["MQTTDiscOn"];
    }
    if (ParamsJson.containsKey("MQTTdeviceName"))
    {
      MQTTdeviceName = ParamsJson["MQTTdeviceName"].as<String>();
    }
    SaveConfigParameters();
    ParamsJson.clear();
    request->send(201, "text/plain", "Created");
  }
}

void HandleGW(AsyncWebServerRequest *request)
{
  for (uint8_t i = 0; i < request->args(); i++)
  {
    if (request->argName(i) == "action" && request->arg(i) == "restart")
    {
      request->send(202, "text/plain", "Accepted");
      delay(1000);
      ESP.restart();
    }
  }
  request->send(400, "text/plain", "Bad Request");
}

// Web page that holds all parameters/measurements in a table
void HandleInverter(AsyncWebServerRequest *request)
{
  String url = request->url();
  int _InvId;
  String Requested = "";
  String msg = String(HTMLInverterBody);
  int InvListSize = mypInverterList.size();
  if (url.startsWith("/inverter/")) {
    if (InvListSize >= 1) {
      Requested = url.substring(10);
      if (Requested.length() >= 1) {
        _InvId = Requested.toInt();
        if (0 < _InvId && _InvId <= InvListSize) {
          myInv = mypInverterList.at(_InvId - 1);
          if (myInv->PrintProductNumber().c_str()[0] != 0) {
            msg += "<tr><td>Product Number</td><td>" + myInv->PrintProductNumber() + "</td><td></td><td></td></tr>";
          }
          if (myInv->PrintSN().c_str()[0] != 0) {
            msg += "<tr><td>Serial Number</td><td>" + myInv->PrintSN() + "</td><td></td><td></td></tr>";
          }
          msg += "<tr><td>Name</td><td>" + myInv->PrintName() + "</td><td></td><td></td></tr>";
          msg += "<tr><td>Operation mode</td><td>" + myInv->PrintOpMode() + "</td><td></td><td>OpModeTxt</td></tr>";
          for (int i = 0; i < myInv->PrintParametersCount(); i++) {
            msg += "<tr><td>" + myInv->PrintNameParameter(i) + "</td><td>" + myInv->PrintFormatedMeasParameter(i) + "</td><td>" + myInv->PrintUnitParameter(i) + "</td><td>" + myInv->PrintParNameParameter(i) + "</td></tr>";
          }
          msg += "</body></html>";
          request->send(200, "text/html", msg);
        }
        else {
          String msg = "No inverter with id : " + String(_InvId) + " found, please check your inverter configuration !";
          request->send(404, "text/plain", msg);
        }
      }
      else {
        String msg = "No inverter id given, please send inverter id after /inverter ex : /inverter/1";
        request->send(404, "text/plain", msg);
      }
    }
    else {
      String msg = "No inverter configured, please configure inverter first !";
      request->send(404, "text/plain", msg);
    }
  }
}

String JsonInverters()
{
  InverterConfigElement *MemInv;
  byte _OptionByte;
  int InvNb = InverterMemList.size();
  InvJson.clear();

  if (InvNb > 0)
  {
    for (int i = 0; i < InvNb; i++)
    {
      MemInv = InverterMemList.at(i);
      _OptionByte = MemInv->OptionByte;
      InvJson[i]["Id"] = MemInv->Id;
      InvJson[i]["InvType"] = MemInv->InvType;
      InvJson[i]["ProductNumber"] = MemInv->ProductNumber;
      InvJson[i]["SerialNumber"] = MemInv->SerialNumber;
      InvJson[i]["Address"] = MemInv->Address;
      InvJson[i]["HistData"] = ((_OptionByte & 1) == 1);
      InvJson[i]["ModTempS"] = ((_OptionByte & 2) == 2);
      InvJson[i]["AmbTempS"] = ((_OptionByte & 4) == 4);
      InvJson[i]["IRSTemp"] = ((_OptionByte & 8) == 8);
      InvJson[i]["IRS"] = ((_OptionByte & 16) == 16);
      InvJson[i]["EM"] = ((_OptionByte & 32) == 32);
    }
  }
  String jsonString;
  serializeJson(InvJson, jsonString);
  return jsonString;
}

void HandleGetInverters(AsyncWebServerRequest *request)
{
  String json = JsonInverters();
  request->send(200, "application/json", json);
  json = String();
}

void HandlePostInverters(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
  String jsonString = (const char *)data;
  int _InvId;
  String _InvType;
  String _InvProductNumber;
  String _InvSerialNumber;
  String _InvAddress;
  boolean _HistData;
  boolean _ModTempS;
  boolean _AmbTempS;
  boolean _IRSTemp;
  boolean _IRS;
  boolean _EM;
  byte _OptionByte = 0;

  InvJson.clear();
  DeserializationError error = deserializeJson(InvJson, jsonString);

  if (error)
  {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    InvJson.clear();
    request->send(400, "text/plain", "Error Parsing json body");
    return;
  }

  JsonArray InvArr = InvJson.as<JsonArray>();
  InverterMemList.clear();

  for (JsonObject InvO : InvArr)
  {
    _InvId = InvO["Id"];
    _InvType = InvO["InvType"].as<String>();
    _InvProductNumber = InvO["ProductNumber"].as<String>();
    _InvSerialNumber = InvO["SerialNumber"].as<String>();
    _InvAddress = InvO["Address"].as<String>();
    if (InvO.containsKey("HistData")) { _HistData = InvO["HistData"]; } else { _HistData = false;}
    if (InvO.containsKey("ModTempS")) { _ModTempS = InvO["ModTempS"]; } else { _ModTempS = false;}
    if (InvO.containsKey("AmbTempS")) { _AmbTempS = InvO["AmbTempS"]; } else { _AmbTempS = false;}
    if (InvO.containsKey("IRSTemp")) { _IRSTemp = InvO["IRSTemp"]; } else { _IRSTemp = false;}
    if (InvO.containsKey("IRS")) { _IRS = InvO["IRS"]; } else { _IRS = false;}
    if (InvO.containsKey("EM")) { _EM = InvO["EM"]; } else { _EM = false;}
    _OptionByte |= (_EM << 5);
    _OptionByte |= (_IRS << 4);
    _OptionByte |= (_IRSTemp << 3);
    _OptionByte |= (_AmbTempS << 2);
    _OptionByte |= (_ModTempS << 1);
    _OptionByte |= (_HistData << 0);
    if ((_InvType == "TLX" || _InvType == "ULX") && _InvProductNumber != "" && _InvSerialNumber != "" && _InvAddress != "")
    {
      String msg = "Inverter " + String(_InvId) + " posted";
      Serial.println(msg);
      if (_InvType == "TLX" ) {
        InverterMemList.push_back(new InverterConfigElement(_InvId, _InvType, _InvProductNumber, _InvSerialNumber, _InvAddress, _OptionByte));
      } else {
        InverterMemList.push_back(new InverterConfigElement(_InvId, _InvType, _InvProductNumber, _InvSerialNumber, _InvAddress));
      }
    }
    else
    {
      request->send(400, "text/plain", "Invalid inverters parameters");
      GetInvertersFromEprom(&InverterMemList);
      Serial.println("Invalid inverters parameters");
      return;
    }
  }
  Serial.println("All inverters posted");

  SaveInvertersToEprom(&InverterMemList);
  InvJson.clear();
  request->send(201, "text/plain", "Created");
}

void HandleInvertersScan(AsyncWebServerRequest *request)
{
  String url = request->url();
  bool found = false;
  String Requested = "";
  Requested = url.substring(16);
  Requested.toLowerCase();
  if (Requested == "start" && request->method() == HTTP_POST) {
    AsyncWebServerResponse *response = request->beginResponse(202, "text/plain", "Accepted");
    response->addHeader("Location","/inverters/scan/state");
    request->send(response);
    if (!*InverterScan) {
      xTaskCreate( ScanComlynxBus, "scan comlynx Task", 10000, NULL, 0, NULL);
    }
    found = true;
  }
  if (Requested == "state" && request->method() == HTTP_GET) {
    if (*InverterScan) {
      String msg = "{\"status\": \"running\", \"progresspercent\": " + String(*ScanPercent) + "}";
      request->send(200, "application/json", msg);
    } else {
      if (*ScanPercent == 100 ) {
        AsyncWebServerResponse *response = request->beginResponse(303, "text/plain", "scan completed, reload inverters list to edit or save");
        response->addHeader("Location","/inverters");
        request->send(response);
        *ScanPercent = 0;
      } else {
        request->send(409, "text/plain", "No new scan started, please start scan before calling state !");
      }
    }
    found = true;
  }
  if (!found) {
    String msg = "unknown endpoint";
    request->send(404, "text/plain", msg);
  }
}

String contentInfo()
{
  String InfoMsg = "<div class=\"boldT\"><br>General</small></div><br><div class='r'>Software Version : ";
  InfoMsg += String(Version) + " from " + String(CodeDate);
  InfoMsg += "</div><div class='r'>Compiled : ";
  InfoMsg += String(__DATE__) + " " + String(__TIME__);
  InfoMsg += "</div><div class='r'>Hostname : ";
  InfoMsg += hostname;
  InfoMsg += "</div><div class='r'> MAC Address : ";
  InfoMsg += WiFi.macAddress();
  InfoMsg += "</div><div class='r'> Chip Model : ";
  InfoMsg += ESP.getChipModel();
  InfoMsg += "</div><div class='r'> Chip Revision : ";
  InfoMsg += ESP.getChipRevision();
  InfoMsg += "</div><br>";
  InfoMsg += "<div style='text-align:center;padding-top:20px;' id='ResetSetButt'>";
  InfoMsg += "<input type='button' onclick='Resetsettings();' value='Reset config'>";
  InfoMsg += "<small>   Reset TCP & MQTT settings to factory</small></div>";
  return InfoMsg;
}

// Web page that holds configuration parameters
void HandleSettings(AsyncWebServerRequest *request) {
  String url = request->url();
  String low_url = url;
  low_url.toLowerCase();
  bool found = false;
  String RequestedPageName = "";
  if (low_url.startsWith("/settings"))
  {
    if (low_url == "/settings") {
      RequestedPageName = PageMenu.PageName[0];
    }
    else {
      RequestedPageName = url.substring(10);
    }
    if (RequestedPageName == "Param" && request->method() == HTTP_GET ) {
      String json = JsonParam();
      request->send(200, "application/json", json);
      json = String();
      found = true;
    }
    if (RequestedPageName == "commonJS" && request->method() == HTTP_GET) {
      request->send(200, "application/javascript", String(commonJS));
      found = true;
    }
    if (RequestedPageName == "TCPSetJS" && request->method() == HTTP_GET) {
      request->send(200, "application/javascript", String(TCPSetJS));
      found = true;
    }
    if (RequestedPageName == "CLSetJS" && request->method() == HTTP_GET) {
      request->send(200, "application/javascript", String(CLSetJS));
      found = true;
    }
    if (RequestedPageName == "MQTTSetJS" && request->method() == HTTP_GET) {
      request->send(200, "application/javascript", String(MQTTSetJS));
      found = true;
    }
    RequestedPageName.toLowerCase();
    for (int i = 0; i < MPAGE_ENUMS; i++) {
      if (RequestedPageName == PageMenu.PageName[i]) {
        found = true;
        String msg = String(HTMLCommonHead);
        if (PageMenu.ScriptName[i] != "") {
          msg += "<script src=" + PageMenu.ScriptName[i] + "></script>";
          msg += "</head>";
          msg += "<body onload=" + PageMenu.FunctionName[i] + ";>";
        } else {
          msg += "</head>";
          msg += "<body>";
        }
        msg += "<h1>ComLynx &rArr; TCP Gateway (MQTT + REST API)</h1>";
        msg += "<div class=w>";
        msg += "<div class=m>";
        // Left Menu
        for (byte j = 0; j < MPAGE_ENUMS; j++) {
          msg += "<h4 ";
          if ((j) == i) {
            msg += " style=background-color:#FF6600";
          }
          msg += "><a href=/settings/" + PageMenu.PageName[j] + ">" + PageMenu.MenuName[j] + "</a></h4>";
        }
        msg += "</div><div class=c><h4>" + PageMenu.MenuName[i] + "</h4><form method=post>";

        switch (i) {
        case SysInfo:
          msg += contentInfo();
          break;
        case WifiSet: {
          String NewSsid = "";
          String NewPassword;
          if (request->hasParam("ssid", true)) {
            NewSsid = request->getParam("ssid", true)->value();
            NewSsid.trim();
          }
          if (request->hasParam("wifipassword", true)) {
            NewPassword = request->getParam("wifipassword", true)->value();
            NewPassword.trim();
          }
          Serial.println(NewSsid);
          Serial.println(NewPassword);
          if (NewSsid.length() == 0) {
            msg += String(HTMLContentWifi);
            if (SSID != "") {
              msg.replace("name='ssid' id='ssid'", "name='ssid' value='" + SSID + "' id='ssid'");
            }
          } else {
            msg += "<div class='boldT'><br>switch to wifi ! " + NewSsid + " </div>";
            SSID = NewSsid;
            SECRET_WIFI_PSWD = NewPassword;
            SaveWifiParameters();
            delay(5000);
            ESP.restart();
          }
        }
        break;
        case TCPSet:
          msg += String(HTMLContentTCP);
          break;
        case CLSet:
          msg += String(HTMLContentCLX);
          break;
        case MQTTSet:
          msg += String(HTMLContentMQTT);
          break;
        default:
          break;
        }
        msg += "</form></body></html>";
        request->send(200, "text/html", msg);
      }
    }
    if (!found) {
      String msg = "Page not found \n\n";
      request->send(404, "text/plain", msg);
    }
  }
}

void HandleIcon(AsyncWebServerRequest *request) {
  AsyncWebServerResponse *response = request->beginResponse_P(200, "image/x-icon", favicon_ico_gz, favicon_ico_gz_len);
  response->addHeader("Content-Encoding", "gzip");
  request->send(response);
}

// Handle inverter REST API call
void HandleRESTAPI(AsyncWebServerRequest *request) {
  String url = request->url();
  int _InvId;
  int InvVecIdx;
  bool found = false;
  String Requested = "";
  String APIParam;
  int InvListSize = mypInverterList.size();
  if (InvListSize >= 1)
  {
    Requested = url.substring(5);

    if (Requested.length() >= 1)
    {
      _InvId = Requested.substring(0, Requested.indexOf("/")).toInt();
      Serial.println(" for inverter : ");
      Serial.println(_InvId);
      if (0 < _InvId && _InvId <= InvListSize)
      {
        InvVecIdx = _InvId - 1;
        APIParam = Requested.substring(Requested.indexOf("/") + 1);
        Serial.println(APIParam);
        myAPIInverter = mypInverterList.at(InvVecIdx);
        if (APIParam == "" && request->method() == HTTP_GET)
        {
          String msg = "No Parameter specified, please provide the parameter name you want or All !";
          request->send(404, "text/plain", msg);
          found = true;
        }
        if (APIParam == "OpModeTxt" && request->method() == HTTP_GET)
        {
          CreateJsonTxt("OpModeTxt", myAPIInverter->PrintOpMode().c_str());
          request->send(200, "application/json", jsonbuffer);
          found = true;
        }
        if (APIParam == "Product" && request->method() == HTTP_GET)
        {
          CreateJsonTxt("Product", myAPIInverter->PrintProductNumber().c_str());
          request->send(200, "application/json", jsonbuffer);
          found = true;
        }
        if (APIParam == "SerialTxt" && request->method() == HTTP_GET)
        {
          CreateJsonTxt("Serial", myAPIInverter->PrintSN().c_str());
          request->send(200, "application/json", jsonbuffer);
          found = true;
        }
        if (APIParam == "All" && request->method() == HTTP_GET)
        {
          CreateJsonAll();
          request->send(200, "application/json", jsonbuffer);
          found = true;
        }
        if (!found)
        {
          ReturnInvMeasValue(request, APIParam);
        }
      }
      else
      {
        String msg = "No inverter with id : " + String(_InvId) + " found, please check your inverter configuration !";
        request->send(404, "text/plain", msg);
      }
    }
    else
    {
      String msg = "No inverter specified, please provide an inverter id !";
      request->send(404, "text/plain", msg);
    }
  }
  else
  {
    String msg = "No inverter configured, please configure inverter first !";
    request->send(404, "text/plain", msg);
  }
}

// All values from inverter in one json respons without units
void CreateJsonAll(void)
{
  jsonDocument.clear();
  float temp = 1.0;
  for (int i = 0; i < myAPIInverter->PrintParametersCount(); i++)
  {
    Serial.println(myAPIInverter->PrintParNameParameter(i));

    jsonDocument[myAPIInverter->PrintParNameParameter(i)] = myAPIInverter->PrintFormatedMeasParameter(i);
  }
  jsonDocument["Product"] = myAPIInverter->PrintProductNumber();
  jsonDocument["Serial"] = myAPIInverter->PrintSN();
  jsonDocument["OpModeTxt"] = myAPIInverter->PrintOpMode();
  serializeJson(jsonDocument, jsonbuffer);
}

void CreateJson(const char *tag, float value, const char *unit)
{
  jsonDocument.clear();
  jsonDocument["type"] = tag;
  jsonDocument["value"] = value;
  jsonDocument["unit"] = unit;
  serializeJson(jsonDocument, jsonbuffer);
}

void CreateJsonTxt(const char *tag, const char *value)
{
  jsonDocument.clear();
  jsonDocument["type"] = tag;
  jsonDocument["value"] = value;
  serializeJson(jsonDocument, jsonbuffer);
}

// Send json object of requested parameter/value. See inverter constructor for valid names
void ReturnInvMeasValue(AsyncWebServerRequest *request, String _APIParam)
{
  bool found = false;
  if (request->method() == HTTP_GET)
  {
    for (int i = 0; i < myAPIInverter->PrintParametersCount(); i++)
    {
      _APIParam.toUpperCase();
      String Name = myAPIInverter->PrintParNameParameter(i);
      Name.toUpperCase();
      if (_APIParam == Name)
      {
        found = true;
        CreateJson(myAPIInverter->PrintParNameParameter(i).c_str(), myAPIInverter->PrintMeasParameter(i), myAPIInverter->PrintUnitParameter(i).c_str());
        request->send(200, "application/json", jsonbuffer);
        break;
      }
    }
  }
  if (!found)
  {
    String msg = "Parameter not recognized \n";
    msg += "URI: ";
    msg += request->url();
    msg += "\nMethod: ";
    msg += (request->method() == HTTP_GET) ? "GET" : "POST";
    msg += "\nArguments: ";
    msg += request->args();
    msg += "\n";
    for (uint8_t i = 0; i < request->args(); i++)
    {
      msg += " " + request->argName(i) + ": " + request->arg(i) + "\n";
    }
    request->send(404, "text/plain", msg);
  }
}

void Init_Webserver()
{
  server->on("/", [](AsyncWebServerRequest *request) { request->send(200, "text/html", String(HTMLRoot)); });
  server->on("/inverter/*", HandleInverter);
  server->on("/inverters/scan/*", HandleInvertersScan);
  server->on("/inverters", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, HandlePostInverters);
  server->on("/inverters", HTTP_GET, HandleGetInverters);
  server->on("/settings/*", HandleSettings);
  server->on("/settings", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, HandlePostSettings);
  server->on("/settings", HTTP_GET, HandleSettings);
  server->on("/gw", HTTP_POST, HandleGW);
  server->on("/favicon.ico", HTTP_GET, HandleIcon);

  if (RestAPIOn) {
    server->on("/api/*", HandleRESTAPI);
  }
  server->onNotFound([](AsyncWebServerRequest *request)
    { String msg = "Page not found \n\n"; request->send(404, "text/plain", msg);});
  server->begin();
  Serial.println("HTTP server started");
}
