# cellarium
**cellarium** is a graphical simulation of a discrete world inhabited by cells
that exists according to the laws of an evolutionary algorithm.
<br/>
<br/>

## How does it work
<details>
<summary>Click to see how does it work</summary>
<br/>

It is a cellular automaton that is a simulator of evolution.
The world is discrete in space and time.
Space is limited vertically and closed horizontally.
In fact, the world of simulation is a two-dimensional section of a reservoir where
top surface and bottom - bottom.

The reservoir is inhabited by cells. At first only one.
Each cell occupies one cell in space,
has a certain energy level, command counter and genome.

The genome is a closed tape of constant size,
consisting of genes that are expressed as integers.
Each such gene means a certain instruction, for example:
return, move, eat, touch.

The command counter is a number that is the address of the current gene.

Every step each cell looks at its command counter,
finds the gene - the instruction, and executes it.
After each command is executed, the counter is incremented.
Conditional and unconditional transitions are also possible.
According to the principle of the genome, the cell is somewhat similar to the Turing machine.

Each step the energy level of the cell is decremented, and,
if it drops to zero, the cell dies and may disappear
or turn into organic.

Energy can be obtained from photosynthesis, minerals and food.
Photosynthesis is available during the day and is more efficient closer to the surface.
Minerals accumulate more efficiently closer to the bottom and can
be converted into energy.
Food can be living cells and organic matter.

A cell that gets enough energy to reproduce
can rest.
If the cell has received the maximum amount of energy, it is obliged
rest, and if this is not possible, it turns
in organic.
The genome is copied from the mother cell to the daughter, and at the same time is
some chance of mutation. This is how benign or malignant can occur
changes in cell behavior.

So the cell's genome determines its behavior in the simulation.
Cells with the most successful genome survive and give offspring,
and cells with a less successful genome die.
Thus, the main factors of evolution are realized in the simulation:
mutations and natural selection.

Because the cell's genome determines its behavior, in a simulation
certain formations may occur, for example:
colonies, solitaries, worms, etc.
Changing environment properties and switching simulation rules
directly affect the formation of such formations.

The program interface allows you to control the progress of the simulation,
view statistics, monitor individual cells
and view all their properties, including the genome,
as well as display its simulation in different modes:
diet, energy, age, etc.

All simulation settings can be configured before startup,
that allows to establish correspondence of set of rules to formations,
which occur in such conditions.

</details>
</br>

