import dartwic as d
import time

#loop params
|servervariables/loops.igniter_sequence.target_loop_frequency.value| = 10.0
|servervariables/loops.igniter_sequence.running.value| = 0

# Global: time when loop started
start_time = None
accumulated_hold_time = 0
hold_start_time = None


@d.on_start("igniter_sequence")
def start():
    global start_time
    global accumulated_hold_time

    # reset accumulated hold time on startup
    accumulated_hold_time = 0

    #if we wanna propogate hold to mclock, hold mclock also on startup due to prompts below
    if (|sequences/igniter_sequence.propogate_hold_to_mclock.value| == 1):
        |TESTING_NETWORK:mission_clock/hold.value| = 1

    ### ARMING WARNING ###
    if |sequences/igniter_sequence.arm.value| == 0:
        result = d.promptYesNo(
            "IGNITION SEQUENCE NOT ARMED",
            "The ignition sequence was started without being armed. Arm and continue?",
            ["TESTING_NETWORK:sequences/igniter_sequence.arm.value"],
            "Yes to arm and continue. No to cancel."
        )

        if result == 1:
            |sequences/igniter_sequence.arm.value| = 1
        else:
            |sequences/igniter_sequence.arm.value| = 0
            |servervariables/loops.igniter_sequence.running.value| = 0
            return  

    ### ON START ###
    ignition_sequence_start()

    #if we wanna propogate hold to mclock, release the mclock hold after prompts have been answered
    if (|sequences/igniter_sequence.propogate_hold_to_mclock.value| == 1):
        d.releaseHold("MISSION CLOCK HOLD")

    ### TIME SINCE LOOP START ###
    start_time = time.time() 



@d.on_loop("igniter_sequence")
def loop():
    global start_time
    global accumulated_hold_time
    global hold_start_time

    if |sequences/igniter_sequence.arm.value| == 0:
        return

    ### HOLDS ###
    if int(|sequences/igniter_sequence.hold.value|) == 1:

        ### NEW — record hold start ###
        hold_start_time = time.time()

        #if we wanna propogate hold to mclock, hold mclock also
        if (|sequences/igniter_sequence.propogate_hold_to_mclock.value| == 1):
            |TESTING_NETWORK:mission_clock/hold.value| = 1

        d.hold(
            "IGNITION SEQUENCE HOLD",
            "The ignition sequence is on hold, all commands are halted.",
            ["TESTING_NETWORK:sequences/igniter_sequence.hold.value"],
            "Release hold to continue."
        )

        ### NEW — add hold duration to accumulated_hold_time ###
        accumulated_hold_time += time.time() - hold_start_time
        hold_start_time = None

        #if we wanna propogate hold to mclock, release the mclock hold
        if (|sequences/igniter_sequence.propogate_hold_to_mclock.value| == 1):
            d.releaseHold("MISSION CLOCK HOLD")

        |sequences/igniter_sequence.hold.value| = 0

    ### TIME SINCE LOOP START ###

    now = time.time()

    ### NEW — freeze dt during hold ###
    dt = (now - start_time) - accumulated_hold_time

    # write dt into tag
    |sequences/igniter_sequence.sequence_time.value| = dt

    ### SEQUENCE ###
    ignition_ignition_sequence_loop(dt)



@d.on_end("igniter_sequence")
def end():
    #if we wanna propogate hold to mclock, release the mclock hold
    if (|sequences/igniter_sequence.propogate_hold_to_mclock.value| == 1):
        d.releaseHold("MISSION CLOCK HOLD")

    if |sequences/igniter_sequence.arm.value| == 0:
        return

    ### ON END ###
    ignition_ignition_sequence_end()

    |sequences/igniter_sequence.arm.value| = 0
    



### SEQUENCE ###

#sequence start times
#evaluated by mission clock loop
# T- 40 seconds
|sequences/igniter_sequence.target_t_sign.value| = -1     
|sequences/igniter_sequence.target_t_hours.value| = 0
|sequences/igniter_sequence.target_t_minutes.value| = 0
|sequences/igniter_sequence.target_t_seconds.value| = 40

#to see current dt
|sequences/igniter_sequence.sequence_time.value| = 0

#for holds
|sequences/igniter_sequence.hold.value| = 0
|sequences/igniter_sequence.propogate_hold_to_mclock.value| = 0


#arming
|sequences/igniter_sequence.arm.value| = 0

|controls/bray_valve.arm.value| = 0


# one-shot flags
propane_on = False
sparker_off = False
propane_off = False
air_off = False

def ignition_sequence_start():
    global propane_on, sparker_off, propane_off, air_off

    propane_on = False
    sparker_off = False
    propane_off = False
    air_off = False

    #BRAY VALVE ARMING
    if (|controls/bray_valve.arm.value| == 0):
        result = d.promptYesNo(
            "BrAY VALVE NOT ARMED",
            "The ignition sequence was started without the bray valve being armed.",
            ["TESTING_NETWORK:controls/bray_valve.arm.value"],
            "Would you like to arm the valve before continuing?."
        )

        #1 for yes, 0 for no
        |controls/bray_valve.arm.value| = result

    #turn on sparker and air
    |controls/starter.igniter_coil.desired_state.value| = 1

    |controls/starter.air_valve.desired_state.value| = 1

    #turn siren on
    |controls/siren.desired_state.value| = 1

def ignition_ignition_sequence_loop(dt):
    global propane_on, sparker_off, propane_off, air_off

    # T+5 seconds → turn on propane ONCE
    if dt >= 5 and not propane_on:
        propane_on = True
        |controls/starter.propane_valve.desired_state.value| = 1

    # T+10 seconds → spark off ONCE & siren off
    if dt >= 10 and not sparker_off:
        sparker_off = True
        |controls/starter.igniter_coil.desired_state.value| = 0
        |controls/siren.desired_state.value| = 0


    # T+30 seconds → propane OFF ONCE
    if dt >= 30 and not propane_off:
        propane_off = True
        |controls/starter.propane_valve.desired_state.value| = 0

    # T+40 seconds → shut air & stop loop ONCE & TURN ON BRAY VALVE
    if dt >= 40 and not air_off:
        air_off = True
        |controls/starter.air_valve.desired_state.value| = 1
        |servervariables/loops.igniter_sequence.running.value| = 0

        #toggle bray valve
        if (|controls/bray_valve.arm.value| == 1):
            |controls/bray_valve.last_known_state.value| = 1
            #pulse open
            |controls/bray_valve.open.desired_state.value| = 1
            time.sleep(0.3)
            |controls/bray_valve.open.desired_state.value| = 0



def ignition_ignition_sequence_end():
    #safe system
    |controls/starter.igniter_coil.desired_state.value| = 0
    |controls/starter.air_valve.desired_state.value| = 0
    |controls/starter.propane_valve.desired_state.value| = 0

    |sequences/igniter_sequence.propogate_hold_to_mclock.value| = 0
    |controls/bray_valve.arm.value| = 0

    



