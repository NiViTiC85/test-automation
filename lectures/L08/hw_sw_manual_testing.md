# HW/SW integration specification tests

## Prerequisites
* Build and flash an ATmega328p processor.
* Run the system, open a serial terminal.

## 1. Toggle Functionality

### 1.1 Toggle Button
* Press the Toggle button (sw1).
* The LED (LED1) shall start to flash.
* Press the Toggle button (sw1) again.
* The LED (LED1) shall stop flashing.

### 1.2 Verify Flash Speed
* Press the Toggle button (sw1).
* The LED (LED1) shall start to flash.
* Measure LED (LED1) flash speed by counting number of flashes over 10 secounds.
* Get the frequensy (Hz) by deviding the number of flashes by ten (number of secounds).
* Get the flash time intervall by devidding 1 with the fequency.
* The intervall shall match two times the "toggleTimerTimeout" = 200 ms.

### 1.3 Verify Debounce Functionality
* Quickly press the toggle button (sw1) at least twice within 300 ms.
* The LED (LED1) shall start to flash.
* The terminal shall only print "Toggle timer enabled!" and not both enabled and disabled.

## 2. Temperature Measurement

### 2.1 Temperature Button
* Press the temperature button (sw2).
* The temperature shall be printed in the terminal.

### 2.2 Temperature Reading
* Compare the temperature reading printed in the terminal with the room temperature.
* Expect equal temperature.
* Try to heat the sensor (with your hand).
* Press the temperature button (sw2).
* The temperature shall be printed in the terminal.
* Expect a raise in temperature.

### 2.3 Temperature Timer
* Ensure that the temperature is printed in the terminal every 60 seconds (after latest print), 
* or 60 seconds after the latest pressdown on the temperature button (sw2).

### 2.4 Verify Debounce Functionality
* Quickly press the temperature button (sw2) at least twice within 300 ms.
* The temperature shall only be printed in the terminal once.

## 3. Watchdog timer

### 3.1 Verify Watchdog Activation
* Reset program with reset button.
* Expect that the terminal says "Running the system!" only once.

### 3.2 Verify Watchdog Functionality
* Disable the Watchdog function by commenting out the line "myWatchdog.reset();" in the file logic.cpp in the function "void Logic::run(const bool& stop) noexcept".
* Expect the terminal atomaticly to say "Running the system!" every 1024 ms ~ once every second.
* Restore the Watchdog, myWatchdog.reset();, functionality again.
* Ensure that the terminal says "Running the system!" only once.

## 4. EEPROM-persistens

### 4.1 Verify EEPROM Functionality
* Activate the toggle timer by pressing the Toggle button (sw1).
* The LED (LED1) shall start to flash.
* Power cycle the unit or reset program with reset button.
* The LED (LED1) shall start to flash.
* Expect the terminal atomaticly to say "Toggle timer enabled!".
* Deactivate the toggle timer by pressing the Toggle button (sw1).
* The LED (LED1) shall turn off.
* Power cycle the unit or reset program with reset button.
* The LED (LED1) shall be off.

## 5. End-to-end scenario
**OBS!** Interrupt-hantering och debounce-funktionalitet testas redan i sektionerna 2-3 ovan.

### 5.1 End-to-end scenario

Kör ett komplett scenario:
1. Starta systemet från reset.
2. Tryck på toggle-knappen (sw1) → lysdioden (led1) ska blinka.
3. Tryck på temperaturknappen (sw2) → temperatur ska skrivas ut i terminalen.
4. Vänta på automatisk temperaturavläsning (60 sekunder).
5. Tryck på toggle-knappen (sw1) igen → lysdioden (led1) ska sluta blinka.
6. Tryck på toggle-knappen (sw1) igen → lysdioden (led1) ska börja blinka.
7. Resetta systemet och verifiera att toggle-tillståndet är aktivt vid start, dvs. lysdioden (led1) ska börja blinka efter omstart.
8. Tryck på toggle-knappen (sw1) igen → lysdioden (led1) ska sluta blinka.
9. Resetta systemet och verifiera att toggle-tillståndet är inaktivt vid start, dvs. lysdioden (led1) ska vara släckt efter omstart.

--------------------------------------------------------------------------------
