# Resistor



<img src="img/resistor.png" width="300" alt="image" />



Resistor is an electronic element that can limit the branch current. A fixed resistor is a kind of resistor whose resistance cannot be changed, while that of a potentiometer or a variable resistor can be adjusted.



Two generally used circuit symbols for resistor. Normally, the resistance is marked on it. So if you see these symbols in a circuit, it stands for a resistor.



<img src="img/resistor_symbol.png" width="400" alt="image" />



**惟** is the unit of resistance and the larger units include K惟, M惟, etc. Their relationship can be shown as follows: 1 M惟=1000 K惟, 1 K惟 = 1000 惟. Normally, the value of resistance is marked on it.



When using a resistor, we need to know its resistance first. Here are two methods: you can observe the bands on the resistor, or use a multimeter to measure the resistance. You are recommended to use the first method as it is more convenient and faster.



![image](img/resistance_card.jpg)



As shown in the card, each color stands for a number.



|       |       |     |        |        |       |      |        |      |       |      |        |

|-------|-------|-----|--------|--------|-------|------|--------|------|-------|------|--------|

| Black | Brown | Red | Orange | Yellow | Green | Blue | Violet | Grey | White | Gold | Silver |

| 0     | 1     | 2   | 3      | 4      | 5     | 6    | 7      | 8    | 9     | 0.1  | 0.01   |



The 4- and 5-band resistors are frequently used, on which there are 4 and 5 chromatic bands.



Normally, when you get a resistor, you may find it hard to decide which end to start for reading the color. The tip is that the gap between the 4th and 5th band will be comparatively larger.



Therefore, you can observe the gap between the two chromatic bands at one end of the resistor; if it's larger than any other band gaps, then you can read from the opposite side.



Let's see how to read the resistance value of a 5-band resistor as shown below.



<img src="img/220ohm.jpg" width="500" alt="image" />



So for this resistor, the resistance should be read from left to right. The value should be in this format: 1st Band 2nd Band 3rd Band x 10^Multiplier (惟) and the permissible error is 卤Tolerance%. So the resistance value of this resistor is 2(red) 2(red) 0(black) x 10^0(black) 惟 = 220 惟, and the permissible error is 卤 1% (brown).



**Common resistor color band**



| Resistor | Color Band                      |

|----------|---------------------------------|

| 10惟      | brown black black silver brown  |

| 100惟     | brown black black black brown   |

| 220惟     | red red black black brown       |

| 330惟     | orange orange black black brown |

| 1k惟      | brown black black brown brown   |

| 2k惟      | red black black brown brown     |

| 5.1k惟    | green brown black brown brown   |

| 10k惟     | brown black black red brown     |

| 100k惟    | brown black black orange brown  |

| 1M惟      | brown black black green brown   |



You can learn more about resistor from Wiki: [Resistor - Wikipedia](https://en.wikipedia.org/wiki/Resistor).



**Example**



- `blinking_mega` (Mega Board Project)

- `button_mega` (Mega Board Project)

- `blinking_uno` (R3 Board Project)

- `button_uno` (R3 Board Project)

- `14. GAME - Eat Apple` (Scratch Project)

- `2. Table Lamp` (Scratch Project)
