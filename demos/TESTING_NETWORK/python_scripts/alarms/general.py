import dartwic as d
from time import sleep

#loop params
|TESTING_NETWORK:servervariables/loops.general_alarms.target_loop_frequency.value| = 1
#set to run default
|TESTING_NETWORK:servervariables/loops.general_alarms.running.value| = 1 

last_stream_read_number = 0

@d.on_loop("general_alarms")
def loop():

    sleep(3) #seconds

    global last_stream_read_number

    ### LABJACK STREAM ###
    stream_read_number = |TESTING_NETWORK:labjackT7/stream.read_number.value|

    # if stream read number did not increase from last recorded one
    if (stream_read_number <= last_stream_read_number):
        d.warning(
            "LabJack Stream Read Number Warning",
            "The stream read number for the labjack is not increasing over a period of 3 seconds.",
            ["TESTING_NETWORK:labjackT7/stream.read_number.value"],
            "If the stream is stopped or if the labjack is disconnected, this is expected. If not, there is an issue.",
            5
        )
    else:
        last_stream_read_number = stream_read_number