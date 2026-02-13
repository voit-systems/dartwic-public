import dartwic as d

#loop params
|servervariables/loops.control_checks.target_loop_frequency.value| = 10.0
#default not running - set to 1 outside of script to run autofill
|servervariables/loops.control_checks.running.value| = 1 

@d.on_loop("control_checks")
def loop():

    # when pressures equalize
    arg = 1
    