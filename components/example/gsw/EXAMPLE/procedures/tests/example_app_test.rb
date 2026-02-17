require 'cosmos'
require 'cosmos/script'
require "example_lib.rb"

##
## This script tests the standard cFS component application functionality.
## Currently this includes: 
##   Housekeeping, request telemetry to be published on the software bus
##   NOOP, no operation but confirm correct counters increment
##   Reset counters, increment as done in NOOP and confirm ability to clear repeatably
##   Invalid ground command, confirm bad lengths and codes are rejected
##

# Get to known state
safe_example()

##
##   Housekeeping, request telemetry to be published on the software bus
##
EXAMPLE_TEST_LOOP_COUNT.times do |n|
    get_example_hk()
end


##
## NOOP, no operation but confirm correct counters increment
##
EXAMPLE_TEST_LOOP_COUNT.times do |n|
    example_cmd("EXAMPLE EXAMPLE_NOOP_CC")
end


##
## Reset counters, increment as done in NOOP and confirm ability to clear repeatably
##
EXAMPLE_TEST_LOOP_COUNT.times do |n|
    example_cmd("EXAMPLE EXAMPLE_NOOP_CC")
    cmd("EXAMPLE EXAMPLE_RST_COUNTERS_CC") # Note standard `cmd` as we can't reset counters and then confirm increment
    get_example_hk()
    check("EXAMPLE EXAMPLE_HK_TLM CMD_COUNT == 0")
    check("EXAMPLE EXAMPLE_HK_TLM CMD_ERR_COUNT == 0")
end


##
##   Invalid ground command, confirm bad lengths and codes are rejected
##
EXAMPLE_TEST_LOOP_COUNT.times do |n|
    # Bad length
    cmd_cnt = tlm("EXAMPLE EXAMPLE_HK_TLM CMD_COUNT")
    cmd_err_cnt = tlm("EXAMPLE EXAMPLE_HK_TLM CMD_ERR_COUNT")
    cmd("EXAMPLE EXAMPLE_NOOP_CC with CCSDS_LENGTH #{n+2}") # Note +2 due to CCSDS already being +1
    get_example_hk()
    check("EXAMPLE EXAMPLE_HK_TLM CMD_COUNT == #{cmd_cnt}")
    check("EXAMPLE EXAMPLE_HK_TLM CMD_ERR_COUNT == #{cmd_err_cnt+1}")
end

for n in 6..(5 + EXAMPLE_TEST_LOOP_COUNT)
    # Bad command codes
    cmd_cnt = tlm("EXAMPLE EXAMPLE_HK_TLM CMD_COUNT")
    cmd_err_cnt = tlm("EXAMPLE EXAMPLE_HK_TLM CMD_ERR_COUNT")
    cmd("EXAMPLE EXAMPLE_NOOP_CC with CCSDS_FC #{n+1}")
    get_example_hk()
    check("EXAMPLE EXAMPLE_HK_TLM CMD_COUNT == #{cmd_cnt}")
    check("EXAMPLE EXAMPLE_HK_TLM CMD_ERR_COUNT == #{cmd_err_cnt+1}")
end
