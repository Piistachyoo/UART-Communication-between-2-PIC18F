/* 
 * File:   application.c
 * Author: omarm
 *
 * Created on March 6, 2023, 2:10 AM
 */

#include "application.h"
#include "MCAL_Layer/USART/hal_usart.h"

Std_ReturnType ret = E_OK;

void usart_module_init(void){
    usart_t usart_obj;
    
    usart_obj.baudrate = 9600;
    usart_obj.baudrate_cfg = BAUDRATE_ASYN_8BIT_LOW_SPEED;
    
    usart_obj.tx_cfg.usart_tx_enable = EUSART_ASYNCHRONOUS_TX_ENABLE;
    usart_obj.tx_cfg.usart_tx_interrupt_enable = EUSART_ASYNCHRONOUS_INTERRUPT_TX_DISABLE;
    usart_obj.tx_cfg.usart_tx_9bit_enable = EUSART_ASYNCHRONOUS_9BIT_TX_DISABLE;
    
    usart_obj.rx_cfg.usart_rx_enable = EUSART_ASYNCHRONOUS_RX_ENABLE;
    usart_obj.rx_cfg.usart_rx_interrupt_enable = EUSART_ASYNCHRONOUS_INTERRUPT_RX_DISABLE;
    usart_obj.rx_cfg.usart_rx_9bit_enable = EUSART_ASYNCHRONOUS_9BIT_RX_DISABLE;
   
    usart_obj.EUSART_TxDefaultInterruptHandler = NULL;
    usart_obj.EUSART_RxDefaultInterruptHandler = NULL;
    usart_obj.EUSART_OverrunErrorHandler       = NULL;
    usart_obj.EUSART_FramingErrorHandler       = NULL;
    
    EUSART_Init(&usart_obj);
}

led_t led1 = {
    .port_name = GPIO_PORTC_INDEX, .pin = GPIO_PIN0, .led_status = GPIO_LOW
};

int main() {
    application_initialize();
    Std_ReturnType ret;
    uint8 temp;
    while(1){
        ret = EUSART_ReadByteNonBlocking(&temp);
        if(ret == E_OK){
            if(temp == 'A'){
                EUSART_SendStringBlocking((uint8*)"\n\rLed ON\n\r");
                led_turn_on(&led1);
            }
            else if(temp == 'B'){
                led_turn_off(&led1);
                EUSART_SendStringBlocking((uint8*)"\n\rLed OFF\n\r");
            }
            else{ /* Do Nothing */ }
        }
    }
    return (EXIT_SUCCESS);
}

void application_initialize(void)
{
    led_initialize(&led1);
    usart_module_init();
}