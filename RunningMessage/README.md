### Message Notification Panel

This program turns the Funshield into a message notification panel. It receives messages from the computer via a USB serial link and displays them on the 7-segment LED display as running text, scrolling from right to left.

### Getting Started

To begin using the message notification panel with your Funshield, follow these steps:

1. **Download Starter Pack:** Start by downloading the provided starter pack containing all necessary files.

2. **Set Up Arduino Environment:** Ensure that you have the Arduino IDE installed and configured on your computer.

3. **Connect Funshield:** Connect your Funshield to your computer via USB.

4. **Upload Code:** Use the Arduino IDE to upload the provided `solution.ino` file to your Funshield.

### Usage

Here's how to effectively use the message notification panel:

1. **Initialization:** In your Arduino sketch, invoke the `initialize()` method within the `setup()` function to initialize the message notification panel.

2. **Loop Execution:** Make sure to call the `updateInLoop()` method in every iteration of the loop within your Arduino sketch to update the message display.

3. **Displaying Messages:** Messages are received from the computer through a USB serial link. When the display is empty, each message will start scrolling from right to left. The interval between shifts is set to 300 ms. After displaying the entire message, the display will show four empty spaces before the next message begins scrolling.

4. **Message Format:** Keep in mind that only letters (a-z) are supported in the message. Any non-letter characters will be displayed as empty spaces on the Funshield.

#### Example

An example of displaying the message "CAKE IS A LIE" on the Funshield:

```
____
___C
__CA
_CAK
CAKE
AKE_
KE_I
E_IS
_IS_
IS_A
S_A_
_A_L
A_LI
_LIE
LIE_
IE__
E___
```
