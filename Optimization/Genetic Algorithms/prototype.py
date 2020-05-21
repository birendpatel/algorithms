"""
Author: Biren Patel
Description: A prototype for a genetic algorithm with a small memory footprint.
This is just a rough sketch, to be rewritten in C for use in microcontrollers.
Also, this one constrains chromosomes to maximum length of 64 bits for more
agressive optimizations. Also avoids numpy arrays.
"""

from numpy.random import default_rng

class GeneticAlgorithm():
    def __init__(self, obj_func, popsize, chr_len, debug=0):
        self.debug = debug

        #function to **MINIMIZE**
        self.obj_func = obj_func

        #demand popsize multiple of 8 to work efficiently with the PCG word size
        if popsize % 8 != 0 or popsize <= 0:
            raise ValueError("population size must be positive multiple of 8")
        else:
            self.popsize = popsize

        #also demand multiple of 8 for chromosome length
        if chr_len %8 != 0 or chr_len <= 0:
            raise ValueError("chromosome length must be positive multiple of 8")
        if chr_len > 64:
            raise ValueError("chromosome length exceeds PRNG word size")
        else:
            self.chr_len = chr_len

        #handy mask over chromosome length to be used through the class.
        self.chr_mask = 2**self.chr_len - 1

        #this allows us to access the unsigned 64 bit integer from a PCG.
        #algo works with the PCG word size, hence everything in multiples of 8.
        self.prng = default_rng().bit_generator.random_raw
        self.rand = 0

        ## ALGORITHM STEPS
        #initialize a population of random individuals.
        #not going to be picky about data types since this is just a sketch.
        self.population = []
        self.initialize_population()

        #calc fitness of initial population
        self.fitness = []
        self.calculate_fitness()

    def initialize_population(self):
        """
        pcg word is AND'd with chromosome mask, result is a population member.
        rshift the word by chromosome length and repeat. if no capacity, run
        PCG again and repeat. no need to dump extra bits at end, there wont be
        any since we work in multiples of 8.
        """
        for i in range(self.popsize):
            #fetch next int from PCG if bit stream is empty
            if self.rand == 0:
                self.rand = self.prng()

            #create a population member then dump the used bits
            self.population.append(self.rand & self.chr_mask)
            self.rand >>= self.chr_len

        if self.debug:
            print("---------- POPULATION INIT ----------\n")
            print(self.population, end = "\n\n")

    def calculate_fitness(self):
        for i in range(self.popsize):
            self.fitness.append(self.obj_func(self.population[i]))

        if self.debug:
            print("---------- FITNESS INIT ----------\n")
            print(self.fitness, end = "\n\n")

    def select(self):
        pass

    def crossover(self):
        pass

    def mutate(self):
        pass

decoder = (5.12 - -5.12)/(2**8 -1)
min = -5.12
def sphere(chromosome):
    """
    sample objective function, a toy problem taking a 16 bit chromosome and
    using two 8 bit sections as binary encodings of real numbers. The
    function is the famous 3D sphere, a convex minimization with a gradient.
    """
    param_1 = (chromosome & 255) * decoder + min
    param_2 = (chromosome >> 8) * decoder + min

    return (param_1 ** 2) + (param_2 ** 2)

#debugging
if __name__ == "__main__":
    genetic_algorithm = GeneticAlgorithm(sphere, 8,16, debug=1)
