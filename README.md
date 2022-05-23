# cellarium
**cellarium** is a graphical simulation of a discrete world inhabited by cells
that exists according to the laws of an evolutionary algorithm.
<br/>
<br/>

## How does it work
<details>
<summary>Click to see how does it work</summary>
<br/>

This is a cellular automaton that is an evolution simulator.
The world is discrete by space and time.
Space is verticaly limited and horizontaly closed.
In fact simulation world is a two-dimensional section of water
where there are surface on top and bottom down below.

Water is habited by cells. At the beginning there is only one cell.
Each cell ocuppies its own space cell, has own energy level,
command counter, genome, etc.

Genome is a tape of certain size closed at the ends that consists of
separate gens expressed by integers.
Each gene means certain instruction, for example: turn, move, eat, touch.

Command counter is an integer that means an address of current gene.

On each step each cell looks at its command counter, looks at this address
in genome and executes instruction.
Than command counter is incremented.
Conditional and inconditional jumps are available.
According to the principle of the genome, the cell
is similar to the Turing machine.

On each step energy level of cell is decremented and in the case
it reaches zero the cell dies and can disappear or turn into organic.

Energy can be received from photosynthesis, minerals or food.
Photosynthesis is possible during daytime and is more effective
closer to the surface.
Minerals are accumulated more effective closer to the bottom
and can be converted into energy.
Food can be living cells and organic.

The cell that gains enough energy for reproduction is able to bud.
If the cell gains maximal amount of energy it has to bud
and if it can't - it turns into organic.
During budding genome from maternal cell is copied to daughterly one
with some chance of mutation. That's how benign and malignant
behaviour changes can appear.

So cell genome determines its behaviour in simulation.
Cells with the most successful genome survive and give offspring,
other cells die.
Thus the simulation has basic evolutionary factors implemented:
mutations and natural selection.

Since genome determines behaviour of the cell several formation
can arise in the simulation, for example: colonies, winds,
singletons, worms, etc.

Application interface allows you to change simulation speed,
show cells in different modes (diet, energy, etc.)
and also follow specific cell and review its genome.

All simulation parameters can be setted which allows
to establish correspondence of sets of rules to formations
that arise in such conditions.
This is one of the targets of cybernetics.

</details>
</br>

