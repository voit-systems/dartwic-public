import dartwic as d

ambient = True

#loop params
|servervariables/loops.sensor_checks.target_loop_frequency.value| = 10.0
#default not running - set to 1 outside of script to run autofill
|servervariables/loops.sensor_checks.running.value| = 1 

@d.on_loop("sensor_checks")
def loop():

    ### STATE CHECK ###
    if |mission_clock/t_sign.value| == 1:
        #1 is T-, 0 is T+
        ambient = True
    else:
        ambient = False

    ### RANGE DEFINITION ###
    if ambient: 
        #during everyday conditions
        run_tank_pressure_high = 30
        run_tank_pressure_low = -30

        injector_pressure_high = 30
        injector_pressure_low = -30

        chamber_pressure_high = 30
        chamber_pressure_low = -30

        oxidizer_supply_high = 30
        oxidizer_supply_low = -30

        pressurant_supply_high = 30
        pressurant_supply_low = -30

        load_cell_high = 30
        load_cell_low = -30

        nitrous_scale_high = 30
        nitrous_scale_low = -30

    else:
        #during engine running
        run_tank_pressure_high = 1000
        run_tank_pressure_low = -30

        injector_pressure_high = 1000
        injector_pressure_low = -30

        chamber_pressure_high = 1000
        chamber_pressure_low = -30

        oxidizer_supply_high = 1500
        oxidizer_supply_low = -30

        pressurant_supply_high = 1500
        pressurant_supply_low = -30

        load_cell_high = 800
        load_cell_low = -30

        nitrous_scale_high = 1000
        nitrous_scale_low = -1000
        


    ### CHECKS ###
    # when pressures equalize
    if |sensors/run_tank.pressure.value| >= (|sensors/nitrous_supply.pressure.value|*0.9):
        d.warning(  
            "Fill Pressure Warning",
            "The run tank has reached around the same pressure as the supply tank, reducing the ability to fill efficiently.",
            ["sensors/nitrous_supply.pressure.value",
                "sensors/run_tank.pressure.value"
            ],
            "Vent the run tank slightly to remove some pressure.",
            5
        )

    # run tank PT
    if |sensors/run_tank.pressure.value| > run_tank_pressure_high or |sensors/run_tank.pressure.value| < run_tank_pressure_low:
        d.warning(
            "Run Tank Pressure Warning",
            "The run tank pressure is outside of specified bounds.",
            ["sensors/run_tank.pressure.value"],
            "Either an engine, sensor, or DAQ malfunction. Check systems.",
            5
        )

    # injector PT
    if |sensors/injector.pressure.value| > injector_pressure_high or |sensors/injector.pressure.value| < injector_pressure_low:
        d.warning(
            "Injector Pressure Warning",
            "The injector pressure is outside of specified bounds.",
            ["sensors/injector.pressure.value"],
            "Either an engine, sensor, or DAQ malfunction. Check systems.",
            5
        )

    # chamber PT
    if |sensors/chamber.pressure.value| > chamber_pressure_high or |sensors/chamber.pressure.value| < chamber_pressure_low:
        d.warning(
            "Chamber Pressure Warning",
            "The chamber pressure is outside of specified bounds.",
            ["sensors/chamber.pressure.value"],
            "Either an engine, sensor, or DAQ malfunction. Check systems.",
            5
        )

    # nitrous_supply PT
    if |sensors/nitrous_supply.pressure.value| > nitrous_scale_high or |sensors/nitrous_supply.pressure.value| < nitrous_scale_low:
        d.warning(
            "Nitrous Supply Pressure Warning",
            "The nitrous supply tank pressure is outside of specified bounds.",
            ["sensors/nitrous_supply.pressure.value"],
            "Either an engine, sensor, or DAQ malfunction. Check systems.",
            5
        )

    # pressurant_supply PT
    if |sensors/pressurant_supply.pressure.value| > pressurant_supply_high or |sensors/pressurant_supply.pressure.value| < pressurant_supply_low:
        d.warning(
            "Pressurant Supply Pressure Warning",
            "The pressurant supply tank pressure is outside of specified bounds.",
            ["sensors/pressurant_supply.pressure.value"],
            "Either an engine, sensor, or DAQ malfunction. Check systems.",
            5
        )

    # load cell 
    if |sensors/load_cell.value| > load_cell_high or |sensors/load_cell.value| < load_cell_low:
        d.warning(
            "Load Cell Warning",
            "The test cell load cell is outside of specified bounds.",
            ["sensors/load_cell.value"],
            "Either an engine, sensor, or DAQ malfunction. Check systems.",
            5
        )

    # load cell PT
    if |sensors/nitrous_scale.value| > nitrous_scale_high or |sensors/nitrous_scale.value| < nitrous_scale_low:
        d.warning(
            "Nitrous Scale Warning",
            "The nitrous scale is outside of specified bounds.",
            ["sensors/nitrous_scale.value"],
            "Either an engine, sensor, or DAQ malfunction. Check systems.",
            5
        )

    