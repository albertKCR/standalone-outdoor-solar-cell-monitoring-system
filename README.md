# Standalone outdoor solar cell monitoring-system
Software used to send data from an ESP32 or Arduino+ESP8266 to a google spreadsheet, where the data is processed and displayed on a web page.<br>
When using the ESP32 it receives the values of a measurement through an ADC by SPI communication and sends this data to a spreadsheet through google script deployments. After receiving the data, it is processed and displayed in the form of graphics on a web page.<br>
When using Arduino+ESP8266, the Arduino receives the data through the ADC and sends it to the ESP8266 through UART communication, finally the ESP8266 sends it to the google spreadsheet.<br>
<a href="https://docs.google.com/spreadsheets/d/1as9wJFIINmXNEiZtwQfexIcdRDS1L1k5qA1dt9N_czA/edit?usp=sharing">Example of google spreadsheet using the scripts.</a><br>
<a href="https://albertkcr.github.io/standalone-outdoor-solar-cell-monitoring-system/">Charts page web page (under construction).</a>