### Demo
[![Here must be demo](https://img.youtube.com/vi/T9Jqv2ou00k/hqdefault.jpg)](https://www.youtube.com/watch?v=T9Jqv2ou00k "\"default\" preset demo")
[![Here must be demo](https://img.youtube.com/vi/zMXJDCG_DDw/hqdefault.jpg)](https://www.youtube.com/watch?v=zMXJDCG_DDw "GUI demo")
</br>
</br>

### Simulation rules
<details>
<summary>Click to see simulation rules</summary>
<br/>

- features of the environment:
    - discrete in space (square cells) and time (ticks, or steps)
    - space above and below is limited, on the sides - closed
    - the ability to obtain energy from photosynthesis closer to the top and minerals - to the bottom
    - change of time of day (the vertical borders defining a zone of light move; it is possible to disconnect)
    - change of seasons (changes the distance between these vertical boundaries; can be disabled)
    - Neighboring cells interfere with the passage of solar energy
    - gamma flash once in a certain number of steps, which causes mutations in a random number of genes in each cell
    - the energy of a dead cell (organic) does not change and is equal to the energy of the cell at the time of its death
    - dead cells (organics) with each tick fall one cell down if there is free space and then settle to the bottom or (tuned) stop after the first obstacle
- during each run, each cell executes a command according to the genome
- cells are processed sequentially with increasing age (so younger cells have a faster "reaction")
- each cell has a certain set of properties:
    - energy level - an indicator of cell viability:
        - with each tick the level is decremented
        - with the depletion of energy, the cell dies and disappears or becomes organic (tuned)
    - cell direction - determined by an integer from 0 to 7 (0 - up, 7 - left up)
    - cell position - two positive integers (x - column, y - row)
    - command counter - indicates the command in the genome to be executed
    - genome - a closed from the ends of a strip of constant size of integers - genes, each of which corresponds to a specific command:
        - commands are divided into final (command counter is incremented) and intermediate (conditional transition is performed):
            - the number of intermediate commands per turn is limited to a certain number, the number of final - one
        - the number of commands is equal to the size of the genome (for unconditional transitions) and a multiple of 8 (for the number of directions)
        - list of teams:
        - 0 - inaction (final)
            - 1 - return (intermediate):
                - the angle of rotation is the remainder of the division by 8 values ​​of the gene № + 1
            - 2 - move (final):
                - movement in the direction that is the remainder of the division by 8 of the sum of its own direction and the value of the gene № + 1
                - if the movement fails, the command to touch is executed, and the command counter is not incremented
            - 3 - photosynthesis (final):
                - getting energy from the sun
                - the farther from the ellipse formed by the Sun, the less sunlight
                - time of day affects the horizontal placement of maximum light
                - time of year affects the intensity of light
            - 4 - mineralosynthesis (final):
                - extraction of energy from accumulated minerals
                - minerals accumulate every step (there is a limit)
                - the deeper, the more minerals
            - 5 - eat (final):
                - conversion into food energy in the direction that is the remainder of the division by 8 of the sum of its own direction and the value of the gene № + 1
                - the received energy is equal to the energy of food and does not exceed a certain maximum energy from food or equal to this maximum energy (adjustable)
            - 6 - bud (final):
                - the daughter cell buds in the direction that is the remainder of the division by 8 sums of its own direction and the value of the gene № + 1
            - 7 - mutate (final):
                - mutation of an arbitrary gene
            - 8 - transfer energy (final):
                - energy transfer in the direction that is the remainder of the division by 8 of the sum of its own direction and the value of the gene № + 1
                - the amount of transmitted energy is equal to the given value (value / number of commands * current energy level) of the gene № + 2
            - 9 - touch (intermediate):
                - determine the object in the direction that is the remainder of the division by 8 of the sum of its own direction and the value of the gene № + 1
                - if the front is empty - the value of the gene № + 2 is added to the command counter
                - if in front of the organic - the value of the gene № + 3 is added to the command counter
                - if there is a related cell in front - the value of the gene № + 4 is added to the command counter
                - if there is a foreign cell in front - the value of the gene № + 5 is added to the command counter
                - in a friendly cell the genome differs by no more than a certain number of genes
            - 10 - determine the level of own energy (intermediate):
                - control level is the reduced value (value / number of commands * maximum energy) of the gene № + 1
                - if the intrinsic level is less than the control - the value of the gene № + 2 is added to the command counter
                - if the intrinsic level is greater than or equal to the control - the value of the gene № + 3 is added to the command counter
            - 11 - determine the depth (intermediate):
                - control depth is the reduced value (value / number of commands * maximum depth) of the gene № + 1
                - if the natural depth is less than the control - the value of the gene № + 2 is added to the command counter
                - if the natural depth is greater than or equal to the control - the value of the gene № + 3 is added to the command counter
            - 12 - determine the level of available solar energy (intermediate):
                - control level is the reduced value (value / number of commands * maximum energy from the sun) of the gene № + 1
                - if the intrinsic level is less than the control - the value of the gene № + 2 is added to the command counter
                - if the intrinsic level is greater than or equal to the control - the value of the gene № + 3 is added to the command counter
            - 13 - determine the level of available minerals (intermediate):
                - the control level is the reduced value (value / number of commands * maximum energy from minerals) of the gene № + 1
                - if the intrinsic level is less than the control - the value of the gene № + 2 is added to the command counter
                - if the intrinsic level is greater than or equal to the control - the value of the gene № + 3 is added to the command counter
            - 14 - determine the level of available energy from accumulated minerals (intermediate):
                - the control level is the reduced value (value / number of commands * maximum energy from minerals) of the gene № + 1
                - if the intrinsic level is less than the control - the value of the gene № + 2 is added to the command counter
                - if the intrinsic level is greater than or equal to the control - the value of the gene № + 3 is added to the command counter
            - all other numbers - unconditional transition commands (add their value to the command counter)
        - cell color - three positive integers (RGB)
        - energy transfer balance - the difference between transmitted and received energy for all time
        - past energy transfer - the sign value of the last transferred energy (-1, 0 or 1), the value of which fades with time
    - mutations:
        - mutation - change of a random gene to a random value
        - separate chances of mutation for parent and daughter cells during budding, and the chance of random mutation
    - reproduction:
        - reproduction occurs by budding of a daughter cell
        - energy is divided in half
        - the daughter cell inherits the color and direction of the mother
        - there is a complete copying of the genome into a daughter cell
        - there is a separate chance of mutation in budding, the same for both cells
        - the daughter cell is budded in the first direction of the first free cell, which is checked clockwise starting from the direction of the mother cell (optional)
        - the cell must multiply if its energy is not less than the maximum energy (optional)
        - a cell can multiply if its energy is not less than the minimum energy for division, and the budding command is executed
        - if the cell needs to bud but does not have space for it - it becomes organic with energy conservation (optional)
- interface:
    - setting simulation parameters, genome, energy level, direction and position of the stem cell, as well as simulation grains in JSON format
    - pause rendering, pause simulation, set the number of ticks per drawing, step-by-step mode
    - switching on / off the image of photosynthesis and mineral nutrition zones
    - change the simulation image mode:
        - diet (greener - more energy from photosynthesis, blue - from minerals, red - from food, gray - organic)
        - energy level mode (redder - more energy, yellower - less, gray - organic)
        - energy transfer balance mode (more brown - more energy, more turquoise - more transmission, gray - organic)
        - mode of past energy transfer (more brown - recently received, more turquoise - recently transferred, gray - organic)
        - age regime (redder - older, more turquoise - younger, gray - organic)
    - display simulation statistics: frames per second, steps per second, step, day, year, season, number of days before gamma-ray flash, live / dead cells, number of buds, amount of energy consumption from photosynthesis / minerals / food
    - change the simulation parameters and enable / disable individual cell instructions on the go
    - select the cell with the cursor:
        - view all its properties
        - memorizing the properties of the cell, if it has disappeared, with the possibility of reset
        - highlight it in black

</details>
</br>

### Formations
<details>
<summary>Click to see formations</summary>
<br/>

You can discover different formations yourself.
Here are formations that I have found:
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
- cellSize4: chess, spinners, colonies, worms, singletons
- mineralEnergy04: worms, spinners, diagonal sticks, jumpers, colonies
- mineralEnergy08: wind, singletons, colonies
- mineralEnergy10: sticks, singletons, jumpers, spinners, colonies

</details>
</br>

## Building
<details>
<summary>Click to see how to build it</summary>
<br/>

The prototype of the application is written in Processing so if you don't want
to build the application yourself you can download Processing 3.5+ and run it.
However, Processing version does not contain all the available rules,
configuration features and GUI.

The final version of the application is written in C++ using:
1. Dear ImGui
2. OpenGL 4.6 (GLFW and GLAD)
3. Qt 5

Therefore, before building, you need to satisfy all the dependencies
by taking these steps:
1. Install Qt using your system package manager
or [official installer](https://www.qt.io/download)
and add it to PATH if on Windows.
2. Download [GLFW](https://www.glfw.org/download), build it,
then place static library archive in ```c++/lib/```
and include directories in ```c++/include/```.
3. Download [GLAD](https://glad.dav1d.de/),
build it as a static or shared library and place it in ```c++/lib/```.
Alternatively you can place source file in ```c++/src/glad/``` and
then place include directory in ```c++/include/```.
4. Clone [Dear ImGui](https://github.com/ocornut/imgui) repository,
then copy all the source and header files from the repository's root
and also ```imgui_impl_glfw*``` with ```imgui_impl_opengl3*``` from ```backends/```
to ```c++/src/dear_imgui/```.

To build the application run these commands from ```c++/``` directory:
```bash
cmake . -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```
You will have ```./build/bin/Release/cellarium```
or ```./build/bin/Release/cellarium.exe``` built.

</details>
</br>

## Using
<details>
<summary>Click to see how to use it</summary>
<br/>

You can download released binaries and presets
on [release page](https://github.com/maldenol/cellarium/releases)
or build the source code yourself (see Building section).

Run the application with ```-h``` parameter to see help information.  
Use ```-c``` parameter to see controls help information.  
Use ```-g``` parameter to generate default configuration so you can edit it and use.  
Run the application with configuration file specified to start simulation.

</details>
</br>

## License
cellarium is licensed under the MIT License, see [LICENSE](LICENSE) for more information.
</br>
