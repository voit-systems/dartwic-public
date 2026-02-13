from datetime import datetime, timedelta
import dartwic as d


### LOOP
# start clock
|TESTING_NETWORK:servervariables/loops.mission_clock.running.value| = 1
|TESTING_NETWORK:servervariables/loops.mission_clock.target_loop_frequency.value| = 10

# abort / hold
|TESTING_NETWORK:mission_clock/hold.value| = 0
hold_active = False
hold_start = None

# target tags
|TESTING_NETWORK:mission_clock/target_hour.value| = 23
|TESTING_NETWORK:mission_clock/target_minute.value| = 59
|TESTING_NETWORK:mission_clock/target_second.value| = 0

# T-time tags
|TESTING_NETWORK:mission_clock/t_time.value| = 0
|TESTING_NETWORK:mission_clock/t_hours.value| = 0
|TESTING_NETWORK:mission_clock/t_minutes.value| = 0
|TESTING_NETWORK:mission_clock/t_seconds.value| = 0
|TESTING_NETWORK:mission_clock/t_sign.value| = 1

prev_hour   = int(|TESTING_NETWORK:mission_clock/target_hour.value|)
prev_minute = int(|TESTING_NETWORK:mission_clock/target_minute.value|)
prev_second = int(|TESTING_NETWORK:mission_clock/target_second.value|)

# compute difference between now and target
def compute_t_time(hour, minute, second):
    now = datetime.now()
    target = now.replace(hour=int(hour), minute=int(minute), second=int(second), microsecond=0)
    delta = now - target
    return int(delta.total_seconds())    # negative when target is in the future

# --- NEW ---
loop_start = None          # when mission_clock loop starts running
t_offset   = None          # the initial t at start()
# ------------

@d.on_start("mission_clock")
def start():
    sequence_checks_start()
    global prev_hour, prev_minute, prev_second, t_offset, loop_start

    prev_hour   = int(|TESTING_NETWORK:mission_clock/target_hour.value|)
    prev_minute = int(|TESTING_NETWORK:mission_clock/target_minute.value|)
    prev_second = int(|TESTING_NETWORK:mission_clock/target_second.value|)

    # initial t
    t_offset = compute_t_time(prev_hour, prev_minute, prev_second)

    # record when loop begins
    loop_start = datetime.now()

    |TESTING_NETWORK:mission_clock/t_sign.value| = -1 if t_offset < 0 else 1


@d.on_loop("mission_clock")
def loop():
    global prev_hour, prev_minute, prev_second
    global hold_active, hold_start
    global t_offset, loop_start

    now = datetime.now()

    # --- COMPUTE T DIRECTLY FROM REAL TIME ---
    elapsed = (now - loop_start).total_seconds()
    t = int(t_offset + elapsed)
    # -----------------------------------------

    # check target changes
    current_hour   = int(|TESTING_NETWORK:mission_clock/target_hour.value|)
    current_minute = int(|TESTING_NETWORK:mission_clock/target_minute.value|)
    current_second = int(|TESTING_NETWORK:mission_clock/target_second.value|)

    if (current_hour   != prev_hour or
        current_minute != prev_minute or
        current_second != prev_second):

        prev_hour   = current_hour
        prev_minute = current_minute
        prev_second = current_second

        # recompute T from now
        t_offset = compute_t_time(current_hour, current_minute, current_second)
        loop_start = now    # restart loop timer

    # hold logic
    if int(|TESTING_NETWORK:mission_clock/hold.value|) == 1 and not hold_active:
        hold_active = True
        hold_start = now

        d.hold(
            "MISSION CLOCK HOLD",
            "The mission clock is on hold.",
            ["TESTING_NETWORK:mission_clock/hold.value"],
            "Release hold to continue."
        )

        # apply hold shift
        hold_duration = datetime.now() - hold_start
        adjusted = now.replace(hour=current_hour, minute=current_minute, second=current_second, microsecond=0) + hold_duration

        |TESTING_NETWORK:mission_clock/target_hour.value|   = adjusted.hour
        |TESTING_NETWORK:mission_clock/target_minute.value| = adjusted.minute
        |TESTING_NETWORK:mission_clock/target_second.value| = adjusted.second

        # refresh timing bases
        t_offset = compute_t_time(adjusted.hour, adjusted.minute, adjusted.second)
        loop_start = datetime.now()

        hold_active = False
        |TESTING_NETWORK:mission_clock/hold.value| = 0

    # update tags
    t_sign  = -1 if t < 0 else 1
    at      = abs(t)
    hours   = at // 3600
    minutes = (at % 3600) // 60
    seconds = at % 60

    |TESTING_NETWORK:mission_clock/t_sign.value|   = t_sign
    |TESTING_NETWORK:mission_clock/t_hours.value|  = hours
    |TESTING_NETWORK:mission_clock/t_minutes.value|= minutes
    |TESTING_NETWORK:mission_clock/t_seconds.value|= seconds

    sequence_checks(t_sign, hours, minutes, seconds)


### SEQUENCE CHECKS

# 1 = setup, 2 = checks, 3 = fill, 4 = ignition
|mission_clock/current_mission_state.value| = 1

|mission_clock/autofill_triggered.value| = 0
|mission_clock/igniter_triggered.value| = 0

def sequence_checks_start():
    |mission_clock/autofill_triggered.value| = 0
    |mission_clock/igniter_triggered.value| = 0   


def sequence_checks(t_sign, h, m, s):

    # Convert current to absolute seconds
    current_total = t_sign * (h*3600 + m*60 + s)


    ### AUTO FILL
    # Load target
    ts   = |sequences/autofill_sequence.target_t_sign.value|
    th   = |sequences/autofill_sequence.target_t_hours.value|
    tm   = |sequences/autofill_sequence.target_t_minutes.value|
    tsec = |sequences/autofill_sequence.target_t_seconds.value|

    # Convert target to absolute seconds
    target_total  = ts * (th*3600 + tm*60 + tsec)

    # *** SINGLE IF STATEMENT ***
    if (current_total >= target_total) and (not |mission_clock/autofill_triggered.value|):
        |mission_clock/autofill_triggered.value| = 1
        |mission_clock/current_mission_state.value| = 3

        #force mclock hold propogation
        |sequences/autofill_sequence.propogate_hold_to_mclock.value| = 1        
        #start autofill sequence
        |servervariables/loops.autofill_sequence.running.value| = 1



    ### IGNITER
    # Load target
    ts   = |sequences/igniter_sequence.target_t_sign.value|
    th   = |sequences/igniter_sequence.target_t_hours.value|
    tm   = |sequences/igniter_sequence.target_t_minutes.value|
    tsec = |sequences/igniter_sequence.target_t_seconds.value|

    # Convert target to absolute seconds
    target_total  = ts * (th*3600 + tm*60 + tsec)

    # *** SINGLE IF STATEMENT ***
    if (current_total >= target_total) and (not |mission_clock/igniter_triggered.value|):
        |mission_clock/igniter_triggered.value| = 1
        |mission_clock/current_mission_state.value| = 4

        #force mclock hold propogation
        |sequences/igniter_sequence.propogate_hold_to_mclock.value| = 1        
        #start igniter sequence
        |servervariables/loops.igniter_sequence.running.value| = 1



