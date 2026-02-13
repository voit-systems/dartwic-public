import dartwic as d

'''
tags the sim uses
|sensors/nitrous_supply.pressure.mean| = 1000.0 
|sensors/run_tank.pressure.mean| = 0.0

|sensors/load_cell.mean| = 0.0
|sequences/autofill_sequence.run_tank_fill_target.value| = 0.0

|controls/nitrous.fill_valve.desired_state.value| = 0.0 
|controls/pressurant.vent_valve.desired_state.value| = 0.0
'''

#autofill params
|sequences/autofill_sequence.fill_command.value| = 1
#start filling once pressure diff is greater than: 
|sequences/autofill_sequence.fill_pressure_diff_setpoint.value| = 400
#start venting once pressure diff is less than:
|sequences/autofill_sequence.vent_pressure_diff_setpoint.value| = 100

#loop params
|servervariables/loops.autofill_sequence.target_loop_frequency.value| = 10.0
|servervariables/loops.autofill_sequence.running.value| = 0

# Global: time when loop started
start_time = None
accumulated_hold_time = 0
hold_start_time = None


@d.on_start("autofill_sequence")
def start():
    print("autofill sequence")
    global start_time, accumulated_hold_time

    # reset accumulated hold time on startup
    accumulated_hold_time = 0

    #if we wanna propogate hold to mclock, hold mclock also on startup due to prompts below
    if (|sequences/autofill_sequence.propogate_hold_to_mclock.value| == 1):
        |TESTING_NETWORK:mission_clock/hold.value| = 1

    ### ARMING WARNING ###
    if |sequences/autofill_sequence.arm.value| == 0:
        result = d.promptYesNo(
            "AUTOFILL SEQUENCE NOT ARMED",
            "The autofill sequence was started without being armed. Arm and continue?",
            ["TESTING_NETWORK:sequences/autofill_sequence.arm.value"],
            "Yes to arm and continue. No to cancel."
        )

        if result == 1:
            |sequences/autofill_sequence.arm.value| = 1
        else:
            |sequences/autofill_sequence.arm.value| = 0
            |servervariables/loops.autofill_sequence.running.value| = 0
            return

    ### ON START ###
    autofill_sequence_start()

    #if we wanna propogate hold to mclock, release the mclock hold after prompts have been answered
    if (|sequences/autofill_sequence.propogate_hold_to_mclock.value| == 1):
        d.releaseHold("MISSION CLOCK HOLD")

    ### TIME SINCE LOOP START ###
    start_time = time.time() 



@d.on_loop("autofill_sequence")
def loop():
    global start_time
    global accumulated_hold_time
    global hold_start_time


    if |sequences/autofill_sequence.arm.value| == 0:
        return

    ### HOLDS ###
    if int(|sequences/autofill_sequence.hold.value|) == 1:
        ### NEW — record hold start ###
        hold_start_time = time.time()

        #if we wanna propogate hold to mclock, hold mclock also
        if (|sequences/autofill_sequence.propogate_hold_to_mclock.value| == 1):
            |TESTING_NETWORK:mission_clock/hold.value| = 1

        d.hold(
            "AUTOFILL SEQUENCE HOLD",
            "The autofill sequence is on hold, all commands are halted.",
            ["TESTING_NETWORK:sequences/autofill_sequence.hold.value"],
            "Release hold to continue."
        )

        ### NEW — add hold duration to accumulated_hold_time ###
        accumulated_hold_time += time.time() - hold_start_time
        hold_start_time = None

        #if we wanna propogate hold to mclock, release the mclock hold
        if (|sequences/autofill_sequence.propogate_hold_to_mclock.value| == 1):
            d.releaseHold("MISSION CLOCK HOLD")

        |sequences/autofill_sequence.hold.value| = 0

    ### TIME SINCE LOOP START ###

    now = time.time()
    ### NEW — freeze dt during hold ###
    dt = (now - start_time) - accumulated_hold_time

    # write dt into tag
    |sequences/autofill_sequence.sequence_time.value| = dt

    ### SEQUENCE ###
    autofill_sequence_loop(dt)



@d.on_end("autofill_sequence")
def end():
    #if we wanna propogate hold to mclock, release the mclock hold
    if (|sequences/autofill_sequence.propogate_hold_to_mclock.value| == 1):
        d.releaseHold("MISSION CLOCK HOLD")


    if |sequences/autofill_sequence.arm.value| == 0:
        return

    ### ON END ###
    autofill_sequence_end()

    |sequences/autofill_sequence.arm.value| = 0
    



### SEQUENCE ###

