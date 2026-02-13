import dartwic as d

#loop params
|TESTING_NETWORK:servervariables/loops.general_loop.target_loop_frequency.value| = 10.0
#set to run default
|TESTING_NETWORK:servervariables/loops.general_loop.running.value| = 1 

@d.on_loop("general_loop")
def loop():

    ### STATE CHECK ###
    if |TESTING_NETWORK:mission_clock/t_sign.value| == 1:
        #1 is T-, 0 is T+
        ambient = True
    else:
        ambient = False


    ### TANK LEVEL ###
    if ambient:
        #reads scale before mdot
        scale_mean = |sensors/load_cell.mean|
        |general/tank_level_scale.value| = scale_mean
    else:
        #always full - when mdot
        |general/tank_level_scale.value| = 10000 

    ### RUN TANK & NITROUS SUPPLY PRESSURE DIFF ###
    run_tank_pressure = |sensors/run_tank.pressure.mean|
    nitrous_pressure = |sensors/nitrous_supply.pressure.mean|
    diff = abs(run_tank_pressure - nitrous_pressure)
    |general/tank_pressure_diff.value| = diff

    ### Load Cell Absolute Value
    load_cell = |sensors/load_cell.mean|
    load_cell_abs = abs(load_cell)
    |general/load_cell_mean_abs.value| = load_cell_abs
