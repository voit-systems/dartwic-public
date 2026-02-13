import dartwic as d

## Default labjack settings to low sample rate

|TESTING_NETWORK:labjackT7/stream.target_scan_rate.value| = 10
|TESTING_NETWORK:labjackT7/stream.scans_per_read.value| = 10

|general/last_set_data_buffer.value| = 10

@d.on_function("set_buffer_values")
def set_buffer_values_function(args):

    #calc recommended buffer size
    target_scan_rate = |TESTING_NETWORK:labjackT7/stream.target_scan_rate.value|

    scans_per_read = |TESTING_NETWORK:labjackT7/stream.scans_per_read.value|

    #ex, 1000Hz data, 100 scans per read = data every 10 hz, and the data contains 100 values. 
    # so u can do buffer size of 100, match the scans_per_read

    requested_buffer_size = d.promptNumerical(
            "Set Buffer Values",
            "Set sensor buffer values to high to smoothe data.",
            ["TESTING_NETWORK:labjackT7/stream.scans_per_read.value"
            ],
            "Recommended buffer size is equal to number of scans per labjack read."
        )
    
    
    