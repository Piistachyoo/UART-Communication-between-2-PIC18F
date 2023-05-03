
#include "hal_usart.h"

static void EUSART_Baud_Rate_Calculation(const usart_t *_eusart);
static void EUSART_ASYNC_TX_Init(const usart_t *_eusart);
static void EUSART_ASYNC_RX_Init(const usart_t *_eusart);

Std_ReturnType EUSART_Init(const usart_t *_eusart){
    Std_ReturnType ret = E_NOT_OK;
    if(_eusart != NULL){
        RCSTAbits.SPEN = EUSART_MODULE_DISABLE;
        TRISCbits.RC6 = 1;
        TRISCbits.RC7 = 1;
        EUSART_Baud_Rate_Calculation(_eusart);
        EUSART_ASYNC_TX_Init(_eusart);
        EUSART_ASYNC_RX_Init(_eusart);
        RCSTAbits.SPEN = EUSART_MODULE_ENABLE;
        ret = E_OK;
    }
    return ret;
}

Std_ReturnType EUSART_DeInit(const usart_t *_eusart){
    Std_ReturnType ret = E_NOT_OK;
    if(_eusart != NULL){
        RCSTAbits.SPEN = EUSART_MODULE_DISABLE;
        ret = E_OK;
    }
    return ret;
}

Std_ReturnType EUSART_ReadByteBlocking(uint8 *_data){
    Std_ReturnType ret = E_NOT_OK;
    if(_data != NULL){
        while(!PIR1bits.RCIF);
        *_data = RCREG;
        ret = E_OK;
    }
    return ret;
}

Std_ReturnType EUSART_ReadByteNonBlocking(uint8 *_data){
    Std_ReturnType ret = E_NOT_OK;
    if(_data != NULL){
        if(1 == PIR1bits.RCIF){
            *_data = RCREG;
            ret = E_OK;
        }
        else{
            ret = E_NOT_OK;
        }
    }
    return ret;
}

Std_ReturnType EUSART_WriteByteBlocking(uint8 _data){
    Std_ReturnType ret = E_OK;
    while(!TXSTAbits.TRMT);
    TXREG = _data;
    return ret;
}

Std_ReturnType EUSART_SendStringBlocking(uint8 *_string){
    Std_ReturnType ret = E_NOT_OK;
    int i=0;
    if(_string != NULL){
        while(_string[i] != '\0'){
            EUSART_WriteByteBlocking(_string[i]);
            i++;
        }
        ret = E_OK;
    }
    return ret;
}

static void EUSART_Baud_Rate_Calculation(const usart_t *_eusart){
    float Baud_Rate_temp;
    switch(_eusart->baudrate_cfg){
        case BAUDRATE_ASYN_8BIT_LOW_SPEED:
            TXSTAbits.SYNC = EUSART_ASYNCHRONOUS_MODE;
            TXSTAbits.BRGH = EUSART_ASYNCHRONOUS_LOW_SPEED;
            BAUDCONbits.BRG16 = EUSART_8BIT_BAUDRATE_GEN;
            Baud_Rate_temp = ((_XTAL_FREQ/(float)_eusart->baudrate)/64) - 1;
            SPBRG = (uint8)((uint32)Baud_Rate_temp);
            break;
        case BAUDRATE_ASYN_8BIT_HIGH_SPEED:
            TXSTAbits.SYNC = EUSART_ASYNCHRONOUS_MODE;
            TXSTAbits.BRGH = EUSART_ASYNCHRONOUS_HIGH_SPEED;
            BAUDCONbits.BRG16 = EUSART_8BIT_BAUDRATE_GEN;
            Baud_Rate_temp = ((_XTAL_FREQ/(float)_eusart->baudrate)/16) - 1;
            SPBRG = (uint8)((uint32)Baud_Rate_temp);
            break;
        case BAUDRATE_ASYN_16BIT_LOW_SPEED:
            TXSTAbits.SYNC = EUSART_ASYNCHRONOUS_MODE;
            TXSTAbits.BRGH = EUSART_ASYNCHRONOUS_LOW_SPEED;
            BAUDCONbits.BRG16 = EUSART_16BIT_BAUDRATE_GEN;
            Baud_Rate_temp = ((_XTAL_FREQ/(float)_eusart->baudrate)/16) - 1;
            SPBRG = (uint8)((uint32)Baud_Rate_temp);
            SPBRGH = (uint8)((uint32)Baud_Rate_temp>>8);
            break;
        case BAUDRATE_ASYN_16BIT_HIGH_SPEED:
            TXSTAbits.SYNC = EUSART_ASYNCHRONOUS_MODE;
            TXSTAbits.BRGH = EUSART_ASYNCHRONOUS_HIGH_SPEED;
            BAUDCONbits.BRG16 = EUSART_16BIT_BAUDRATE_GEN;
            Baud_Rate_temp = ((_XTAL_FREQ/(float)_eusart->baudrate)/4) - 1;
            SPBRG = (uint8)((uint32)Baud_Rate_temp);
            break;
        case BAUDRATE_SYN_8BIT:
            TXSTAbits.SYNC = EUSART_SYNCHRONOUS_MODE;
            BAUDCONbits.BRG16 = EUSART_8BIT_BAUDRATE_GEN;
            break;
        case BAUDRATE_SYN_16BIT:
            TXSTAbits.SYNC = EUSART_SYNCHRONOUS_MODE;
            BAUDCONbits.BRG16 = EUSART_16BIT_BAUDRATE_GEN;
            break;
        default: /* Do Nothing */ break;
    }
}

