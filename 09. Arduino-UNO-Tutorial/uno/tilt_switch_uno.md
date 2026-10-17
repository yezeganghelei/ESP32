# Lesson 5 Tilt Switch



## Introduction



The tilt switch used here is a ball one with a metal ball inside. It is used to detect inclinations of a small angle.



## Components



<img src="img/uno07.png" class="align-center" alt="image" />



- `Arduino R3 Board`

- `Breadboard`

- `Jumper Wires`

- `Tilt Switch`



## Schematic Diagram



![image](img/image76.png)



## Experimental Procedures



**Step 1:** Build the circuit.



![image](img/image77.png)



**Step 2:** Open the code file.



**Step 3:** Select the **Board** and **Port.**



**Step 4:** Upload the sketch to the board.



Now, tilt the switch, and the LED attached to pin 13 on Uno board will light up.



![image](img/image78.jpeg)



## Code



<iframe src=https://create.arduino.cc/editor/Arduino01/b836688d-452c-4c43-a5d3-d8cc6c163b2a/preview?embed style="height:510px;width:100%;margin:10px 0" frameborder=0></iframe>



## Code Analysis



**Code Analysis 7-1 Whole Code**



``` arduino

const int ledPin = 13; //the led attach to



void setup()



{



  pinMode(ledPin,OUTPUT); //initialize the ledPin as an output



  pinMode(2,INPUT); //set pin2 as INPUT



  digitalWrite(2, HIGH); //set pin2 as HIGH



}



/******************************************/



void loop()



{



  int digitalVal = digitalRead(2); //Read the value of pin2



  if(HIGH == digitalVal) //if tilt switch is not breakover



  {



     digitalWrite(ledPin,LOW); //turn the led off



  }



  else //if tilt switch breakover



  {



     digitalWrite(ledPin,HIGH); //turn the led on



  }



}

```



The whole code are very simple, one pin of the tilt switch is connected to pin2, another pin is connected to GND, when tilt the switch, the two pins of the switch will be connected to GND, then let the LED on the pin13 lights up.
