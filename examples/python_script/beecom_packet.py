import struct
import logging

logger = logging.getLogger(__name__)

def calculate_crc16_ccitt(data, poly=0x1021, crc=0x1D0F):
    """Calculate CRC16 CCITT variant."""
    for byte in data:
        crc ^= byte << 8
        for _ in range(8):
            if crc & 0x8000:
                crc = (crc << 1) ^ poly
            else:
                crc = crc << 1
            crc &= 0xFFFF
    return crc

class BeeCOMPacket:
    def __init__(self, sop=0xA5, packet_type=0x00, payload=b''):
        self.sop = sop
        self.packet_type = packet_type
        self.payload = payload

    def create_packet(self):
        """Create a BeeCOM packet from the instance variables."""
        length = len(self.payload)
        header = struct.pack('BBB', self.sop, self.packet_type, length)
        crc = calculate_crc16_ccitt(header + self.payload)
        packet = header + self.payload + struct.pack('H', crc)
        logger.debug(f"Packet created with SOP: {self.sop}, Type: {self.packet_type}, Length: {length}, CRC: {crc}")
        return packet
