#include "utgenstub.h"
#include "example_device.h"

int32_t EXAMPLE_ReadData(uart_info_t *device, uint8_t *read_data, uint8_t data_length)
{
    UT_GenStub_SetupReturnBuffer(EXAMPLE_ReadData, int32_t);

    UT_GenStub_AddParam(EXAMPLE_ReadData, uart_info_t *, device);
    UT_GenStub_AddParam(EXAMPLE_ReadData, uint8_t *, read_data);
    UT_GenStub_AddParam(EXAMPLE_ReadData, uint8_t, data_length);

    UT_GenStub_Execute(EXAMPLE_ReadData, Basic, NULL);

    return UT_GenStub_GetReturnValue(EXAMPLE_ReadData, int32_t);
}

int32_t EXAMPLE_CommandDevice(uart_info_t *device, uint8_t cmd, uint32_t payload)
{
    UT_GenStub_SetupReturnBuffer(EXAMPLE_CommandDevice, int32_t);

    UT_GenStub_AddParam(EXAMPLE_CommandDevice, uart_info_t *, device);
    UT_GenStub_AddParam(EXAMPLE_CommandDevice, uint8_t, cmd);
    UT_GenStub_AddParam(EXAMPLE_CommandDevice, uint32_t, payload);

    UT_GenStub_Execute(EXAMPLE_CommandDevice, Basic, NULL);

    return UT_GenStub_GetReturnValue(EXAMPLE_CommandDevice, int32_t);
}

int32_t EXAMPLE_RequestHK(uart_info_t *device, EXAMPLE_Device_HK_tlm_t *data)
{
    UT_GenStub_SetupReturnBuffer(EXAMPLE_RequestHK, int32_t);

    UT_GenStub_AddParam(EXAMPLE_RequestHK, uart_info_t *, device);
    UT_GenStub_AddParam(EXAMPLE_RequestHK, EXAMPLE_Device_HK_tlm_t *, data);

    UT_GenStub_Execute(EXAMPLE_RequestHK, Basic, NULL);

    return UT_GenStub_GetReturnValue(EXAMPLE_RequestHK, int32_t);
}

int32_t EXAMPLE_RequestData(uart_info_t *device, EXAMPLE_Device_Data_tlm_t *data)
{
    UT_GenStub_SetupReturnBuffer(EXAMPLE_RequestData, int32_t);

    UT_GenStub_AddParam(EXAMPLE_RequestData, uart_info_t *, device);
    UT_GenStub_AddParam(EXAMPLE_RequestData, EXAMPLE_Device_Data_tlm_t *, data);

    UT_GenStub_Execute(EXAMPLE_RequestData, Basic, NULL);

    return UT_GenStub_GetReturnValue(EXAMPLE_RequestData, int32_t);
}
