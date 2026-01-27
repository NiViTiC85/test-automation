# TEST PROTOCOL

| ID  | TEST NAME                     | EXPECTED RESULTS                                                     | TIME                  | ACTUAL RESULTS                                                                 |
|-----|-------------------------------|----------------------------------------------------------------------|-----------------------|--------------------------------------------------------------------------------|
| 1.1 | Toggle Button                 | LED1 starts flashing, then stops                                     | Less than 10 seconds  | LED1 starts flashing, then stops                                               |
| 1.2 | Verify Flash Speed            | Interval ≈ 2 × toggleTimerTimeout = 400 ms                           | Approximately 1 minute| Measure flashes over X seconds; divide count by X to get f; compute 1/f; multiply by 2 |
| 1.3 | Verify Debounce Functionality | LED1 flashes; terminal prints "Toggle timer enabled!" only once      | Less than 10 seconds  | Pressing twice within <300 ms changes the LED state only once                  |
| 2.1 | Temperature Button            | Temperature is printed in the terminal                               | Less than 10 seconds  | "Temperature: X Celsius" is shown in the terminal                              |
| 2.2 | Temperature Reading           | Reading ≈ room temperature; then higher after warming the sensor     | Approximately 1 minute| The X in "Temperature: X Celsius" increases                                    |
| 2.3 | Temperature Timer             | Temperature is printed every 60 seconds as specified                 | Less than 5 minutes   | "Temperature: X Celsius" updates every 60 seconds in the terminal              |
| 2.4 | Verify Debounce Functionality | Only one temperature print                                           | Less than 10 seconds  | Pressing twice within <300 ms prints the temperature only once in the terminal |
| 3.1 | Verify Watchdog Activation    | "Running the system!" is printed only once                           | Less than 10 seconds  | "Running the system!" is printed only once in the terminal after reset         |
| 3.2 | Verify Watchdog Functionality | With reset() disabled: message repeats every ~1024 ms; restored: only once | Less than 3 minutes | "Running the system!" is printed repeatedly, approximately once per second     |
| 4.1 | Verify EEPROM Functionality   | After reset when enabled: LED1 flashes + prints enabled; after disable + reset: LED1 off | Less than 2 minutes | The actual toggle state persists between system resets                          |
| 5.1 | End-to-end Scenario           | All steps behave as specified, including persistence across resets   | Approximately 2 minutes| Test runs according to the description                                         |
