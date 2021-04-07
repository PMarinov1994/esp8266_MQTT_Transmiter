#include "BootManager.h"
#include "GlobalDefinitions.h"

/*
*	\brief
*
*/
CBootManager::CBootManager()
{
    pinMode(GPIO_RX, INPUT);
}



/*
*	\brief
*
*/
bool CBootManager::IsConfigBoot()
{
    bool pinState = digitalRead(GPIO_RX) == LOW ? true : false;

    DEBUG_PRINT("BOOT_PIN = ");
    DEBUG_PRINT_LN(digitalRead(GPIO_RX));

    return pinState;
}
