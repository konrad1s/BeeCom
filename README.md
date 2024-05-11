# Overview

BeeCOM is a lightweight, efficient, and easy-to-use C++ library designed for communication in embedded systems. It provides a comprehensive solution for packet-based communication, including packet serialization/deserialization, CRC (Cyclic Redundancy Check) validation, and customizable transport mechanisms. This library is particularly suited for applications requiring reliable data exchange over serial interfaces or custom communication protocols.

![Build Status](https://github.com/konrad1s/BeeCom-Protocol/actions/workflows/stm32-c-cpp.yml/badge.svg)

## Key Features

- **Packet-Based Communication:** Efficiently converts structured data into a serial stream for transmission and vice versa, ensuring data integrity and compatibility across different platforms.
- **CRC Validation:** Offers support for multiple CRC algorithms, enhancing data integrity checks to suit different requirements and scenarios.
- **Serialization and Deserialization:** Automatically convert between packets and byte arrays for transmission and reception.
- **Customizable Transport Layer:** Define your own byte receive and transmit functions to integrate with any physical layer protocol.
- **Observer Pattern for Event Handling** Implements the observer design pattern to allow for easy notification and handling of communication events, making the system highly extensible and adaptable to various use cases.

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

# Getting Started
Before you begin, ensure you have the following:
- A compatible C++ compiler (C++11 or later recommended).
- Basic understanding of C++ and embedded systems communication.
- Any necessary hardware for testing communication, such as development boards or equivalent microcontrollers.

## Installation
1. Clone the Repository:
   ```
   git clone https://github.com/konrad1s/BeeCom-Protocol.git
   ```
2. Add BeeCom-Protocol/Src and BeeCom-Protocol/Inc files to your preffered build system (e.g. Makefile)
3. Check the example below to integrate BeeCom with your application

## STM32 Example Usage

This example demonstrates integrating the BeeCOM library with an STM32 microcontroller for UART communication.

- Ensure you have the HAL library configured for UART communication (huart1 in this example).
- Place the BeeCOM library files in your project directory.
- Include the BeeCOM header (#include "BeeCom.h") in your main file.
- Insert the example code in the appropriate sections of your main function, typically after initializing the system and peripherals.

### Observer Pattern Implementation:
```cpp
class MyPacketObserver : public beecom::IPacketObserver {
public:
    enum class PacketType : uint8_t {
        ping = 0x01U,
        pong = 0x02U,
        invalidPacket = 0xFFU
    };

    void OnPacketReceived(const beecom::Packet &packet, bool crcValid, void *beeComInstance) override {
        beecom::BeeCOM *const beeCom = static_cast<beecom::BeeCOM *>(beeComInstance);

        if (!crcValid) {
            handleInvalidPacket(packet, beeCom);
            return;
        }

        switch (static_cast<PacketType>(packet.header.type)) {
        case PacketType::ping:
            handlePing(packet, beeCom);
            break;
        case PacketType::pong:
            handlePong(packet, beeCom);
            break;
        default:
            handleInvalidPacket(packet, beeCom);
            break;
        }
    }

private:
    void handlePing(const beecom::Packet &packet, beecom::BeeCOM *beeCom) {
        beeCom->Send(static_cast<uint8_t>(PacketType::pong), nullptr, 0U);
    }

    void handlePong(const beecom::Packet &packet, beecom::BeeCOM *beeCom) {
        beeCom->Send(static_cast<uint8_t>(PacketType::ping), nullptr, 0U);
    }

    void handleInvalidPacket(const beecom::Packet &packet, beecom::BeeCOM *beeCom) {
        beeCom->Send(static_cast<uint8_t>(PacketType::invalidPacket), nullptr, 0U);
    }
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
```

Receiving data involves setting up buffer and an observer that will handle incoming packets:
```cpp
MyPacketObserver observer;

constexpr size_t bufferSize = 1024;
uint8_t buffer[bufferSize];
beecom::BeeComBuffer beecomBuffer(buffer, bufferSize);

beecom::BeeCOM beecom(receive, transmit, beecomBuffer);
beecom.SetObserver(&observer);
```
The while (1) loop continuously calls beecom.Receive() to process incoming bytes and handle them according to the defined packet handler.
```cpp
while (1) {
  beecom.Receive();
}
```

To send data using BeeCOM, follow these steps:

```cpp
uint8_t packetType = 0x00U;
uin8_t payload[1] = {0x01U};
beecom.Send(packetType, payload, sizeof(payload));
```

### BeeCOM Python Script Example

[BeeCOM Python script example](https://github.com/konrad1s/BeeCom/tree/master/examples/python_script) demonstrates how to send and receive packets over UART (Universal Asynchronous Receiver/Transmitter) communication in Python. The script is designed to work with microcontrollers and other devices that support UART communication, making it a valuable tool for developers working on embedded systems, IoT devices, and similar projects.

