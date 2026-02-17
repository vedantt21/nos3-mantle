// ======================================================================
// \title  ExampleSim.hpp
// \author jstar
// \brief  hpp file for ExampleSim component implementation class
// ======================================================================

#ifndef Components_ExampleSim_HPP
#define Components_ExampleSim_HPP

#include "example_src/ExampleSimComponentAc.hpp"
#include "example_src/ExampleSim_ActiveStateEnumAc.hpp"

extern "C"{
#include "example_device.h"
#include "libuart.h"
}
  

#define EXAMPLE_DEVICE_DISABLED 0
#define EXAMPLE_DEVICE_ENABLED  1

typedef struct
{
    uint8_t                     CommandErrorCount;
    uint8_t                     CommandCount;
    uint8_t                     DeviceErrorCount;
    uint8_t                     DeviceCount;
    uint8_t                     DeviceEnabled;
} __attribute__((packed)) EXAMPLE_Hk_tlm_t;
#define EXAMPLE_HK_TLM_LNGTH sizeof(EXAMPLE_Hk_tlm_t)


namespace Components {

  class ExampleSim :
    public ExampleSimComponentBase
  {

    public:

    uart_info_t ExampleUart; 
    EXAMPLE_Device_HK_tlm_t ExampleHK; 
    EXAMPLE_Device_Data_tlm_t ExampleData;
    int32_t status = OS_SUCCESS;

    EXAMPLE_Hk_tlm_t HkTelemetryPkt;

      // ----------------------------------------------------------------------
      // Component construction and destruction
      // ----------------------------------------------------------------------

      //! Construct ExampleSim object
      ExampleSim(
          const char* const compName //!< The component name
      );

      //! Destroy ExampleSim object
      ~ExampleSim();

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for commands
      // ----------------------------------------------------------------------


      void REQUEST_HOUSEKEEPING_cmdHandler(
        FwOpcodeType opCode, 
        U32 cmdSeq
      ) override;

      void NOOP_cmdHandler(
        FwOpcodeType opCode, 
        U32 cmdSeq
      )override;

       void EXAMPLE_SEQ_cmdHandler(
        FwOpcodeType opCode, 
        U32 cmdSeq
      )override;

      void ENABLE_cmdHandler(
        FwOpcodeType opCode,
        U32 cmdSeq
      )override;

      void DISABLE_cmdHandler(
        FwOpcodeType opCode,
        U32 cmdSeq
      )override;

      void RESET_COUNTERS_cmdHandler(
        FwOpcodeType opCode,
        U32 cmdSeq
      )override;

      void CONFIGURE_cmdHandler(
        FwOpcodeType opCode,
        U32 cmdSeq,
        const U32 config
      )override;

      inline ExampleSim_ActiveState get_active_state(uint8_t DeviceEnabled);

  };

}

#endif
