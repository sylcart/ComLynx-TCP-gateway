# Comlynx TCP gateway

Revision 0.4

This program is a gateway beetween Dansfoss Inverters (TLX and ULX) and TCP networks.
  It use the Danfoss comlynx protocol (over RS485) to query the inverters parameters.

  Many parameters can be setted by the web interface (wifi, Networking, Comlynx and MQTT).

  The scan function of the comlynx interface can discover interters connected to the RS485 bus.

  This gateway can send inverters values to an MQTT broker or offer a REST API interface to get these values.

  Both ways can be used with Homeassistant or other systems to monitor and display the Danfoss inverters production.

  MQTT is the easiest way because of the Home assistant MQTT auto-discovery function.

Based on DanfossTLX by AMajland https://github.com/AMajland/Danfoss-TLX

Itself based on TLX-ino by Torben https://github.com/TorbenNor/Danfoss-TLX

It can be used with Home Assistant. In case of HA Integration using REST API, each parameter/value can be read as a sensor.

Each parameter can be queried separately (see DanfossInverter.h for parameters list)

    sensor:
      # Danfoss TLX
      # Energy
      - platform: rest
        resource: http://[IP-Address]/api/[inverter Id]/TotalE
        name: "TLX Total Energy Production"
        method: GET
        value_template: "{{ value_json.value |round(3)}}"
        unit_of_measurement: kWh
        device_class: 'energy'
        state_class: total_increasing
            
To get all parameters of an inverter in one call. It is possible to call the All API wich return all paramters in on json document. See configuration.yaml for all rest sensors

    rest:
      - resource: http://[IP-Address]/api/[inverter Id]/All
        sensor:
          # Info
          - name: "TLX Operation Mode"
            value_template: "{{ value_json.OpMode }}"
          - name: "TLX Operation Mode Text"
            value_template: "{{ value_json.OpModeTxt }}"
          - name: "TLX Product Number"
            value_template: "{{ value_json.Product }}"
          - name: "TLX Serial" 
           value_template: "{{ value_json.Serial }}"        
          # Produkction Energy
          - name: "TLX Total Energy Production"
            value_template: "{{ value_json.TotalE |round(3)}}"
            unit_of_measurement: kWh
            device_class: 'energy'
            state_class: total_increasing
            ...
            ...
         

Material used with this project    

Board ESP32-WROOM-32U with external wifi antenna (choose LOLIN D32 as dev board)          
https://fr.aliexpress.com/item/1005006699362972.html

or

Board LOLIN D32                                       
https://www.aliexpress.com/item/32808551116.html

  Pin 16  :RXD2 Serial in
  Pin 17  :TXD2 Serial out

and

R411A01 mini 3.3V Auto RS485 to TTL232 Converter Board  
https://www.aliexpress.com/item/32782552104.html

See all libraries dependencies in main ino file.

# BOM:
esp32 board

R411A01 mini 3.3V Auto RS485 to TTL232 Converter Board or similar.

# WIRING:
See the schema for connecting all devices and the inverters. Please note that termination is mandatory on last inverter of the bus. (no termination on R411A01's side).

<img  alt="Wiring Schema" src="/doc/comlynx TCP gateway wiring schema.png">

By using RJ45 connectors on inverter's side, color code may vary, I respected the danfoss's color code ou schema by in my case, orange and white/orange were ground and green and white/green were datas.
Important is to cross the data wire between inverters and R411A01.

Pay attention to use a 3.3V R411A01, it exists in both 3.3V and 5V. If using a 5V, schema should be modified to use the 5V pin of the ESP32.

# Web interface:

root webpage is basic to redirect to other pages :

<img  alt="root page" src="/doc/Webpages/root page.png">

system page give some general informations :

<img  alt="root page" src="/doc/Webpages/system info page.png">

use this page to configure wifi credentials :

<img  alt="root page" src="/doc/Webpages/wifi configuration page.png">

network settings can be configured on this page (here you can enable REST API) :

<img  alt="root page" src="/doc/Webpages/network settings page.png">

Discover your inverters (by scanning RS485 network or manually) and comlynx parameters on this page :

<img  alt="root page" src="/doc/Webpages/comlynx configuration page.png">

Enable MQTT and home assistant MQTT auto-discovery on this page :

<img  alt="root page" src="/doc/Webpages/MQTT configuration page.png">

Enable MQTT and home assistant MQTT auto-discovery on this page :

<img  alt="root page" src="/doc/Webpages/MQTT configuration page.png">

View each configured inverter on this page :

<img  alt="root page" src="/doc/Webpages/All inverter parameters page.png">

# REST API:

After enabling REST API you can get one inverter's metrics :

<img  alt="root page" src="/doc/REST API examples/REST API all parameters.png">

or get one metric only (using his name) :

<img  alt="root page" src="/doc/REST API examples/REST API one parameter.png">
