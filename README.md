# Danfoss-TLX
    RS485 Communication with Danfoss TLX solar inverter and REST server

    I use it for Home Assistant intergration where each parameter/value can be read as a sensor.
    See DanfossTLX-RS485.h for parameters
    sensor:
      # Danfoss TLX
        # Energy
      - platform: rest
        resource: http://[IP-Address]/TotalE
        name: "TLX Total Energy Production"
        method: GET
        value_template: "{{ value_json.value |round(3)}}"
        unit_of_measurement: kWh
        device_class: 'energy'
        state_class: total_increasing

    Based on TLX-ino by Torben https://github.com/TorbenNor/Danfoss-TLX

    Board LOLIN D32                                        
    https://www.aliexpress.com/item/32808551116.htm
      Pin 16  :RXD2 Seriel in
      Pin 17  :TXD2 Seriel out
    R411A01 mini 3.3V Auto RS485 to TTL232 Converter Board  
    https://www.aliexpress.com/item/32782552104.html



    Used Libraries - output from verbose compile
    Multiple libraries were found for "WiFi.h" Used:       ...\packages\esp32\hardware\esp32\2.0.3\libraries\WiFi
    Using library WiFi at version 2.0.0 in folder:         ...\packages\esp32\hardware\esp32\2.0.3\libraries\WiFi 
    Using library WebServer at version 2.0.0 in folder:    ...\packages\esp32\hardware\esp32\2.0.3\libraries\WebServer 
    Using library ArduinoJson at version 6.19.4 in folder: ...\libraries\ArduinoJson 
    Using library FS at version 2.0.0 in folder:           ...\packages\esp32\hardware\esp32\2.0.3\libraries\FS 



# BOM:
esp32 lolin.

R411A01 mini 3.3V Auto RS485 to TTL232 Converter Board or similar.


