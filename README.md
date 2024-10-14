# cell_owd_control
Seeeduino Nano relay control
Required hardware: Seeeduino Nano, LB GPIO-32ch-Rev-B, LB Cell Voltage OC. Cell Temperature OC Board

## Use the Seeeduino Legacy board overlay package on Arduino IDE

## How to use (WIP, need a new board to add CMU18)
Command structure: on:# to open channel

|Channel|MCP23017 Address|Register|Bit Position|Descriptor|
|:----|:----|:----|:----|:----|
|1|0x20|GPIOA|0|C1-|
|2|0x20|GPIOA|1|C1+|
|3|0x20|GPIOA|2|C2+|
|4|0x20|GPIOA|3|C3+|
|5|0x20|GPIOA|4|C4+|
|6|0x20|GPIOA|5|C5+|
|7|0x20|GPIOA|6|C6+|
|8|0x20|GPIOA|7|LTC GND|
|9|0x20|GPIOB|0|C8+|
|10|0x20|GPIOB|1|C9+|
|11|0x20|GPIOB|2|C10+|
|12|0x20|GPIOB|3|C11+|
|13|0x20|GPIOB|4|C12+|
|14|0x20|GPIOB|5|LTC PWR|
|15|0x20|GPIOB|6|N/A|
|16|0x20|GPIOB|7|C7+|
|17|0x21|GPIOA|0|T2 OC|
|18|0x21|GPIOA|1|T3 OC|
|19|0x21|GPIOA|2|T4 OC|
|20|0x21|GPIOA|3|T5 OC|
|21|0x21|GPIOA|4|T6 OC|
|22|0x21|GPIOA|5|T1 SC|


### Special commands
To reset all channels: on:35