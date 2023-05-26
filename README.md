# controller-robot communication

The two Arduinos communicate with two [ENCJ2860](https://github.com/JAndrassy/EthernetENC)s over an Ethernet cable (needs to be a crossover for a direct run). Messages/packets are sent over UDP.

## message structure

Messages begin with a 1 byte ID that determines the type of message.

| ID   | purpose           |
| ---- | ----------------- |
| 0b00 |                   |
| 0b01 | ping? (todo)      |
| 0b10 | controller status |
| 0b11 | robot status      |

Messages also contain a timestamp from [millis](https://reference.arduino.cc/reference/en/language/functions/time/millis/). As of current, it's not needed as packets always seem to arrive in the proper order.

The rest of the message body is a struct determined by the ID.