#sequence start times
#evaluated by mission clock loop
# T- 20 min
|sequences/autofill_sequence.target_t_sign.value| = -1     
|sequences/autofill_sequence.target_t_hours.value| = 0
|sequences/autofill_sequence.target_t_minutes.value| = 20
|sequences/autofill_sequence.target_t_seconds.value| = 0

#to see current dt
|sequences/autofill_sequence.sequence_time.value| = 0

#for holds
|sequences/autofill_sequence.hold.value| = 0
|sequences/autofill_sequence.propogate_hold_to_mclock.value| = 0


#arming
|sequences/autofill_sequence.arm.value| = 0


def autofill_sequence_start():
    
    #ask test operators to set fill target
    fill_target = d.promptNumerical(
            "Run Tank Auto Fill Target",
            "Autofill sequence has started. Please enter fill target for run tank.",
            ["TESTING_NETWORK:sequences/autofill_sequence.run_tank_fill_target.value"],
            "Enter target."
        )

    #ensure fill target tag is set
    |sequences/autofill_sequence.run_tank_fill_target.value| = fill_target


def autofill_sequence_loop(dt):
    ## PRESSURE DIFF 
    run_tank_pressure = |sensors/run_tank.pressure.mean|
    nitrous_pressure = |sensors/nitrous_supply.pressure.mean|
    diff = abs(run_tank_pressure - nitrous_pressure)


    #HOLDS
    if |TESTING_NETWORK:tank_sim/autofill_hold.value| == 1:
        #safe system
        |controls/nitrous.fill_valve.desired_state.value| = 0
        |controls/pressurant.vent_valve.desired_state.value| = 0
        d.hold(
            "RUN TANK AUTO FILL HOLD",
            "A hold has been called on run rank auto fill sequence.",
            ["TESTING_NETWORK:tank_sim/autofill_hold.value"],
            "Evaluate and release hold."
        )
        #set hold to 0
        |TESTING_NETWORK:tank_sim/autofill_hold.value| = 0
        


    ### FILL COMMANDS ; vent = 1, fill = 2
    if |sequences/autofill_sequence.fill_command.value| == 1:
        #close supply fill and vent tank
        |controls/nitrous.fill_valve.desired_state.value| = 0
        |controls/pressurant.vent_valve.desired_state.value| = 1

        #start filling once pressure diff has reached greater than desired
        if diff >= |sequences/autofill_sequence.fill_pressure_diff_setpoint.value|:
            |sequences/autofill_sequence.fill_command.value| = 2


    if |sequences/autofill_sequence.fill_command.value| == 2:
        #fill tank and close vent
        |controls/nitrous.fill_valve.desired_state.value| = 1
        |controls/pressurant.vent_valve.desired_state.value| = 0


    ### SET NEW FILL COMMAND
    #set fill command
    #start venting once pressure diff has become less than desired
    if (diff <= |sequences/autofill_sequence.vent_pressure_diff_setpoint.value|):
        |sequences/autofill_sequence.fill_command.value| = 1


    ### END OF FILL
    #if fill target has been reached
    load_cell_mean = |sensors/load_cell.mean|
    if  abs(load_cell_mean) >= |sequences/autofill_sequence.run_tank_fill_target.value|:
        #stop loop
        |TESTING_NETWORK:servervariables/loops.autofill.running.value| = 0


def autofill_sequence_end():
    #once fill is complete, close valves
    |controls/nitrous.fill_valve.desired_state.value| = 0
    |controls/pressurant.vent_valve.desired_state.value| = 0

    |sequences/autofill_sequence.propogate_hold_to_mclock.value| = 0

    d.message(
            "Run Tank Auto Fill Complete",
            "Auto fill sequence for the run tank has completed, as run tank fill has reached target level.",
            ["TESTING_NETWORK:sensors/load_cell.mean", "TESTING_NETWORK:sequences/autofill_sequence.run_tank_fill_target.value"],
            "-"
        )


@d.on_function("force_autofill_command_switch")
def force_autofill_command_switch(args):
    ## PRESSURE DIFF 
    run_tank_pressure = |sensors/run_tank.pressure.mean|
    nitrous_pressure = |sensors/nitrous_supply.pressure.mean|
    diff = abs(run_tank_pressure - nitrous_pressure)

    #autofill params
    current_command = |sequences/autofill_sequence.fill_command.value|

    #if we are currently filling
    if (current_command == 2):
        #set vent pressure diff to current diff to force command change w/ some factor
        |sequences/autofill_sequence.vent_pressure_diff_setpoint.value| = diff + 10

    #if we are currently venting
    else:
        #set fill pressure diff to current diff to foce a command swap
        |sequences/autofill_sequence.fill_pressure_diff_setpoint.value| = diff - 10
