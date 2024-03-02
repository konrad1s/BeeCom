from tqdm import tqdm
from beecom_packet import BeeCOMPacket
from uart_com import UARTCommunication
import logging

def read_hex_file(file_path):
    """Read a .hex file and return its contents as bytes."""
    with open(file_path, 'rb') as f:
        return f.read()

def send_hex_file_contents(uart_comm, file_path, max_payload_size=200):
    """Read a .hex file and send its contents in chunks as BeeCOM packets with progress bar."""
    file_contents = read_hex_file(file_path)
    
    total_chunks = len(file_contents) // max_payload_size + (1 if len(file_contents) % max_payload_size else 0)
    
    logging.info("Sending file contents...")
    with tqdm(total=total_chunks, unit="packet") as pbar:
        for i in range(0, len(file_contents), max_payload_size):
            payload_chunk = file_contents[i:i+max_payload_size]
            packet = BeeCOMPacket(packet_type=0x01, payload=payload_chunk).create_packet()
            response = uart_comm.send_packet(packet)
            UARTCommunication.interpret_response(response)
            pbar.update(1)

if __name__ == '__main__':
    logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(name)s - %(levelname)s - %(message)s')
    
    uart_comm = UARTCommunication('COM8', 115200)
    file_path = '../STM32F407VE/build/beecom.hex'

    send_hex_file_contents(uart_comm, file_path)
