> Author: Ang Li  
> Blog: [angli.me](http://angli.me/)  
> Github: [github.com/leeang/Embedded-System-Design](https://github.com/leeang/Embedded-System-Design)

##Key Point
1. .c .h .s .elf .hex (flowchart)
1. Interrupt Service Routine
1. self documenting
1. diagram on datasheet page 8 / Assembly Language
1. etch a sketch code
1. diagram on datasheet page 51 / General Digital I/O
1. Power & LCD schematic


###AVR GCC Flowchart (2014 Q3 i)
![AVR GCC] (https://raw.githubusercontent.com/leeang/Embedded-System-Design/master/img/AVR-GCC.png)

High level language files get compiled into .s files (speaking Assembly Language) by the **C Compiler**. Then the **Assembler** forms a .obj file from these .s & .S files. At the next stage, the **Linker** links related libraries and adds the startup code to generate a .elf file. Eventually, the **object copy** converted the .elf file into a hex file which will be uploaded to the programmer.

###Interrupt Service Routine (2014 Q3 ii)
What happens during an ISR  
Interrupt arrives at the interrupt unit

If the Global Interrupt Enable (GIE) is set:  
1. **GIE** is cleared (instruction is CLI CLear Interrupt) disabling new interrupts  
2. The **program counter** is pushed to the stack  
3. The processor jumps to the **interrupt vector** (hard wired)  
4. The **interrupt vector** ordinarily has a jump instruction to the **ISR**  
5. Once the **ISR** is complete it runs a return from interrupt instantaneously  
6. This returns the previous **program counter** from the stack  
7. Enables **GIE** (instruction is SEI SEt Interrupt)  
8. Executes one more instruction before servicing any more ISRs

###Self documenting Code

	#define SET(REGISTER, MASK, VALUE) REGISTER = ((VALUE & MASK) | (REGISTER & ~MASK))
	#define GET(PIN, MASK) PIN & MASK

	#define SPI_ENABLE (_BV(SPIE) | _BV(SPE))
	SET(SPCE, SPI_ENABLE, ON);

An example of self-documenting code [2014 Q4 iii](https://github.com/leeang/Embedded-System-Design/blob/master/2014Q4-3.c)


###Assembly Language
![Architecture] (https://raw.githubusercontent.com/leeang/Embedded-System-Design/master/img/architecture.png)

	ldi r1, 0x19
	ldi r2, 0x24
	add r1, r2
	out PORTB, r1

Instructions are stored in program memory and executed consecutively.  
The program is stored in Flash Program Memory.

**Execution of program line 1**  
Load Immediate Register 1 25

+ **Program counter** points to program address with `ldi r1, 0x19`
+ On rising edge of clock1, the instruction is latched into the **instruction register** and the **program counter** is incremented
+ The instruction is decoded and the control signals are activated to do the following
  - Output the value 0x19 onto the **direct addressing** line
  - The **input** of register 1 is **enabled**
+ On clock2 rising edge 0x19 is latched into register 1

**Execution of program line 2**  
Load Immediate Register 2 36

+ **Program counter** points to program address with `ldi r2, 0x25`
+ On rising edge of clock2, the instruction is latched into the **instruction register** and the **program counter** is incremented
+ The instruction is decoded and the control signals are activated to do the following
  - Output the value 0x25 onto the **direct addressing** line
  - The **input** of register 2 is **enabled**
+ On clock3 rising edge 0x25 is latched into register 2

**Execution of program line 3**  
ADD Register 1 to Register 2

+ **Program counter** points to program address with `add r1, r2`
+ On rising edge of clock3, the instruction is latched into the **instruction register** and the **program counter** is incremented
+ The instruction is decoded and the control signals are activated to do the following
  - Output the value from register 1 to **input 1 of the ALU**
  - Output the value from register 2 to **input 2 of the ALU**
  - The ALU instruction is set to **ADD**
  - The **input** of register 1 is **enabled**

**Execution of program line 4**

+ **Program counter** points to program address with `out PORTB, r1`
+ On rising edge of clock4, the instruction is latched into the **instruction register** and the **program counter** is incremented
+ The instruction is decoded and the control signals are activated to do the following
  - Output the value from register 1 onto the **data bus**
  - The **input** of register for PORTB is **enabled**
+ On clock5 rising edge output of r1 is latched into PORTB, the output is then available on the external pins

###Etch a Sketch
	while(INFINITE_LOOP) {
		if(upButton) row++;
		if(downButton) row--;
		if(leftButton) column--;
		if(rightButton) column++;

		page = row/8;
		pixel = row%8;

		selectPage(page);
		selectColumn(column);

		pixel = (_BV(pixel) | (frameBuffer[column][page]));
		frameBuffer[column][page] = pixel;

		LCD_DataTx(pixel);
		
		_delay_ms(255);
	}

###General Digital I/O
![General Digital I/O] (https://raw.githubusercontent.com/leeang/Embedded-System-Design/master/img/General%20Digital%20I:O.png)

[How I/O pins work - Sahil Khanna](http://sahilkhanna.org/blog/wp-content/uploads/2014/11/how-io-pin-works.pdf)  

#####PORTx
+ A register that stores data that is available at the physical pin. Stores data on **WPx** rising edge.
+ This register can be both written to and read from. Outputs to the data bus if **RRx** is enabled (you can use it to check what is already there)
+ The register (all 8bits) are referred to as PORTx if referring to a particular bit you refer to it as PORTxn where x is the register, n is the bit.
+ AVR uses the most significant bit as the highest bit (0x80 is the MSB, 0x01 is the LSB)

#####DDRx
+ Data Direction Register x
+ A register that stores whether the output register (PORTx) is available on the physical pin. Stored when there is a rising edge on **WDx**
+ Connected to the tri-state buffer at the output of PORTx.
+ This register can be written and readfrom (you can use it to check what is already there). Available on data bus when **RDx** is enabled

#####Metastability
+ If the clock edge and the data edge occurs at the same time then a race condition occurs.
+ It will remain in this state until noise knocks it out of that state.
+ The time it is in the metastable state varies but takes on a probabilistic curve.

#####PINx
+ Diodes ensure that voltage on the pin are not 1 diode drop higher than VCC or 1 diode drop lower than GND
+ R<sub>pu</sub> is between 20k and 50k ohms

**What is the main use of the 2 stage synchronizer connected to each I/O pin?**

+ Two registers. If metastability occurs, it occurs at the first register.
+ By the time it arrives at the next register PINx, it should be stable.
+ There is a two I/O clock cycle delay before data arrives at the data bus.
+ When **RPx** is enabled the state at PINx is available on the bus.


###Power & LCD schematic



##Example Exam Question
####Lecture 3
Why use a PCB?  
Easier to assemble  
Neater  
More robust  


####Lecture 6
**Q1** 4V * 100mA / 90% / 3.2V = 139mA

**Q2** 90%


####Lecture 7
**Q1** Why would you not have an external pull-up resistor on an input to the ATmega 16?

Because ATMEGA16 input pins have software enabled pull up resistors.

**Q2** 1mAh / 1uA = 1000h = 41.7 days


####Lecture 8
**Q1** Why don't you drive the backlight of the LCD directly from the output of the ATmega 16?

Each microcontroller pin can only source 80mA, not enough to drive LCD.

**Q2** Why would you put a 15 ohm resistor in series with the MOSFET's gate?

For any MOSFET that G is essentially a capacitor, should have a resistor between G and a microcontroller to avoid a current surge.


####Lecture 9
**Q1** Why would you not place parts under a component such as an LCD or battery holder?

Never put SMD parts under other parts. Don't have parts under the LCD or the battery pack.  
Otherwise, we have to remove a large component to fix them. Debugging the circuit would be difficult without immediate access to the components.  
Heat dissipation might be an issue depending what these components are.  
There will not be even space to put up LCD over a big capacitor.

**Q2** What order did you place your components and why.

1 LCD, battery pack, buttons. (through-hole componets that influence the other side of the PCB; ergonomics consideration) 
2 microcontroller, crystal, decoupling capacitors, RAM. (high speed lines & sensitive analog lines)  
3 the rest.


####Lecture 11
**Q1** Why might you have a big ground plane and Vcc plane on a multilayer board?

Act as a good capacitor to decouple the system.  
Easy to make shorter tracks to the pins/devices that require GND or VCC.  
Guaranteed path of lowest inductance.

Ensure shortest possible return path for signals.

**Q2** Why would you terminate a PCB track with a resistor?

A signal travelling down an unterminated wire will reflect doubling the voltage on the way back.  
By matching the impedance of your track with the input impedance of the device, these reflections can be eliminated. (often with the help of a termination resistor)

**Q3** Why might you swap general IO pins around on a microcontroller?

In order to optimize PCB layout.  
General I/O pins are completely interchangeable.


####Lecture 16
**Q1** Referencing to page 8 of the Atmega 16 datasheet, describe what happens when the following instructions are executed by the ATmega 16.

**Q2** What is the most important part of soldering?

+ Heat transfer
+ Make sure the area is clean and the tip is well tinned.
+ Make sure the temperature is set to a suitable range(300 C). Otherwise, the solder might get oxidized.

####Lecture 17
**Q1** Describe how a file is converted from a .c and .h file into machine code.

**Q3** Why don't you use malloc or calloc on an AVR?

No operating system to catch faults.  
The microcontroller does not have an operating system that ensures important reserved memories are left untouched.

####Lecture 22
**Q2** Explain the steps to set up the ADC such that it samples ADC7 pin in reference to the internal 2.56v reference. Explain how to start a conversion and where the result is made available.

	void analogReadInit(void) {
		ADMUX = _BV(REFS1) | _BV(REFS0) | _BV(MUX2) | _BV(MUX1) | _BV(MUX0);
		ADCSRA = _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
	}

	unsigned int analogRead(void) {
		ADCSRA |= _BV(ADSC);
		while( ADCSRA & _BV(ADSC) );
		return ADC;
	}

**Q4** Why would you place a small ground plane around the crystal with lots of vias?

If possible, place a guard ring (connected to ground) around the crystal. This helps isolate the crystal from noise coupled from adjacent signals. http://www.emesystems.com/pdfs/parts/DS1307_xtal.pdf
