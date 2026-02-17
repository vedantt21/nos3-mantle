require 'cosmos'
require 'cosmos/script'
require "example_lib.rb"

##
## This script tests the cFS component in an automated scenario.
## Currently this includes: 
##   Hardware failure
##   Hardware status reporting fault
##


##
## Hardware failure
##
EXAMPLE_TEST_LOOP_COUNT.times do |n|
    # Prepare
    example_prepare_ast()

    # Disable sim and confirm device error counts increase
    dev_cmd_cnt = tlm("EXAMPLE EXAMPLE_HK_TLM DEVICE_COUNT")
    dev_cmd_err_cnt = tlm("EXAMPLE EXAMPLE_HK_TLM DEVICE_ERR_COUNT")
    example_sim_disable()
    check("EXAMPLE EXAMPLE_HK_TLM DEVICE_COUNT == #{dev_cmd_cnt}")
    check("EXAMPLE EXAMPLE_HK_TLM DEVICE_ERR_COUNT >= #{dev_cmd_err_cnt}")

    # Enable sim and confirm return to nominal operation
    example_sim_enable()
    confirm_example_data_loop()
end


##
## Hardware status reporting fault
##
EXAMPLE_TEST_LOOP_COUNT.times do |n|
    # Prepare
    example_prepare_ast()

    # Add a fault to status in the simulator
    example_sim_set_status(255)

    # Confirm that status register and that app disabled itself
    get_example_hk()
    check("EXAMPLE EXAMPLE_HK_TLM DEVICE_STATUS == 255")
    get_example_hk()
    check("EXAMPLE EXAMPLE_HK_TLM DEVICE_ENABLED == 'DISABLED'")
    
    # Clear simulator status fault
    example_sim_set_status(0)
end