### Demo
[![Here must be demo](https://media.istockphoto.com/photos/barbary-macaque-picture-id824860820?k=20&m=824860820&s=612x612&w=0&h=W8783ZGcqMfDMJoXvBAyDFcSjnOWdKqKhgLGvf-VIuU=)](https://www.youtube.com/watch?v=dQw4w9WgXcQ "Default configuration demo")
</br>
</br>

### Simulation rules
<details>
<summary>Click to see simulation rules</summary>
<br/>

- features of the environment:
    - discrete in space (square cells) and time (ticks, or steps)
    - space above and below is limited, on the sides - closed
    - the ability to get energy from photosynthesis closer
    to the top and minerals - to the bottom
    - change of seasons (moving vertical boundaries that determine
    light zone; can be disabled)
    - change of seasons (the distance between these vertical boundaries changes;
    can be disabled)
    - neighboring cells interfere with the passage of solar energy
    - gamma flash every few steps, causing mutations
    random number of genes of each cell
    - the energy of the dead cell (organic) does not change and is equal to the energy
    cell at the time of her death
    - dead cells (organic) with each tick fall to one cell
    down if there is free space and then settle to the bottom or
    (adjustable) stops after the first obstacle
- in each run, each cell executes a command according to the genome
- cells are processed sequentially with increasing age (so younger cells
have a faster "reaction")
- each cell has a certain set of properties:
    - energy level - an indicator of cell viability:
        - with each tick the level is decremented
        - With the depletion of energy, the cell dies and disappears or becomes organic
        (customizable)
    - cell direction - is determined by an integer from 0 to 7 (0 - up,
    7 - left up)
    - cell position - two positive integers (x - column, y - row)
    - command counter - indicates the command in the genome to be executed
    - genome - closed from the ends of the tape of constant size of integers - genes,
    each of which corresponds to a specific command:
        - commands are divided into final (command counter is incremented)
        and intermediate (conditional transition is performed):
            - the number of intermediate commands per turn is limited to a certain number,
            the number of finals is one
        - the number of commands is equal to the size of the genome (for unconditional transitions)
        and a multiple of 8 (for the number of directions)
        - list of teams:
            - 0 - inaction (final)
            - 1 - return (intermediate):
                - the angle of rotation is the remainder of the division by 8 values ​​of the gene № + 1
            - 2 - move (final):
                - movement in the direction that is the remainder of the division by 8 sums
                own direction and the value of the gene № + 1
                - if the movement failed, the command to touch is executed,
                and the command counter is not incremented
            - 3 - photosynthesis (final):
                - getting energy from the sun
                - the farther from the ellipse formed by the Sun, the less solar
                light
                - time of day affects the horizontal placement of the maximum
                light
                - time of year affects the intensity of light
            - 4 - mineralosynthesis (final):
                - extraction of energy from accumulated minerals
                - minerals accumulate every step (there is a limit)
                - the deeper, the more minerals
            - 5 - eat (final):
                - conversion into food energy in the direction that is the remainder of
                division by 8 sums of the self-direction and the value of the gene № + 1
                - the received energy is equal to the energy of food and does not exceed a certain one
                maximum energy from food or equal to this maximum
                energy (customizable)
            - 6 - bud (final):
                - the daughter cell buds in the direction that is the rest
                from the division by 8 of the sum of its own direction and the value of the gene № + 1
            - 7 - mutate (final):
                - mutation of an arbitrary gene
            - 8 - transfer energy (final):
                - energy transfer in the direction that is the remainder of the division by 8
                the sum of its own direction and the value of the gene № + 1
                - the amount of transmitted energy is equal to the given value
                (value / number of commands * current energy level)
                gene № + 2
            - 9 - touch (intermediate):
                - determine the object in the direction that is the remainder of the division by 8
                the sum of its own direction and the value of the gene № + 1
                - if the front is empty - is added to the command counter
                the value of the gene № + 2
                - if there is organic in front - it is added to the command counter
                the value of the gene № + 3
                - if there is a related cell in front - to the command counter
                the value of the gene № + 4 is added
                - if someone else's cell is in front - to the command counter
                the value of the gene № + 5 is added
                - in a friendly cell the genome differs no more than in
                a certain number of genes
            - 10 - determine the level of own energy (intermediate):
                - control level is a reduced value (value /
                number of commands * maximum energy) gene № + 1
                - if own level is less than control - to command
                the value of the gene № + 2 is added to the counter
                - if the own level is greater than or equal to the control -
                the value of the gene № + 3 is added to the command counter
            - 11 - determine the depth (intermediate):
                - control depth is the given value (value /
                number of commands * maximum depth) of the gene № + 1
                - if the own depth is less than the control - to the command
                the value of the gene № + 2 is added to the counter
                - if the own depth is greater than or equal to the control -
                the value of the gene № + 3 is added to the command counter
            - 12 - determine the level of available solar energy (intermediate):
                - control level is a reduced value (value /
                number of commands * maximum energy from the sun) gene № + 1
                - if own level is less than control - to command
                the value of the gene № + 2 is added to the counter
                - if the own level is greater than or equal to the control -
                the value of the gene № + 3 is added to the command counter
            - 13 - determine the level of available minerals (intermediate):
                - control level is a reduced value (value /
                number of commands * maximum energy from minerals) gene № + 1
                - if own level is less than control - to command
                the value of the gene № + 2 is added to the counter
                - if the own level is greater than or equal to the control -
                the value of the gene № + 3 is added to the command counter
            - 14 - determine the level of available energy from accumulated minerals
            (intermediate):
                - control level is a reduced value (value /
                number of commands * maximum energy from minerals) gene № + 1
                - if own level is less than control - to command
                the value of the gene № + 2 is added to the counter
                - if the own level is greater than or equal to the control -
                the value of the gene № + 3 is added to the command counter
            - all other numbers are unconditional transition commands (add to
            command counter its value)
        - cell color - three positive integers (RGB)
        - energy transfer balance - the difference between transmitted and received energy
        for all time
        - past energy transfer - the symbolic value of the last transferred
        energy (-1, 0 or 1), the value of which fades with time
    - mutations:
        - mutation - change of a random gene to a random value
        - the highest chance of mutation in budding (equal to the maternal,
        and the daughter cell), the smallest - when following instructions
    - reproduction:
        - reproduction occurs by budding of a daughter cell
        - energy is divided in half
        - the daughter cell inherits the color and direction of the mother
        - there is a complete copying of the genome into a daughter cell
        - there is a separate chance of mutation in budding, the same for both cells
        - the daughter cell buds in the first direction of the first free
        cells that are checked clockwise starting from the direction
        mother cell (optional)
        - a cell must multiply if its energy is not less than the maximum
        energy (optional)
        - a cell can multiply if its energy is not less than the minimum
        energy for separation, and the budding command is executed
        - if the cell needs to bud but does not have room for it -
        it becomes organic with energy conservation (optional)

</details>
</br>

### Formations
<details>
<summary>Click to see formation</summary>
<br/>

Here are founded formations:
- gas - cells that move in constant random direction
- singletons - stationary cells that keep distance to each other
- chess - stationary cells located in chess order
- wind - cells that move in constant horizontal direction
- jumpers - cells that move from side to side
- spinners - cells that move in a circle
- sticks - stationary cells located in the form of stick
- diagonal sticks - stationary cells located in the form of stick diagonaly
- worms - cells that located one by one with a certain curvature
- colonies - cells that stick to each other and eat strangers:
    - colonies are possible only if cells can differentiate each other
    - if cells are familiar having less than 3 different gens superpower colony can appear
    - colony nucleation is similar to some kind of wave
    - neighbour colonies extruse each other by eating outside cells
    - when in a colony some cells differ enough to be foreign
    cracks appear that can grow and divide the colony
    - each colony has its own energy sharing and distribution specifics

</details>
</br>

### Configuration presets
<details>
<summary>Click to see configuration presets</summary>
<br/>

Here are available configuration presets and formations that can be found in them:
- default: spinners, worms, colonies, gas, chess, sticks, wind
- akin2: spinners, chess, singletons, wind, colonies, superpower colony
- childEnergy1maximizeFood0: flame
- childEnergy1maximizeFood1: red terror
- daytimes: spinners, gas, wind, colonies
- pinOrganic0: gas, sticks, diagonal sticks, colonies, wind
- cellSize4: just a little bigger world
- mineralEnergy04: worms, spinners, diagonal sticks, jumpers, colonies
- mineralEnergy08: wind, singletons, colonies
- mineralEnergy10: sticks, singletons, jumpers, spinners, colonies

</details>
</br>

## Building
<details>
<summary>Click to see how to build it</summary>
<br/>

The prototype of application is written in Processing so if you don't want
to build the application yourself you can download Processing 3.5+ and run it.
However, Processing version does not contain all available rules,
configuration feature and GUI.

The final version of application is written in C++ using:
1. Dear ImGui
2. OpenGL 4.6 (GLFW and GLAD)
3. Qt 5

Therefore before building you need to satisfy all the dependencies
by taking these steps:
1. Install Qt using your system package manager or [official installer](https://www.qt.io/download).
2. Download [GLFW](https://www.glfw.org/download), build it,
then place static library file in ```c++/lib/```
and header directories in ```c++/include/```.
3. Download [GLAD](https://glad.dav1d.de/),
build it as static or shared library and place it in ```c++/lib/```
or place source file in ```c++/src/cellarium/glad/```,
then place header directory in ```c++/include/```.
4. Clone [Dear ImGui](https://github.com/ocornut/imgui) repository,
then copy all the source and header files from the repository's root
and also ```imgui_impl_glfw*``` with ```imgui_impl_opengl3*``` from ```backends\```
to ```c++/src/cellarium/dear_imgui/```.

To build the application run these commands from ```c++/``` directory:
```bash
cmake .
cmake -build .
```
You will have ```./bin/cellarium``` builded.

</details>
</br>

## Using
<details>
<summary>Click to see how to use it</summary>
<br/>

Run the application with ```-h``` parameter to see help information.
Use ```-g``` parameter to generate default configuration so you can edit it and use.
Run the application with configuration file specified to start simulation.

</details>
</br>
