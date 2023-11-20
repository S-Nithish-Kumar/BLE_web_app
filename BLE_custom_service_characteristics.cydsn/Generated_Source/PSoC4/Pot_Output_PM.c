/*******************************************************************************
* File Name: Pot_Output.c  
* Version 2.20
*
* Description:
*  This file contains APIs to set up the Pins component for low power modes.
*
* Note:
*
********************************************************************************
* Copyright 2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "cytypes.h"
#include "Pot_Output.h"

static Pot_Output_BACKUP_STRUCT  Pot_Output_backup = {0u, 0u, 0u};


/*******************************************************************************
* Function Name: Pot_Output_Sleep
****************************************************************************//**
*
* \brief Stores the pin configuration and prepares the pin for entering chip 
*  deep-sleep/hibernate modes. This function applies only to SIO and USBIO pins.
*  It should not be called for GPIO or GPIO_OVT pins.
*
* <b>Note</b> This function is available in PSoC 4 only.
*
* \return 
*  None 
*  
* \sideeffect
*  For SIO pins, this function configures the pin input threshold to CMOS and
*  drive level to Vddio. This is needed for SIO pins when in device 
*  deep-sleep/hibernate modes.
*
* \funcusage
*  \snippet Pot_Output_SUT.c usage_Pot_Output_Sleep_Wakeup
*******************************************************************************/
void Pot_Output_Sleep(void)
{
    #if defined(Pot_Output__PC)
        Pot_Output_backup.pcState = Pot_Output_PC;
    #else
        #if (CY_PSOC4_4200L)
            /* Save the regulator state and put the PHY into suspend mode */
            Pot_Output_backup.usbState = Pot_Output_CR1_REG;
            Pot_Output_USB_POWER_REG |= Pot_Output_USBIO_ENTER_SLEEP;
            Pot_Output_CR1_REG &= Pot_Output_USBIO_CR1_OFF;
        #endif
    #endif
    #if defined(CYIPBLOCK_m0s8ioss_VERSION) && defined(Pot_Output__SIO)
        Pot_Output_backup.sioState = Pot_Output_SIO_REG;
        /* SIO requires unregulated output buffer and single ended input buffer */
        Pot_Output_SIO_REG &= (uint32)(~Pot_Output_SIO_LPM_MASK);
    #endif  
}


/*******************************************************************************
* Function Name: Pot_Output_Wakeup
****************************************************************************//**
*
* \brief Restores the pin configuration that was saved during Pin_Sleep(). This 
* function applies only to SIO and USBIO pins. It should not be called for
* GPIO or GPIO_OVT pins.
*
* For USBIO pins, the wakeup is only triggered for falling edge interrupts.
*
* <b>Note</b> This function is available in PSoC 4 only.
*
* \return 
*  None
*  
* \funcusage
*  Refer to Pot_Output_Sleep() for an example usage.
*******************************************************************************/
void Pot_Output_Wakeup(void)
{
    #if defined(Pot_Output__PC)
        Pot_Output_PC = Pot_Output_backup.pcState;
    #else
        #if (CY_PSOC4_4200L)
            /* Restore the regulator state and come out of suspend mode */
            Pot_Output_USB_POWER_REG &= Pot_Output_USBIO_EXIT_SLEEP_PH1;
            Pot_Output_CR1_REG = Pot_Output_backup.usbState;
            Pot_Output_USB_POWER_REG &= Pot_Output_USBIO_EXIT_SLEEP_PH2;
        #endif
    #endif
    #if defined(CYIPBLOCK_m0s8ioss_VERSION) && defined(Pot_Output__SIO)
        Pot_Output_SIO_REG = Pot_Output_backup.sioState;
    #endif
}


/* [] END OF FILE */
