# Lesson 23 Simple Creation - Digital Dice



## Introduction



In previous experiments, we learned how to use a 7-segment display and control LEDs by a button. In this lesson, we will use a 7-segment display and a button together to create a simple digital dice.



## Components



<img src="img/uno27.png" class="align-center" alt="image" />



- `Arduino R3 Board`

- `Breadboard`

- `Jumper Wires`

- `Resistor`

- `7-segment Display`

- `74HC595`

- `Button`

- `Capacitor`



## Schematic Diagram



The idea behind a digital dice is very simple: a 7-segment display circularly jumps from 1 to 7 rapidly. When the button is pressed, the jumping will slow down until it stops on a number. When the button is pressed again, the process will repeat.



![image](img/image195.png)



## Experimental Procedures



**Step 1:** Build the circuit.



![image](img/image196.png)



**Step 2:** Open the code file.



**Step 3:** Select the Board and Port.



**Step 4:** Upload the sketch to the board.



You can now see the 7-segment display jump between numbers from 1 to 6. Press the button, and the jumping will slow down until it stops three seconds later. Press the button again, and the process will repeat.



<img src="img/image197.jpeg" class="align-center" alt="image" />



## Code



<iframe src=https://create.arduino.cc/editor/Arduino01/03087a2a-e729-42d0-ac76-6754728a6ca0/preview?embed style="height:510px;width:100%;margin:10px 0" frameborder=0></iframe>



## Code Analysis



**The initial random number comes from A0**



``` arduino

randomSeed(analogRead(0));

```



The initial random number is generated from A0 and the range for the random numbers is 0-1023.



**Digital Dice**



``` Arduino

void loop()

{

   int stat = digitalRead(keyIn);  //store value read from keyIn

   if(stat == HIGH)  // check if the pushbutton is pressed

```



If yes, the corresponding pin is high level.



``` Arduino

{

   num ++; // num adds 1

   if(num > 1) 

   {

      num = 0;

   }

}

```



If num \> 1, clear the value. This is to prevent repeated pressing. So just count it as once no matter how many times you press.



``` Arduino

Serial.println(num);  // print the num on serial monitor

if(num == 1)  //when pushbutton is pressed

{

   randNumber = random(1,7); //Generate a random number in 1-7

   showNum(randNumber);  //show the randNumber on 7-segment

   delay(1000);  //wait for 1 second   

   while(!digitalRead(keyIn));  //When not press button,program stop here. 

```



Make it keep displaying the last random number.



``` Arduino

int stat = digitalRead(keyIn); 

```



Read the state of the button again.



``` Arduino

if(stat == HIGH) // check if the pushbutton is pressed

```



If yes, run the code below.



``` Arduino

{

   num ++; // num+1=2

   digitalWrite(ledPin,HIGH); //turn on the led

   delay(100);

   digitalWrite(ledPin,LOW); //turn off the led

   delay(100);

   if(num >= 1) // clear the num

   {

      num = 0;

   }

}

}

//show random numbers at 100 microseconds intervals

//If the button has not been pressed

randNumber = random(1,7);

showNum(randNumber);

delay(100);

}

```



**showNum() function**



``` arduino

void showNum(int num)



{



    digitalWrite(latchPin,LOW); //ground latchPin and hold low for transmitting



    shiftOut(dataPin,clockPin,MSBFIRST,datArray[num]);



    //return the latch pin high to signal chip that it



    //no longer needs to listen for information



    digitalWrite(latchPin,HIGH); //pull the latchPin to save the data



}

```



This function is to display the number in `dataArray[]` on the 7-segment display.
