require 'cosmos'
require 'cosmos/script'
require "example_lib.rb"

##
## This script tests the cFS component device functionality.
## Currently this includes: 
##   Enable / disable, control hardware communications
##   Configuration, reconfigure example instrument register
##


##
## Enable / disable, control hardware communications
##
EXAMPLE_TEST_LOOP_COUNT.times do |n|
    # Get to known state
    safe_example()

    # Manually command to disable when already disabled
    cmd_cnt = tlm("EXAMPLE EXAMPLE_HK_TLM CMD_COUNT")
    cmd_err_cnt = tlm("EXAMPLE EXAMPLE_HK_TLM CMD_ERR_COUNT")
    cmd("EXAMPLE EXAMPLE_DISABLE_CC")
    get_example_hk()
    check("EXAMPLE EXAMPLE_HK_TLM CMD_COUNT == #{cmd_cnt}")
    check("EXAMPLE EXAMPLE_HK_TLM CMD_ERR_COUNT == #{cmd_err_cnt+1}")

    # Enable
    enable_example()

    # Confirm device counters increment without errors
    confirm_example_data_loop()

    # Manually command to enable when already enabled
    cmd_cnt = tlm("EXAMPLE EXAMPLE_HK_TLM CMD_COUNT")
    cmd_err_cnt = tlm("EXAMPLE EXAMPLE_HK_TLM CMD_ERR_COUNT")
    cmd("EXAMPLE EXAMPLE_ENABLE_CC")
    get_example_hk()
    check("EXAMPLE EXAMPLE_HK_TLM CMD_COUNT == #{cmd_cnt}")
    check("EXAMPLE EXAMPLE_HK_TLM CMD_ERR_COUNT == #{cmd_err_cnt+1}")

    # Reconfirm data remains as expected
    confirm_example_data_loop()

    # Disable
    disable_example()
end


##
##   Configuration, reconfigure example instrument register
##
EXAMPLE_TEST_LOOP_COUNT.times do |n|
    # Get to known state
    safe_example()

    # Confirm configuration command denied if disabled
    cmd_cnt = tlm("EXAMPLE EXAMPLE_HK_TLM CMD_COUNT")
    cmd_err_cnt = tlm("EXAMPLE EXAMPLE_HK_TLM CMD_ERR_COUNT")
    cmd("EXAMPLE EXAMPLE_CONFIG_CC with DEVICE_CONFIG 10")
    get_example_hk()
    check("EXAMPLE EXAMPLE_HK_TLM CMD_COUNT == #{cmd_cnt}")
    check("EXAMPLE EXAMPLE_HK_TLM CMD_ERR_COUNT == #{cmd_err_cnt+1}")
    
    # Enable
    enable_example()

    # Set configuration
    example_cmd("EXAMPLE EXAMPLE_CONFIG_CC with DEVICE_CONFIG #{n+1}")
    check("EXAMPLE EXAMPLE_HK_TLM DEVICE_CONFIG == #{n+1}")
end
