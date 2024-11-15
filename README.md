# Genetic-Algorithms
This Repo is for training on genetic algorithms.

## Task Assignment:

#### This Task was to create a simple genetic algorithm for best allocation of tasks to a single core of the available 2 cores.

### Input format:

First line: Number of test cases (must be at least 1) 

For each test case: 
• Max time limit
• Number of tasks to be allocated

For each each task: 
• task execution time

### The Used technique.

1- Binary, one-dimensional chromosome.
2- To evaluate the chromosome, assign genes with 1 to core 1 and genes with 0 to core 2 where time taken will be the max between the two.
3- Roulette wheel selection.
4- One-point crossover.
5- Flip bit mutation.
6- Elitism replacement.


#### How infeasible solutions are handled?
what about ignoring it and it will be removed in the next generation !...
Yeah, this what happend here and I am explaining it right now.
Infeasible solutions means that the assignment exceeds the time limit required be the test case and it is handled in the code
by sorting the array of chromosomes in descending order so that in selection, crossover ,and mutation, these bad solutions are selected
to be modified. After that, combine all solutions together to select the best solutions till now form the current not selected parents and the offsprings.
