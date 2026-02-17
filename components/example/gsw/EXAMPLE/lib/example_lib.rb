# Library for EXAMPLE Target
require 'cosmos'
require 'cosmos/script'

#
# Definitions
#
EXAMPLE_CMD_SLEEP = 0.25
EXAMPLE_RESPONSE_TIMEOUT = 5
EXAMPLE_TEST_LOOP_COUNT = 1
EXAMPLE_DEVICE_LOOP_COUNT = 5

#
# Functions
#
def get_example_hk()
    cmd("EXAMPLE EXAMPLE_REQ_HK")
    wait_check_packet("EXAMPLE", "EXAMPLE_HK_TLM", 1, EXAMPLE_RESPONSE_TIMEOUT)
    sleep(EXAMPLE_CMD_SLEEP)
end

def get_example_data()
    cmd("EXAMPLE EXAMPLE_REQ_DATA")
    wait_check_packet("EXAMPLE", "EXAMPLE_DATA_TLM", 1, EXAMPLE_RESPONSE_TIMEOUT)
    sleep(EXAMPLE_CMD_SLEEP)
end

def example_cmd(*command)
    count = tlm("EXAMPLE EXAMPLE_HK_TLM CMD_COUNT") + 1

    if (count == 256)
        count = 0
    end

    cmd(*command)
    get_example_hk()
    current = tlm("EXAMPLE EXAMPLE_HK_TLM CMD_COUNT")
    if (current != count)
        # Try again
        cmd(*command)
        get_example_hk()
        current = tlm("EXAMPLE EXAMPLE_HK_TLM CMD_COUNT")
        if (current != count)
            # Third times the charm
            cmd(*command)
            get_example_hk()
            current = tlm("EXAMPLE EXAMPLE_HK_TLM CMD_COUNT")
        end
    end
    check("EXAMPLE EXAMPLE_HK_TLM CMD_COUNT >= #{count}")
end

def enable_example()
    # Send command
    example_cmd("EXAMPLE EXAMPLE_ENABLE_CC")
    # Confirm
    check("EXAMPLE EXAMPLE_HK_TLM DEVICE_ENABLED == 'ENABLED'")
end

def disable_example()
    # Send command
    example_cmd("EXAMPLE EXAMPLE_DISABLE_CC")
    # Confirm
    check("EXAMPLE EXAMPLE_HK_TLM DEVICE_ENABLED == 'DISABLED'")
end

def safe_example()
    get_example_hk()
    state = tlm("EXAMPLE EXAMPLE_HK_TLM DEVICE_ENABLED")
    if (state != "DISABLED")
        disable_example()
    end
end

def confirm_example_data()
    dev_cmd_cnt = tlm("EXAMPLE EXAMPLE_HK_TLM DEVICE_COUNT")
    dev_cmd_err_cnt = tlm("EXAMPLE EXAMPLE_HK_TLM DEVICE_ERR_COUNT")
    
    get_example_data()
    # Note these checks assume default simulator configuration
    raw_x = tlm("EXAMPLE EXAMPLE_DATA_TLM RAW_EXAMPLE_X")
    check("EXAMPLE EXAMPLE_DATA_TLM RAW_EXAMPLE_Y >= #{raw_x*2}")
    check("EXAMPLE EXAMPLE_DATA_TLM RAW_EXAMPLE_Z >= #{raw_x*3}")

    get_example_hk()
    check("EXAMPLE EXAMPLE_HK_TLM DEVICE_COUNT >= #{dev_cmd_cnt}")
    check("EXAMPLE EXAMPLE_HK_TLM DEVICE_ERR_COUNT == #{dev_cmd_err_cnt}")
end

def confirm_example_data_loop()
    EXAMPLE_DEVICE_LOOP_COUNT.times do |n|
        confirm_example_data()
    end
end

#
# Simulator Functions
#
def example_prepare_ast()
    # Get to known state
    safe_example()

    # Enable
    enable_example()

    # Confirm data
    confirm_example_data_loop()
end

def example_sim_enable()
    cmd("SIM_CMDBUS_BRIDGE EXAMPLE_SIM_ENABLE")
end

def example_sim_disable()
    cmd("SIM_CMDBUS_BRIDGE EXAMPLE_SIM_DISABLE")
end

def example_sim_set_status(status)
    cmd("SIM_CMDBUS_BRIDGE EXAMPLE_SIM_SET_STATUS with STATUS #{status}")
end
