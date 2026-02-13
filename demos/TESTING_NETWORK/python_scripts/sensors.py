buffer_size = 10

#RUN TANK PT
|sensors/run_tank.pressure.value| = buffer_size
|sensors/run_tank.pressure.buffer_size| = 0
|sensors/run_tank.pressure.mapped_channel| = "labjackT7/analog_channels.AIN68"
|labjackT7/analog_channels.AIN68.negative_channel.value| = 76
|sensors/run_tank.pressure.scale| = 521    
|sensors/run_tank.pressure.offset| = -123

#INJECTOR PT
|sensors/injector.pressure.value| = 0
|sensors/injector.pressure.buffer_size| = buffer_size
|sensors/injector.pressure.mapped_channel| = "labjackT7/analog_channels.AIN112"
|labjackT7/analog_channels.AIN112.negative_channel.value| = 120
|sensors/injector.pressure.scale| = 521
|sensors/injector.pressure.offset| = -123

#CHAMBER PT
|sensors/chamber.pressure.value| = 0
|sensors/chamber.pressure.buffer_size| = buffer_size
|sensors/chamber.pressure.mapped_channel| = "labjackT7/analog_channels.AIN114"
|labjackT7/analog_channels.AIN114.negative_channel.value| = 122    
|sensors/chamber.pressure.scale| = 222
|sensors/chamber.pressure.offset| = -109

#nitrous supply PT
|sensors/nitrous_supply.pressure.value| = 0
|sensors/nitrous_supply.pressure.buffer_size| = buffer_size
|sensors/nitrous_supply.pressure.mapped_channel| = "labjackT7/analog_channels.AIN115"
|labjackT7/analog_channels.AIN115.negative_channel.value| = 123
|sensors/nitrous_supply.pressure.scale| = 222
|sensors/nitrous_supply.pressure.offset| = -109

#pressurant supply PT
|sensors/pressurant_supply.pressure.value| = 0
|sensors/pressurant_supply.pressure.buffer_size| = buffer_size
|sensors/pressurant_supply.pressure.mapped_channel| = "labjackT7/analog_channels.AIN113"
|labjackT7/analog_channels.AIN113.negative_channel.value| = 121
|sensors/pressurant_supply.pressure.scale| = 222
|sensors/pressurant_supply.pressure.offset| = -109

#load cell 
|sensors/load_cell.value| = 0
|sensors/load_cell.buffer_size| = buffer_size
|sensors/load_cell.mapped_channel| = "labjackT7/analog_channels.AIN70"
|labjackT7/analog_channels.AIN70.negative_channel.value| = 78
|sensors/load_cell.scale| = -156
|sensors/load_cell.offset| = 0

#nitrous scale - comes from modbus so does not need buffer
|sensors/nitrous_scale.value| = 0
|sensors/nitrous_scale.mapped_channel| = "PAD_WEIGHT/input_registers.0"
|sensors/nitrous_scale.scale| = 1/100
|sensors/nitrous_scale.offset| = -10000


