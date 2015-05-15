<h1>Genetic Algorithm Solving TSP</h1>

A legacy C++ genetic algorithm I wrote back in 2010 to solve the <a href="http://en.wikipedia.org/wiki/Travelling_salesman_problem" target="_blank">Traveling Salesman Problem</a>.

<h2>Usage</h2>

  1. Start up and compile the solution in Visual Studio.
  2. Navigate to the Release folder.
  3. Start up cmd, then start the algorithm executable with parameters in the following order:
  <p><code>TSP_Genetic_Algorithm.exe [Number of cities] [Number of generations] [Number of chromosomes] [Visualisation type] [Mutation type] [Cross Over type] [Selection type]</code></p>

- <code>[Visualisation type]:</code> 1, 2 or 3 (random, circle or fixed route plotting)
- <code>[Mutation type]:</code> 1 (inverse mutation)
- <code>[Cross Over type]:</code> 1, 2 or 3 (single point, position based or order)
- <code>[Selection type]:</code> 1 or 2 (elitism or roulette wheel)

<h3>Random Positions on 2D Plane</h3>
Here is an example of running the algorithm with random positions on a 2D plane:
<p><code>TSP_Genetic_Algorithm.exe 1000 1000 1000 1 1 1 1</code></p>

<h3>Fixed Positions on 2D Plane</h3>
In order to run the algorithm with fixed positions on a 2D plane, you need to set <code>[Number of cities]</code> to the number of fixed positions, <code>[Visualisation type]</code> to 3 and then provide the <code>[x,y]</code> positions as last parameters to the algorithm. 

Here is an example of running the algorithm with 7 fixed positions on a 2D plane:
<p><code>TSP_Genetic_Algorithm.exe 7 1000 1000 3 1 1 1 400,333 323,444 897,333 473,222 434,999 423,222 787,191</code></p>
