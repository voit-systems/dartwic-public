import dartwic as d

|checks/labjackT7.data_record_checks.value| = 0
@d.on_function("labjack_data_record_checks")
def labjack_data_record_checks(args):

    target_scan_rate = d.promptNumerical(
            "Set LabJack Stream Target Scan Rate",
            "Set labjack stream target scan rate to 1000Hz.",
            ["TESTING_NETWORK:labjackT7/stream.target_scan_rate.value",
            "TESTING_NETWORK:checks/labjackT7.data_record_checks.value"
            ],
            "Set Target Scan Rate to 1000Hz, for high frequency data."
        )
    |TESTING_NETWORK:labjackT7/stream.target_scan_rate.value| = target_scan_rate

    scans_per_read = d.promptNumerical(
            "Set LabJack Stream Scans Per Read",
            "Set labjack stream scans per read to 10.",
            ["TESTING_NETWORK:labjackT7/stream.scans_per_read.value",
            "TESTING_NETWORK:labjackT7/stream.actual_scan_rate.value",
            "TESTING_NETWORK:checks/labjackT7.data_record_checks.value"
            ],
            "Set Scans Per Read to 10 for 100Hz throughput, if actual target scan rate is 1000Hz"
        )
    |TESTING_NETWORK:labjackT7/stream.scans_per_read.value| = scans_per_read

    stream_started = d.promptYesNo(
            "Confirm LabJack Stream recieving data.",
            "Confirm LabJack Stream recieving data by the increasing stream read number.",
            ["TESTING_NETWORK:labjackT7/stream.read_number.value", 
            "TESTING_NETWORK:checks/labjackT7.data_record_checks.value"
            ],
            "Is stream read number increasing? If not, this check fails and you need to troubleshoot. View controls on the LabJack Schematic."
        )
    if (stream_started != 1):
        return

    #passed
    |checks/labjackT7.data_record_checks.value| = 1

    
