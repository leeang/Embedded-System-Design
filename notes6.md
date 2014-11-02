####Lecture 6 Push Buttons, Interrupts, LEDs

**Q1**  
4V * 100mA / 90% / 3.2V = 139mA

**Q2**  
90%

simultaneous /ˌsɪməlˈteɪnɪəs/ adj.  
Things which are simultaneous happen or exist at the same time. 同时的

polling n.  
Polling, or polled operation, in computer science, refers to actively sampling the status of an external device by a client program as a synchronous activity. Polling is most often used in terms of input/output (I/O), and is also referred to as polled I/O or software-driven I/O. 论询

Polling is checking if the button is pressed over and over agian.  
wastes CPU cycles  
known reaction time.

Interrupt creates a signal when a button is pressed.  
CPU does not check signal until the button is pressed.
Reaction time less predictable.

predictable adj.  
If you say that an event is predictable, you mean that it is obvious in advance that it will happen. 可预见的

Reset button  
Good practice to use an external pull up and possibly a capacitor to ensure no accidental reset due to noise in the system.

####Lecture 7 LEDs, SPI, LCD, RAM & ADC

**Q1**  
Because ATMEGA16 input pins have software enabled pull up resistors.

**Q2**  
1mAh / 1uA = 1000h = 41.7 days

LED  
Voltage drop across a diode depends on the colour of the diode.
Current across an ideal diode is infinite, must always have a resistor in series or magic smoke escapes.

spin n.  
自旋

electron n.  
电子

####Lecture 8 Crestals and PCBs

**Q1**  
Each microcontroller pin can only source 80mA, not enough to drive LCD.

**Q2**  
For any MOSFET that G is essentially a capacitor, should have a resistor between G and a microcontroller to avoid a current surge.

ergonomic /ˌɜːɡəˈnɒmɪk/ adj.  
of or relating to ergonomics 人类工程学的

**high frequency signals** as short as possible  
**high current wires** as short as possible

SMD = Surface Mount Device

interlock v.  
互锁

**Crystal**  
All transfers between registers occur on clock edges.  
Most critical part of the circuit.
Highest frequency device in the system.
Analog system, sensitive to noise.

piezoelectric adj.  
压电的

electric charge n.  
电荷

**Decoupling capacitors**  
Close to each power pin as possible.
Second highest frequency parts.

**PCB tracks**  
Wider, less inductance.
High current tracks take the path of least resistance.
High frequency tracks take the path of least indcutance.

####Lecture 9 PCB Do's and Don'ts

**Q1**  
Never put SMD parts under other parts. Don't have parts under the CLD or the battery pack.

**Q2**  
1 LCD, battery pack, buttons.  
2 microcontroller, crystal, decoupling capacitors, RAM. (high speed lines & sensitive analog lines)  
3 the rest.

