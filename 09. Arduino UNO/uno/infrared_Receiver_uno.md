# Lesson 14 Infrared-Receiver



## Introduction



An infrared-receiver is a component that receives infrared signals and can independently receive infrared ray and output signals compatible with TTL level. It's similar with a normal plastic-packaged transistor in size and it is suitable for all kinds of infrared remote control and infrared transmission.



## Components



<img src="img/uno18.png" class="align-center" alt="image" />



- `SunFounder R3 Board`

- `Breadboard`

- `Jumper Wires`

- `IR Receiver Module`



## Schematic Diagram



<img src="img/image140.png" class="align-center" alt="image" />



## Experimental Procedures



**Step 1:** Build the circuit.



![image](img/image141.png)



**Step 2:** Open the code file.



**Step 3:** Select the **Board** and **Port.**



**Step 4:** Upload the sketch to the board.



After uploading the codes, you can see that the current value of the pressed button of IR Remote Controller displays on the serial monitor.



<div class="note">



<div class="title">



Note



</div>



- The `IRremote` library is used here, you can install it from the **Library Manager**.



  > <img src="img/lib_irremote.png" class="align-center" alt="image" />



</div>



<div class="note">



<div class="title">



Note



</div>



- There is a transparent plastic piece at the back of the remote control to cut off the power and pull it out before you use the remote control.

- Please gently press the button on the remote to avoid invalid data FFFFFFFF.



</div>



## Code



<iframe src=https://create.arduino.cc/editor/sunfounder01/42ad3e60-0233-4b65-b930-971a4f744527/preview?embed style="height:510px;width:100%;margin:10px 0" frameborder=0></iframe>



## Code Analysis



This code is designed to work with an infrared (IR) remote control using the `IRremote` library. Here's the breakdown:



1.  Include Libraries: This includes the IRremote library, which provides functions to work with IR remote controls.



    > ``` arduino

    > #include <IRremote.h>

    > ```



2.  Defines the Arduino pin to which the IR sensor's signal pin is connected and declares a variable to store the last decoded IR value.



    > ``` arduino

    > const int IR_RECEIVE_PIN = 11;  // Define the pin number for the IR Sensor

    > String lastDecodedValue = "";  // Variable to store the last decoded value

    > ```



3.  Initializes serial communication at a baud rate of 9600. Initializes the IR receiver on the specified pin (`IR_RECEIVE_PIN`) and enables LED feedback (if applicable).



    > ``` arduino

    > void setup() {

    >     Serial.begin(9600);                                     // Start serial communication at 9600 baud rate

    >     IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);  // Start the IR receiver

    > }

    > ```



4.  The loop runs continuously to process incoming IR remote signals.



    > ``` arduino

    > void loop() {

    >     if (IrReceiver.decode()) {

    >         String decodedValue = decodeKeyValue(IrReceiver.decodedIRData.command);

    >         if (decodedValue != "ERROR" && decodedValue != lastDecodedValue) {

    >             Serial.println(decodedValue);

    >             lastDecodedValue = decodedValue;  // Update the last decoded value

    >         }

    >         IrReceiver.resume();  // Enable receiving of the next value

    >     }

    > }

    > ```

    >

    > - Checks if an IR signal is received and successfully decoded.

    > - Decodes the IR command and stores it in `decodedValue` using a custom `decodeKeyValue()` function.

    > - Checks if the decoded value is not an error and is different from the last decoded value.

    > - Prints the decoded IR value to the serial monitor.

    > - Updates the `lastDecodedValue` with the new decoded value.

    > - Resumes IR signal reception for the next signal.
