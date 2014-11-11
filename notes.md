##Key Point
1. Interrupt Service Routine
1. diagram on datasheet page 8 / Assembly Language
1. self documenting
1. etch a sketch code
1. diagram on datasheet page 51 / General Digital I/O
1. .c .h .s .elf .hex (flowchart)
1. LCD & power schematic

###Interrupt Service Routine
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

###Assembly Language
![Architecture] (https://raw.githubusercontent.com/leeang/Embedded-System-Design/master/img/architecture.png)

	ldi r1, 0x19
	ldi r2, 0x24
	add r1, r2
	out PORTB, r1


**Execution of program line 1**  
Load Immediate Register 1 25

+ **Program counter** points to program address with `ldi r1, 0x19`
+ On rising edge of clock1, the instruction is latched into the **instruction register** and the **program counter** is incremented
+ The instruction is decoded and the control signals are activated to do the following
 - Output the value 0x19 onto the **direct addressing line**
 - The **input** of register 1 is **enabled**
+ On clock2 rising edge 0x19 is latched into register 1

**Execution of program line 2**  
Load Immediate Register 2 36

+ **Program counter** points to program address with `ldi r2, 0x25`
+ On rising edge of clock2, the instruction is latched into the **instruction register** and the **program counter** is incremented
+ The instruction is decoded and the control signals are activated to do the following
 - Output the value 0x25 onto the **direct addressing line**
 - The **input** of register 2 is **enabled**
+ On clock3 rising edge 0x25 is latched into register 2

**Execution of program line 3**  
ADD Register 1 to Register 2

+ **Program counter** points to program address with `add r1, r2`
+ On rising edge of clock3, the instruction is latched into the **instruction register** and the **program counter** is incremented
+ The instruction is decoded and the control signals are activated to do the following
 - Output the value from register 1 to input 1 of the ALU
 - Output the value from register 2 to input 2 of the ALU
 - The ALU instruction is set to ADD
 - The **input** of register 1 is **enabled**

**Execution of program line 4**
+ **Program counter** points to program address with `out PORTB, r1`
+ On rising edge of clock4, the instruction is latched into the **instruction register** and the **program counter** is incremented
+ The instruction is decoded and the control signals are activated to do the following
 - Output the value from register 1 onto the **data bus**
 - The **input** of register for PORTB is **enabled**
+ On clock5 rising edge output of r1 is latched into PORTB, the output is then available on the external pins

###Self documenting Code
	#define ON 0xFF
	#define INFINITE_LOOP 1

	#define SET(REGISTER, MASK, VALUE) REGISTER = ((VALUE & MASK) | (REGISTER & ~MASK))
	#define GET(PIN, MASK) PIN & MASK

	SET(PORTB, _BV(PB6), ON);

	#define SPI_ENABLE (_BV(SPIE) | _BV(SPE))
	SET(SPCE, SPI_ENABLE, ON);

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

###AVR GCC Flowchart
![AVR GCC] (http://upload.wikimedia.org/wikipedia/commons/0/0f/Avr-gcc.png)

High level language files get compiled into .s files (speaking Assembly Language) by the **C Compiler**. Then the **Assembler** forms a .obj file from these .s & .S files. At the next stage, the **Linker** links related libraries and adds the startup code to generate a .elf file. Eventually, the .elf file is converted into a hex file (which will be uploaded to the programmer) by the **object copy**.

##Example Exam Question
####Lecture 6
**Q1**  
4V * 100mA / 90% / 3.2V = 139mA

**Q2**  
90%


####Lecture 7
**Q1**  
Because ATMEGA16 input pins have software enabled pull up resistors.

**Q2**  
1mAh / 1uA = 1000h = 41.7 days


####Lecture 8
**Q1**  
Each microcontroller pin can only source 80mA, not enough to drive LCD.

**Q2**  
For any MOSFET that G is essentially a capacitor, should have a resistor between G and a microcontroller to avoid a current surge.


####Lecture 9
**Q1**  
Why would you not place parts under a component such as an LCD or battery holder?  
Never put SMD parts under other parts. Don't have parts under the LCD or the battery pack.  
Otherwise, we have to remove a large component to fix them. Debugging the circuit would be difficult without immediate access to the components.  
Heat dissipation might be an issue depending what these components are.  
There will not be even space to put up LCD over a big capacitor.

**Q2**  
What order did you place your components and why.  
1 LCD, battery pack, buttons. (through-hole componets that influence the other side of the PCB) 
2 microcontroller, crystal, decoupling capacitors, RAM. (high speed lines & sensitive analog lines)  
3 the rest.


####Lecture 11
**Q1**  
Why might you have a big ground plane and Vcc plane on a multilayer board?

Act as a good capacitor to decouple the system.  
Easy to make shorter tracks to the pins/devices that require GND or VCC.  
Guaranteed path of lowest inductance.

Ensure shortest possible return path for signals.

**Q2**  
Why would you terminate a PCB track with a resistor?

A signal travelling down an unterminated wire will reflect doubling the voltage on the way back.  
By matching the impedance of your track with the input impedance of the device, these reflections can be eliminated. (often with the help of a termination resistor)

**Q3**  
Why might you swap general IO pins around on a microcontroller?

In order to optimize PCB layout.  
General I/O pins are completely interchangeable.


####Lecture 17
**Q1**  
Describe how a file is converted from a .c and .h file into machine code.

**Q3**  
No operating system to catch faults.  
The microcontroller does not have an operating system that ensures important reserved memories are left untouched.



##Lecture Notes

####Lecture 1
tacit knowledge n.  
隐性知识

self-documenting adj.  
In computer programming, self-documenting (or self-describing) is a common descriptor for source code and user interfaces that follow certain loosely defined conventions for naming and structure. These conventions are intended to enable developers, users, and maintainers of a system to use it effectively without requiring previous knowledge of its specification, design, or behavior.

fabricate v.  
装配

non volatile FeRAM n.  
Non-volatile memory, nonvolatile memory, NVM or non-volatile storage is computer memory that can get back stored information even when not powered. Examples of non-volatile memory include read-only memory, flash memory, ferroelectric RAM (F-RAM), most types of magnetic computer storage devices (e.g. hard disks, floppy disks, and magnetic tape), optical discs, and early computer storage methods such as paper tape and punched cards.

tinker v.  
If you tinker with something, you make some small changes to it, in an attempt to improve it or repair it. 小修改

apprehension n.  
Apprehension is a feeling of fear that something bad may happen. 忧虑 [正式]

commitment n.  
A commitment is something which regularly takes up some of your time because of an agreement you have made or because of responsibilities that you have. 投身的事

disciplinary n.  
Disciplinary bodies or actions are concerned with making sure that people obey rules or regulations and that they are punished if they do not. 纪律性的

laying around adj.  
到处都是

plier n.  
钳子

side cutters n.  
Diagonal pliers (or wire cutters or diagonal cutting pliers or diagonal cutters) are pliers intended for the cutting of wire (they are generally not used to grab or turn anything).

pro-active adj.  
积极主动的

assumed knowledge n.  
先修课程


####Lecture 2 Revision
0805 resistor max power dissipation = 0.125 Watt.

dope v.  
掺杂 doped semiconductor

permit v.  
If a situation permits something, it makes it possible for that thing to exist, happen, or be done or it provides the opportunity for it. 使成为可能; 成为可能

static discharge n.  
静电放电

arbitrary adj.  
任意的

tristate n.  
In digital electronics three-state, tri-state, or 3-state logic allows an output port to assume a high impedance state in addition to the 0 and 1 logic levels, effectively removing the output from the circuit.  
tristate = not connected = floating

peripheral n. /pəˈrɪfərəl/  
Peripherals are devices that can be attached to computers. (计算机的) 外围设备 [计算机]


####Lecture 3 Printed Circuit Boards
plage v.  
电镀

fibreglass n.  
Fibreglass is a material made from short, thin threads of glass which can be used to stop heat from escaping. 玻璃纤维

neat adj.  
A neat place, thing, or person is organized and clean, and has everything in the correct place. 整洁的

lithography n. /lɪˈθɒɡrəfɪ/  
Lithography is a method of printing in which a piece of stone or metal is specially treated so that ink sticks to some parts of it and not to others. 平板印刷术

corrode v.  
If metal or stone corrodes, or is corroded, it is gradually destroyed by a chemical or by rust. 腐蚀

lacquer /ˈlækə/ n.  
Lacquer is a special liquid which is painted on wood or metal in order to protect it and to make it shiny. 保护漆


#### Lecture 5 Power Supply
battery holder n.  
电池盒

buck converter n.  
降压转换器

boost converter n.  
step-up converter  
升压转换器

Zener diode /ˈziːnə/ n.  
a semiconductor diode that exhibits a sharp increase in reverse current at a well-defined reverse voltage: used as a voltage regulator 齐纳二极管 稳压二极管

anode /ˈænəʊd/ n.  
In electronics, an anode is the positive electrode in a cell such as a battery. 阳极

cathode /ˈkæθəʊd/ n.  
A cathode is the negative electrode in a cell such as a battery. 阴极


####Lecture 6 Push Buttons, Interrupts, LEDs
simultaneous /ˌsɪməlˈteɪnɪəs/ adj.  
Things which are simultaneous happen or exist at the same time. 同时的

polling n.  
Polling, or polled operation, in computer science, refers to actively sampling the status of an external device by a client program as a synchronous activity. Polling is most often used in terms of input/output (I/O), and is also referred to as polled I/O or software-driven I/O. 论询

Polling is checking if the button is pressed over and over again.  
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
LED  
Voltage drop across a diode depends on the colour of the diode.
Current across an ideal diode is infinite, must always have a resistor in series or magic smoke escapes.

spin n.  
自旋

electron n.  
电子


####Lecture 8 Crystals and PCBs
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
High frequency tracks take the path of least inductance.


####Lecture 9 PCB Do's and Don'ts
Ground plane provides short signal return loops.

Should tent vias underneath components.

Should avoid vias near or on SMD pads, they will either bridge or siphon away solder.

siphon /ˈsaɪfən/  v.
If you siphon liquid from a container, you make it come out through a tube and down into a lower container by enabling the pressure of the air on it to push it out. 用虹吸管抽吸

0603 for some capacitors  
0805 for resistors, diodes.


####Lecture 10 Advanced PCB Design
annular /ˈænjʊlə/ adj.  
ADJ ring-shaped; of or forming a ring 环状的; 环的或形成环的

antenna /ænˈtɛnə/ AmE n.  
aerial /ˈɛərɪəl/ BrE n.  
An aerial is a device or a piece of wire that sends and receives television or radio signals and is usually attached to a radio, television, car, or building. 天线

crosstalk /ˈkrɒsˌtɔːk/ n.  
unwanted signals in one channel of a communications system as a result of a transfer of energy from one or more other channels 电信频道之间的串话干扰


####Lecture 11 Finding Components


####Lecture 12 Microcontroller Internals
operand n.  
a quantity or function upon which a mathematical or logical operation is performed 操作数


####Lecture 13 Microcontroller Internals
rehash  
n. If you describe something as a rehash, you are criticizing it because it repeats old ideas, facts, or themes, though some things have been changed to make it appear new. 重复; 老调重弹  
v. If you say that someone rehashes old ideas, facts, or accusations, you disapprove of the fact that they present them in a slightly different way so that they seem new or original. 只作轻微改动; 换汤不换药

ALU  
Arithmetic Logical Unit


####Lecture 14 Microcontroller Internals
consecutive adj.  
Consecutive periods of time or events happen one after the other without interruption. 连续的

metastability n.  
亚稳态


####Lecture 15 Assembly, Testing, JTAG
flush adj.  
level or even with another surface (与另一平面)平齐的

tweezer n.  
Tweezers are a small tool that you use for tasks such as picking up small objects or pulling out hairs or splinters. Tweezers consist of two strips of metal or plastic joined together at one end. 镊子

molten adj.  
Molten rock, metal, or glass has been heated to a very high temperature and has become a hot, thick liquid. 熔化的

batch n.  
A batch of things or people is a group of things or people of the same kind, especially a group that is dealt with at the same time or is sent to a particular place at the same time. (一) 批

wick n.  
The wick of a candle is the piece of string in it that burns when it is lit. 蜡烛芯  
The wick of an oil lamp or cigarette lighter is the part that supplies the fuel to the flame when it is lit. (煤油灯的)灯芯; (打火机的)棉芯

stencil n.  
A stencil is a piece of paper, plastic, or metal which has a design cut out of it. You place the stencil on a surface and paint it so that paint goes through the holes and leaves a design on the surface. 模版

squeegee  
n. an implement with a rubber blade used for wiping away surplus water from a surface, such as a windowpane 橡胶擦刷器
v. to remove (water or other liquid) from (something) by use of a squeegee 用擦刷器擦去(水或其它液体)

blow up v.  
If someone blows something up or if it blows up, it is destroyed by an explosion. 爆炸

intermittent adj.  
Something that is intermittent happens occasionally rather than continuously. 断断续续的


####Lecture 17 Interrupts and Good Code


####Lecture 20 Exam Rules
disruptive adj.  
To be disruptive means to prevent something from continuing or operating in a normal way. 妨碍的; 扰乱的

surrender v.  
If you surrender something you would rather keep, you give it up or let someone else have it, for example after a struggle. 放弃; 交出

loose adj.  
Something that is loose is not attached to anything, or held or contained in anything. 零散的

escort v. /ɪsˈkɔːt/  
If you escort someone somewhere, you accompany them there, usually in order to make sure that they leave a place or get to their destination. 护送

If something has a bearing on a situation or event, it is relevant to it. 与…有关系

interpretation n.  
An interpretation of something is an opinion about what it means. 解释

diagonal adj.  
A diagonal line or movement goes in a sloping direction, for example, from one corner of a square across to the opposite corner. 对角线的; 斜的

##Q & A on Facebook
suction n.  
Suction is the process by which liquids, gases, or other substances are drawn out of somewhere. 抽吸(液体、汽体等)

vent n.  
A vent is a hole in something through which air can come in and smoke, gas, or smells can go out. 通风孔; 排放口


sluggish adj.  
You can describe something as sluggish if it moves, works, or reacts much slower than you would like or is normal. 缓慢的; 迟钝的

inrush n.  
a sudden usually overwhelming inward flow or rush; influx 涌入; 流入

in the proximity of...  
在...附近

intrinsic adj.  
If something has intrinsic value or intrinsic interest, it is valuable or interesting because of its basic nature or character, and not because of its connection with other things. 内在的; 本质的 [正式]

sequential adj.  
Something that is sequential follows a fixed order. 按次序的; 顺序的 [正式]

untouched adj.  
Something that is untouched by something else is not affected by it. 不受影响的

interference n.  
When there is interference, a radio signal is affected by other radio waves or electrical activity so that it cannot be received properly. 干扰

explicit adj.  
Something that is explicit is expressed or shown clearly and openly, without any attempt to hide anything. 明确表达的; 公开显露的

reverberation /rɪˌvɜːbəˈreɪʃən/ n.  
A reverberation is the shaking and echoing effect that you hear after a loud sound has been made. 回响; 回声
