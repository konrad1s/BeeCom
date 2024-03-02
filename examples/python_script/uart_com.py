import serial
import time
import logging

logger = logging.getLogger(__name__)

class UARTCommunication:
    def __init__(self, port, baudrate):
        self.port = port
        self.baudrate = baudrate

    def send_packet(self, packet):
        """Send a packet over UART and wait for ACK/NACK."""
        with serial.Serial(self.port, self.baudrate, timeout=1) as ser:
            ser.write(packet)

            timeout = time.time() + 1
            while time.time() < timeout:
                if ser.in_waiting > 0:
                    response = ser.read(ser.in_waiting)
                    return response
        return None

    @staticmethod
    def interpret_response(response):
        """Interpret the received ACK/NACK response."""
        if response:
            ack_code = response[-3]
            if ack_code == 0xAA:
                logger.info("ACK received.")
            elif ack_code == 0x55:
                logger.info("NACK received.")
            else:
                logger.warning(f"Unknown response: {response.hex()}")
        else:
            logger.error("Failed to receive a valid response.")
