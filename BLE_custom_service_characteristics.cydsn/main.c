/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "project.h"
#include "stdio.h"

CYBLE_CONN_HANDLE_T cyBle_connHandle; // Declare a connection handle

int sensor_data_flag = 0;  // declare a global variable which will be used for notifications and indications

void Stack_Handler(uint32 eventCode, void* eventParam)
{
    // Pointer to a structure holding parameters for a GATT Server write request
    CYBLE_GATTS_WRITE_REQ_PARAM_T *writeReqParam; 
    
    switch(eventCode)
    {
        // starting advertising once the stack is turned ON
        case CYBLE_EVT_STACK_ON:
        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
            CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
            break;
        case CYBLE_EVT_GATTS_WRITE_CMD_REQ:
            // Casting the eventParam to a pointer of type CYBLE_GATTS_WRITE_REQ_PARAM_T
            writeReqParam = (CYBLE_GATTS_WRITE_REQ_PARAM_T *)eventParam;
            
            // if the attribute handle equals LED_CONTROL characterisic, enter this condition
            if(writeReqParam->handleValPair.attrHandle == CYBLE_LED_CONTROL_CHAR_HANDLE)
            {
                if(writeReqParam->handleValPair.value.val[0] == 'a')
                {
                    PWM_WriteCompare(0); // LED is off
                }
                else if(writeReqParam->handleValPair.value.val[0] == 'b')
                {
                    PWM_WriteCompare(500); // LED turns on and off every 500ms
                }
                else if(writeReqParam->handleValPair.value.val[0] == 'c')
                {
                    PWM_WriteCompare(1000); // LED turns on
                }
                // Write the attribute value. Required only when a response to the write request is needed.
                // In our case, we are writing without response
                //CyBle_GattsWriteAttributeValue(&writeReqParam->handleValPair, 0, &cyBle_connHandle, CYBLE_GATT_DB_LOCALLY_INITIATED);
            }
            // if the attribute handle equals LED_VALUEREADFLAG characterisic, enter this condition
            if(writeReqParam->handleValPair.attrHandle == CYBLE_LED_VALUEREADFLAG_CHAR_HANDLE)
            {
                if(writeReqParam->handleValPair.value.val[0] == 'e')
                {
                    sensor_data_flag = 'e';  // update flag to send notifications or indications
                }
                else
                {
                    sensor_data_flag = 0;  // update flag to unsubscribe to the sensor data
                }
                // Write the attribute value. Required only when a response to the write request is needed.
                // In our case, we are writing without any response
                //CyBle_GattsWriteAttributeValue(&writeReqParam->handleValPair, 0, &cyBle_connHandle, CYBLE_GATT_DB_LOCALLY_INITIATED);
            }
            // if the attribute handle equals LED_FORMDATA characterisic, enter this condition
            if(writeReqParam->handleValPair.attrHandle == CYBLE_LED_FORMDATA_CHAR_HANDLE)
            {
                // Prints the data received from the text box in the application.
                char form_data[300]={0};
                for(int i=0; i<writeReqParam->handleValPair.value.len; i++)
                {
                    form_data[i] = writeReqParam->handleValPair.value.val[i];
                }
                sprintf(form_data, "%s", form_data);
                UART_1_PutString(form_data);
                UART_1_PutChar('\n');
                UART_1_PutChar('\r');
            }
            //Send a response to the write request.
            // In our case, we are writing without response
            //CyBle_GattsWriteRsp(cyBle_connHandle);
            break; 
        default:
            break;
    }
}


int main(void)
{
    CyGlobalIntEnable; // Enable global interrupts.
    
    UART_1_Start();
    
    ADC_1_Start();
    
    ADC_1_StartConvert();

    PWM_Start();
    
    CyBle_Start(Stack_Handler);
    
    // declare and initialize variables for sensor data
    uint8 SensorValue;
    uint8 PreviousValue = -100;
    
    
    for(;;)
    {
        CyBle_ProcessEvents();  // process BLE events in each iteration
        
        ADC_1_IsEndConversion(ADC_1_WAIT_FOR_RESULT);
              
        // can be replaced with a sensor. Right now a potentiometer is being used
        if(sensor_data_flag=='e')
        {
            SensorValue = ADC_1_GetResult16(0);
            
            // Uncomment for sending notifications
            /*CYBLE_GATTS_HANDLE_VALUE_NTF_T readResponse;
            readResponse.attrHandle = CYBLE_LED_VALUEREAD_CHAR_HANDLE;
            readResponse.value.len = 1;
            readResponse.value.val = &SensorValue;  // Set the desired value here*/
            
            // Set up the indicationHandle with the appropriate attribute handle and value
            CYBLE_GATTS_HANDLE_VALUE_IND_T indicationHandle;
            indicationHandle.attrHandle = CYBLE_LED_VALUEREAD_CHAR_HANDLE;
            indicationHandle.value.val = &SensorValue; // Your data
            indicationHandle.value.len = 1; // Length of your data
            
            //CyBle_GattsWriteAttributeValue(&readResponse, 0, &cyBle_connHandle, 0);  // Write the attribute value
            CyBle_GattsWriteAttributeValue(&indicationHandle, 0, &cyBle_connHandle, 0);  // Write the attribute value

                                 
            if(1){
                // uncomment for sending notifications
                //CyBle_GattsNotification(cyBle_connHandle, &readResponse); // Send a notification
                CyBle_GattsIndication(cyBle_connHandle, &indicationHandle); // Send an indication
                PreviousValue = SensorValue;
            }
        }
    }   
}

/* [] END OF FILE */
