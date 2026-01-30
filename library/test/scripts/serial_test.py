#!/usr/bin/env python3
"""Python script for serial communication with the embedded system.

    This script should:
        1. Read and write data via the serial port.
        2. Format output with timestamps and color coding.
        3. Log all communication to a file.
        4. Send command strings to control program flow.
        5. Verify correct responses from the microcontroller.

    Use the pyserial library, install with the following command:
    
                        pip install pyserial

TODO: Implement functionality according to project requirements.
"""
import colorama
import time
from serial import Serial
from serial.tools import list_ports as serial_ports


def get_com_port() -> str | None:
    """Get COM port of Arduino Uno.
    
    Returns:
        COM port connected to Arduino Uno as a string, or none if not found.
    """
    # Iterate through all COM ports, return port name if Arduino Uno is found.
    for port in serial_ports.comports():
        # Check if port description contains "arduino" or "uno".
        desc = (port.description or "").lower()
        if "arduino" in desc or "uno" in desc:
            return port.device
        # If that didn't match, check if the hardware ID matches Arduino Uno.
        # Note: SB VID:PID for Arduino ID is 2341:0043.
        hw_id = (port.hwid or "").lower()
        if "vid:pid=2341:0043" in hw_id:
            return port.device
    # Return none if no Arduino Uno was found.
    return None


def read_cmd(ser: Serial) -> str:
    return ser.readline().decode("utf-8", errors="ignore").strip().lower()


def get_state(ser: Serial) -> bool:
    """Skicka 's' och returnera 'enabled'/'disabled' (eller None om inget hittas)."""
    # Send status request to Arduino.
    ser.write(b"s")

    # Wait until we receive status.
    while True:
        txt = read_cmd(ser)
        if "enabled" in txt:
            return True
        elif "disabled" in txt:
            return False


def test_toggle(ser: Serial) -> bool:
    # Kolla state (skicka s) kolla om du får "enabled" eller "disabled" i meddelandet.
    state1 = get_state(ser)

    # Skicka t.
    ser.write(b"t")
    time.sleep(0.2)

    # Kolla state igen (skicka s) kolla om du får "enabled" eller "disabled" i meddelandet.
    state2 = get_state(ser)

    # Ensure to reset the output buffer so that only byte is sent per command.
    ser.reset_output_buffer()
    time.sleep(2)

    # Return true om state2 != state1
    return state1 != state2


def test_temperature(ser: Serial, timeout_s : int = 10) -> bool:

    # Send temperature read request.
    ser.write(b"r")

    start_time = time.time()
    end_time = start_time + timeout_s

    # Wait until we receive the temperature of timeout.
    while time.time() < end_time:
        # Kolla vad som skickas till oss, se till att det står "temperature" i strängen.
        txt = read_cmd(ser)
        # Returnera true om du får det.
        if "temperature" in txt.lower():
            return True
    # Return false on timeout.
    return False 


def test_eeprom(ser: Serial) -> bool:
    # Sätt på toggle timern (skicka t om den inte är på).
    if not get_state(ser):
        ser.write(b"t")
        time.sleep(0.2)

    # Reseta MCU:n (kolla kod-snippets för L09).
    # Toggle DTR to trigger hardware reset (DTR is wired to RESET on Arduino Uno).
    ser.dtr = False
    time.sleep(0.1)
    ser.dtr = True
    time.sleep(2.0)

    # Return true if the toggle timer is enabled automatically after startup.
    return get_state(ser)


def run_test(ser: Serial, test, file) -> bool:
    result = test(ser)
    if result:
        # Print the message in green on success.
        msg = f"Test {test.__name__} succeeded!"
        print(colorama.Fore.GREEN + msg)
    else:
        # Print the message in red on failure.
        msg = f"Test {test.__name__} failed!"
        print(colorama.Fore.RED + msg)
    file.write(msg + "\n")
    return result


def run_testsuite(ser: Serial) -> bool:
    result = True
    with open("testresults.txt", "w") as file:
        result &= run_test(ser, test_toggle, file)
        result &= run_test(ser, test_temperature, file)
        result &= run_test(ser, test_eeprom, file)
    if result:
        print(colorama.Fore.GREEN + "Test suite succeeded!")
    else:
        print(colorama.Fore.RED + "Test suite failed!")


def main():
    """Communicate with ATmega328p via UART."""
    com_port = get_com_port()

    if com_port is None:
        raise RuntimeError("Arduino Uno not found on any COM port!")

    # Connect to Arduino Uno (adjust COM port as needed).
    with Serial(com_port, 9600, timeout=1) as ser:
        # Wait for the Arduino to start.
        time.sleep(2)

        # Receive startup responses to clear the buffer before the tests.
        while ser.in_waiting > 0:
            ser.readline().decode("utf-8", errors="replace").strip()
        # Then run the tests.
        run_testsuite(ser)


# Invoke the main function if this is the startup script.
if __name__ == "__main__":
    main()

