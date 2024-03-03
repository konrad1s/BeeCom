# BeeCom

BeeCOM is a lightweight, efficient, and easy-to-use C++ library designed for communication in embedded systems. It provides a comprehensive solution for packet-based communication, including packet serialization/deserialization, CRC (Cyclic Redundancy Check) validation, and customizable transport mechanisms. This library is particularly suited for applications requiring reliable data exchange over serial interfaces or custom communication protocols.

![Build Status](https://github.com/konrad1s/BeeCom-Protocol/actions/workflows/stm32-c-cpp.yml/badge.svg)

## Key Features

- **Packet-Based Communication:** Structure your data into packets for organized and reliable transmission.
- **CRC Validation:** Ensure data integrity with CRC checks, utilizing a customizable CRC calculation function.
- **Serialization and Deserialization:** Automatically convert between packets and byte arrays for transmission and reception.
- **Customizable Transport Layer:** Define your own byte receive and transmit functions to integrate with any physical layer protocol.

## BeeCom Packet
The BeeCom packet is a structured data format designed for efficient and reliable communication between devices, particularly in embedded systems. A BeeCom packet encapsulates the data to be transmitted along with metadata that helps in the processing, routing, and validation of the packet. The structure of a BeeCom packet typically includes several key components:

### Packet Structure:

1. Start of Packet (SOP):
        A predefined byte sequence that indicates the beginning of a packet.
        Helps in synchronization and identifying the start of a new packet in a stream of bytes.

2. Packet Type:
        Identifies the type or category of the data contained within the packet.
        Enables the receiving system to process the packet accordingly.

3. Payload Length:
        Specifies the size of the payload in bytes.
        Allows the receiver to determine how many bytes to read as part of the packet's payload.

4. Payload:
        The actual data being transmitted.
        Its structure and content vary depending on the packet type and the application's requirements.

5. CRC (Cyclic Redundancy Check):
        A checksum calculated over the packet's contents (excluding the SOP but typically including the packet type, payload length, and payload).
        Used by the receiver to detect errors in the packet.

### Example Packet:
![example_packet](https://github.com/konrad1s/BeeCom/blob/master/examples/uart_packet/beecom_packet.png)

## BeeCOM with STM32 and UART Example Usage

This example demonstrates integrating the BeeCOM library with an STM32 microcontroller for UART communication. It includes setting up packet handlers, byte receive and transmit functions, and the main execution loop.

- Ensure you have the HAL library configured for UART communication (huart1 in this example).
- Place the BeeCOM library files in your project directory.
- Include the BeeCOM header (#include "BeeCOM.h") in your main file.
- Insert the example code in the appropriate sections of your main function, typically after initializing the system and peripherals.

A lambda function is used as the packet handler. It constructs and sends an acknowledgment packet (ackPacket) depending on the CRC check result.
```cpp
beecom::PacketHandler packetHandler = [](const beecom::Packet &packet, bool crcValid, beecom::SendFunction send) {
  beecom::Packet ackPacket;
  ackPacket.header.sop = 0x55;
  ackPacket.header.type = 0x01;
  ackPacket.header.length = 1U;
  ackPacket.payload[0] = crcValid ? 0x5A : 0xA5;

  send(ackPacket);
};
```
Byte Receive and Transmit Functions:
- receive: A lambda function that wraps the HAL_UART_Receive call to receive a byte from UART
- transmit: A lambda function for transmitting a buffer of bytes over UART using HAL_UART_Transmit.
```cpp
auto receive = [](uint8_t *uartRxByte) -> bool {
  return (HAL_UART_Receive(&huart1, uartRxByte, 1, 0) == HAL_OK);
};

auto transmit = [](const uint8_t *buffer, size_t size) {
  HAL_UART_Transmit(&huart1, const_cast<uint8_t *>(buffer), size, 100);
};

beecom::BeeCOM beecom(packetHandler, receive, transmit);
```
The while (1) loop continuously calls beecom.receive() to process incoming bytes and handle them according to the defined packet handler.
```cpp
while (1) {
  beecom.receive();
}
```

### BeeCOM Python Script Example

[BeeCOM Python script example](https://github.com/konrad1s/BeeCom/tree/master/examples/python_script) demonstrates how to send and receive packets over UART (Universal Asynchronous Receiver/Transmitter) communication in Python. The script is designed to work with microcontrollers and other devices that support UART communication, making it a valuable tool for developers working on embedded systems, IoT devices, and similar projects.

