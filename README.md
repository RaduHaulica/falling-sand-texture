## Falling Sand Simulation

![Falling sand simulation cover image](https://github.com/RaduHaulica/falling-sand-texture/blob/df7e19194b066492f3deac41114d233493c4a5ed/falling%20sand%20texture/media/falling%20sand.gif)

Individually simulated particles of sand, water, rock and lava. Each particle has to decide where to go every 1/60th of a second.

Sand falls until there's no place to go, water moves around until it's level, rock ignores gravity, and lava melts everything it touches.

Sand displaces water, pushing it upward, lava is cooled by water - becoming rock and vaporising the water - and destroys sand it touches, turning it into smoke. Smoke moves up and is only decorative, it's not involved in any particle computations.

## Technical details

Particles are stored in a grid, making it very easy to check neighbors and to edit a particle at a specific screen position. In actuallity it's a single row of uint8_t, taking into account particle size.

Every 1/60th of a second each particle tries to make a series of moves to valid locations based on its surroundings, as if affected by gravity. Particles are updated starting from the bottom of the grid, going up, following the natural direction of movement. Particle identity checks are done based on color (actual values are within a certain range of the base color value).

The grid is divided into vertical slices based on the configured parallelization factor, and each section is handled by a different thread. You can switch between singlethreaded and multithreaded computation to compare performance.

Sand tries to move South, if the grid slot is already taken, then it tests the SE and SW positions for a space to fall into, and if unable to find an empty space, it stays put. Water also tests the E and W directions for space to move into. Lava flows like sand and also bursts through stuff like the Kool-Aid man.

I introduced some variation in water and lava movement to make things look more interesting, then i introduced some variation in particle color to make things look even more interesting.

Rendering is done by creating a full-screen texture file based on the values of the grid (taking into account the particle render size factor), and then drawing it to the screen in a single draw function call, avoiding any render overhead.

## More technical history

Framerate is relatively steady now even with a large number of particles (around 200k is where things gradually start slowing down). This took a few tries to get right. After figuring out the initial movement formulas and adding particle speeds bigger than the number of updates per second, i started focusing on scale. Performance was starting to decline when a few thousand particles were active on the screen, completely crashing when getting into the tens of thousands.

Naturally, multithreading was the go-to solution in the beginning, but as it turned out, the best optimization results came from changing the way the particles are stored and changing how adjacent spaces are checked.

I started out thinking that representing pixels as a grid would make sense since that's the way they look on the screen and would greatly improve lookup speed and adjacency checks. As i experimented with different approaches (simple C array, containers, particle objects) i kept hitting the same dead end, performance was horrible once over 10k particles were displayed on screen.

After trying out more ways to store the data and perform the checks i built a profiler and gradually eliminated the bottlenecks. I tried using a particle buffer where i would build the new particle grid after each particle was updated, and then overwrite the initial grid once the whole deal was done, i tried marking updated particles so that neighbours won't influence each other twice, i tried keeping track of particles that are "active" in order to reduce the number of updates, and things started looking better and better after each iteration, until i hit a roadblock that seemed tricky to work around.

Creating the actual vertices that are displayed on the screen during every update took a very long time, so i tried to create the render objects once and then just edit them during the update, but that also resulted in a lot of operations trying to locate the particles and their neighbours and still affected performance to an undesirable degree.

After experimenting with different ways to store and process the particles and their rendered avatars, i opted for a simpler approach that greatly improved the performance of both updates and rendering - having particles be represented by their colour (a quartet of chars representing RGB and alpha) and their grid position be their position in a one dimensional array.

This meant that rendering just meant translating that one big array into a texture (conveniently using the same format of a RGB + alpha char quartet for every pixel) that would be ready to draw instantly, and that adjacency would be very simple to calculate based on the fixed position of each pixel in the array, without having to iterate through any random order lists of objects, and without having to order objects every time a particle was changed.

By this time most of the kinks, pitfalls and surprises along the way have been discovered and ironed out. Particle updates were multithreaded, there was no overlap or weird behavior on boundary areas between the screen sections each thread was handling, and the program could handle a 512x512 grid without taking performance hits. But a 512x512 grid of pixels doesn't look too good, so i wanted to allow particles to be rendered bigger, regardless of their game world size.

After i implemented scaling i thought it would be a good time to stop and implement more particle types so i added water that also moves laterally so it won't form a pyramid but instead flow out until it's level. That worked without a hitch, so i made sand swap places with water whenever it would be directly above it.

After that i added rock that's not affected by gravity to try out more complex movement and sand/water mingling and lava that dissolves everything it touches. It only felt fair to have some sort of smoke or vapor be created whenever lava touches water and evaporates it, or to slowly heat sand until it melts it instead of doing it instantly, so i thought long and hard about adding a layer for gas and another one for temperature, or adding density into the movement equation, but then decided it's beyond the scope of this project.

## How to use

Click and drag to create particles. Right click to delete them.

Hold S to create sand, W to create water, R to create rock, and L to create Lava.

![Falling sand simulation help text](https://github.com/RaduHaulica/falling-sand-texture/blob/dc1a7d1118a6a4e9418a749818f688ece611cfa3/falling%20sand%20texture/media/falling%20sand%20instructions.png)
