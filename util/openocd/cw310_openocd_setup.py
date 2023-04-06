import time
import chipwhisperer as cw
target = cw.target(None, cw.targets.CW310)

snake_gpio = target.gpio_mode()
snake_gpio.pin_set_output('USB_A13')#TRST
snake_gpio.pin_set_output('USB_A14')#SRST
snake_gpio.pin_set_output('USB_A15')#PIN_SW_STRAP0
snake_gpio.pin_set_output('USB_A16')#PIN_SW_STRAP1
snake_gpio.pin_set_output('USB_A17')#PIN_SW_STRAP2
snake_gpio.pin_set_output('USB_A18')#TAP_STRAP0
snake_gpio.pin_set_output('USB_A19')#TAP_STRAP1

snake_gpio.pin_set_state('USB_A13', 1) #TRST
snake_gpio.pin_set_state('USB_A14', 1) #SRST

snake_gpio.pin_set_state('USB_A18', 1) #TAP_STRAP0 
snake_gpio.pin_set_state('USB_A19', 1) #TAP_STRAP1

snake_gpio.pin_set_state('USB_A15', 0) #SW_STRAP0
snake_gpio.pin_set_state('USB_A16', 0) #SW_STRAP1
snake_gpio.pin_set_state('USB_A17', 0) #SW_STRAP2

snake_gpio.pin_set_state('USB_A19', 1)
snake_gpio.pin_set_state('USB_A14', 0)
time.sleep(0.1)
snake_gpio.pin_set_state('USB_A14', 1)
snake_gpio.pin_set_state('USB_A19', 1)

#MPSSE time for OpenOCD
target.enable_MPSSE(1)