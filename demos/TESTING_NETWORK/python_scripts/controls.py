import dartwic as d
import time

#bray valve
|controls/bray_valve.open.desired_state.value| = 0
|controls/bray_valve.open.desired_state.mapped_channel| = "labjackT7/digital_channels.DIO10.desired_state"
|controls/bray_valve.open.actual_state.value| = 0
|controls/bray_valve.open.actual_state.mapped_channel| = "labjackT7/digital_channels.DIO10.actual_state"

|controls/bray_valve.close.desired_state.value| = 0
|controls/bray_valve.close.desired_state.mapped_channel| = "labjackT7/digital_channels.DIO8.desired_state"
|controls/bray_valve.close.actual_state.value| = 0
|controls/bray_valve.close.actual_state.mapped_channel| = "labjackT7/digital_channels.DIO8.actual_state"

# 0 = closed; 1 = open
|controls/bray_valve.last_known_state.value| = 0

@d.on_function("toggle_bray_valve")
def toggle_bray_valve(args):

    #if last known was closed
    if |controls/bray_valve.last_known_state.value| == 0:
        |controls/bray_valve.last_known_state.value| = 1
        #pulse open
        |controls/bray_valve.open.desired_state.value| = 1
        time.sleep(0.3)
        |controls/bray_valve.open.desired_state.value| = 0
        

    #if last known was open
    else:
        |controls/bray_valve.last_known_state.value| = 0
        #pulse close
        |controls/bray_valve.close.desired_state.value| = 1
        time.sleep(0.3)
        |controls/bray_valve.close.desired_state.value| = 0
        


#starter propane valve
|controls/starter.propane_valve.desired_state.value| = 0
|controls/starter.propane_valve.desired_state.mapped_channel| = "PLC/coils.1.desired_state"
|controls/starter.propane_valve.actual_state.value| = 0
|controls/starter.propane_valve.actual_state.mapped_channel| = "PLC/coils.1.actual_state"


#starter air valve
|controls/starter.air_valve.desired_state.value| = 0
|controls/starter.air_valve.desired_state.mapped_channel| = "PLC/coils.0.desired_state"
|controls/starter.air_valve.actual_state.value| = 0
|controls/starter.air_valve.actual_state.mapped_channel| = "PLC/coils.0.actual_state"


#starter igniter coil
|controls/starter.igniter_coil.desired_state.value| = 0
|controls/starter.igniter_coil.desired_state.mapped_channel| = "PLC/coils.2.desired_state"
|controls/starter.igniter_coil.actual_state.value| = 0
|controls/starter.igniter_coil.actual_state.mapped_channel| = "PLC/coils.2.actual_state"


#siren
|controls/siren.desired_state.value| = 0
|controls/siren.desired_state.mapped_channel| = "PLC/coils.3.desired_state"
|controls/siren.actual_state.value| = 0
|controls/siren.actual_state.mapped_channel| = "PLC/coils.3.actual_state"


#pressurant fill valve
|controls/pressurant.fill_valve.open.desired_state.value| = 0
|controls/pressurant.fill_valve.open.desired_state.mapped_channel| = "labjackT7/digital_channels.DIO13.desired_state"
|controls/pressurant.fill_valve.open.actual_state.value| = 0
|controls/pressurant.fill_valve.open.actual_state.mapped_channel| = "labjackT7/digital_channels.DIO13.actual_state"

|controls/pressurant.fill_valve.close.desired_state.value| = 0
|controls/pressurant.fill_valve.close.desired_state.mapped_channel| = "labjackT7/digital_channels.DIO11.desired_state"
|controls/pressurant.fill_valve.close.actual_state.value| = 0
|controls/pressurant.fill_valve.close.actual_state.mapped_channel| = "labjackT7/digital_channels.DIO11.actual_state"

# 0 = closed; 1 = open
|controls/pressurant.fill_valve.last_known_state.value| = 0

@d.on_function("toggle_pressurant_fill_valve")
def toggle_pressurant_fill_valve(args):

    #if last known was closed
    if |controls/pressurant.fill_valve.last_known_state.value| == 0:
        |controls/pressurant.fill_valve.last_known_state.value| = 1
        #pulse open
        |controls/pressurant.fill_valve.open.desired_state.value| = 1
        time.sleep(0.3)
        |controls/pressurant.fill_valve.open.desired_state.value| = 0
        return

    #if last known was open
    else:
        |controls/pressurant.fill_valve.last_known_state.value| = 0
        #pulse close
        |controls/pressurant.fill_valve.close.desired_state.value| = 1
        time.sleep(0.3)
        |controls/pressurant.fill_valve.close.desired_state.value| = 0
        return


#pressurant vent valve
|controls/pressurant.vent_valve.desired_state.value| = 0
|controls/pressurant.vent_valve.desired_state.mapped_channel| = "labjackT7/digital_channels.DIO9.desired_state"
|controls/pressurant.vent_valve.actual_state.value| = 0
|controls/pressurant.vent_valve.actual_state.mapped_channel| = "labjackT7/digital_channels.DIO9.actual_state"


#nitrous fill valve
|controls/nitrous.fill_valve.desired_state.value| = 0
|controls/nitrous.fill_valve.desired_state.mapped_channel| = "labjackT7/digital_channels.DIO15.desired_state"
|controls/nitrous.fill_valve.actual_state.value| = 0
|controls/nitrous.fill_valve.actual_state.mapped_channel| = "labjackT7/digital_channels.DIO15.actual_state"


#nitrous vent valve
|controls/nitrous.vent_valve.desired_state.value| = 0
|controls/nitrous.vent_valve.desired_state.mapped_channel| = "labjackT7/digital_channels.DIO17.desired_state"
|controls/nitrous.vent_valve.actual_state.value| = 0
|controls/nitrous.vent_valve.actual_state.mapped_channel| = "labjackT7/digital_channels.DIO17.actual_state"