static void EUSART_ASYNC_TX_Init(const usart_t *_eusart){
    if(_eusart->tx_cfg.usart_tx_enable == EUSART_ASYNCHRONOUS_TX_ENABLE){
        TXSTAbits.TXEN = EUSART_ASYNCHRONOUS_TX_ENABLE;
        if(_eusart->tx_cfg.usart_tx_interrupt_enable == EUSART_ASYNCHRONOUS_INTERRUPT_TX_ENABLE){
            PIE1bits.TXIE = EUSART_ASYNCHRONOUS_INTERRUPT_TX_ENABLE;
        }
        else if(_eusart->tx_cfg.usart_tx_interrupt_enable == EUSART_ASYNCHRONOUS_INTERRUPT_TX_DISABLE){
            PIE1bits.TXIE = EUSART_ASYNCHRONOUS_INTERRUPT_TX_DISABLE;
        }
        else{ /* Do Nothing */ }
        
        if(_eusart->tx_cfg.usart_tx_9bit_enable == EUSART_ASYNCHRONOUS_9BIT_TX_ENABLE){
            TXSTAbits.TX9 = EUSART_ASYNCHRONOUS_9BIT_TX_ENABLE;
        }
        else if(_eusart->tx_cfg.usart_tx_9bit_enable == EUSART_ASYNCHRONOUS_9BIT_TX_DISABLE){
            TXSTAbits.TX9 = EUSART_ASYNCHRONOUS_9BIT_TX_DISABLE;
        }
        else{ /* Do Nothing */ }
    }
    else if(_eusart->tx_cfg.usart_tx_enable == EUSART_ASYNCHRONOUS_TX_DISABLE){
        TXSTAbits.TXEN = EUSART_ASYNCHRONOUS_TX_DISABLE;
    }
    else{ /* Do Nothing */ }
}

static void EUSART_ASYNC_RX_Init(const usart_t *_eusart){
    if(_eusart->rx_cfg.usart_rx_enable == EUSART_ASYNCHRONOUS_RX_ENABLE){
        RCSTAbits.CREN = EUSART_ASYNCHRONOUS_RX_ENABLE;
        if(_eusart->rx_cfg.usart_rx_interrupt_enable == EUSART_ASYNCHRONOUS_INTERRUPT_RX_ENABLE){
            PIE1bits.RCIE = EUSART_ASYNCHRONOUS_INTERRUPT_RX_ENABLE;
        }
        else if(_eusart->rx_cfg.usart_rx_interrupt_enable == EUSART_ASYNCHRONOUS_INTERRUPT_RX_DISABLE){
            PIE1bits.RCIE = EUSART_ASYNCHRONOUS_INTERRUPT_RX_DISABLE;
        }
        else{ /* Do Nothing */ }

        if(_eusart->rx_cfg.usart_rx_9bit_enable == EUSART_ASYNCHRONOUS_9BIT_RX_ENABLE){
            RCSTAbits.RX9 = EUSART_ASYNCHRONOUS_9BIT_RX_ENABLE;
        }
        else if(_eusart->rx_cfg.usart_rx_9bit_enable == EUSART_ASYNCHRONOUS_9BIT_RX_DISABLE){
            RCSTAbits.RX9 = EUSART_ASYNCHRONOUS_9BIT_RX_DISABLE;
        }
        else{ /* Do Nothing */ }
    }
    else if(_eusart->rx_cfg.usart_rx_enable == EUSART_ASYNCHRONOUS_RX_DISABLE){
        RCSTAbits.CREN = EUSART_ASYNCHRONOUS_RX_DISABLE;
    }
    else{ /* Do Nothing */ }
    
    
}
