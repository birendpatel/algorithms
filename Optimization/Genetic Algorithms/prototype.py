"""
Author: Biren Patel
Description: A prototype for a genetic algorithm with a small memory footprint.
This is just a rough sketch, to be rewritten in C for use in microcontrollers.
Also constrains chromosomes to maximum length of 64 bits for more agressive
optimizations. The goal is to eventually create a tiny but powerful GA which
requires no more than 50 bytes of SRAM, excluding the fitness function.
"""

from numpy.random import default_rng
from copy import copy
from time import process_time_ns

class GeneticAlgorithm():
    def __init__(self, obj_func, popsize, chr_len, debug=0):
        self.debug = debug
        self.gen_counter = 1

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
        self.calculate_population_fitness()

        self.children = []

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
            print("---------- POPULATION {} ----------\n".format(self.gen_counter))
            print(self.population, end = "\n\n")

    def calculate_population_fitness(self):
        """
        fitness of each pop member into list
        """
        self.fitness = []

        for i in range(self.popsize):
            self.fitness.append(self.obj_func(self.population[i]))

        if self.debug and self.gen_counter == 1:
            print("---------- FITNESS {} ----------\n".format(self.gen_counter))
            print(self.fitness, end = "\n\n")

    def __rshift__(self, gens):
        for i in range(gens):
            self.evolve()

    def evolve(self):
        """
        evolve forward one generation
        """
        #for popsize
        self.children = []

        for i in range(self.popsize):
            #select two parents from current gen
            p1, p2 = self.select()

            #cross parents
            c1, c2 = self.crossover(p1, p2)

            #mutate results
            c1 = self.mutate(c1)
            c2 = self.mutate(c2)

            #decision on who goes into the pop
            f_val_p1 = self.obj_func(p1)
            f_val_p2 = self.obj_func(p2)
            f_val_c1 = self.obj_func(c1)
            f_val_c2 = self.obj_func(c2)

            if f_val_c1 < f_val_p1:
                self.children.append(c1)
            else:
                self.children.append(p1)

            #if f_val_c2 < f_val_p2:
            #    self.children.append(c2)
            #else:
            #    self.children.append(p2)

        self.population = copy(self.children)
        self.gen_counter += 1
        self.calculate_population_fitness()

        if self.debug:
            print("---------- POPULATION {} ----------\n".format(self.gen_counter))
            print(self.population, end = "\n\n")
            print("---------- FITNESS {} ----------\n".format(self.gen_counter))
            print(self.fitness, end = "\n\n")

    def select(self): #550 ns
        self.rand = self.prng()

        #obviously biased since not power of 2. ITS JUST A PROTOTYPE!
        idx_1 = (self.rand & 0xFFFFFFFF) % self.popsize
        idx_2 = (self.rand >> 32) % self.popsize

        return (self.population[idx_1], self.population[idx_2])

    def crossover(self, p1, p2): #1 us
        """
        1/chr_len probability of no crossover
        """
        crossover_point = self.prng() % self.chr_len

        crossover_mask_R = self.chr_mask >> crossover_point
        crossover_mask_L = self.chr_mask ^ crossover_mask_R

        c1 = (p1 & crossover_mask_R) | (p2 & crossover_mask_L)
        c2 = (p2 & crossover_mask_R) | (p1 & crossover_mask_L)

        #if self.debug:
        #    print(crossover_point)
        #    print(bin(p1)[2:].zfill(self.chr_len))
        #    print(bin(p2)[2:].zfill(self.chr_len))
        #    print(bin(c1)[2:].zfill(self.chr_len))
        #    print(bin(c2)[2:].zfill(self.chr_len))

        return (c1, c2)

    def mutate(self, child): #2.5 us
        """
        bit operations emulate a binomial distribution over each chromosome
        with PDF parmeters n=chr_len, p=.03125.

        The probability is simulated using AND bit operations on sections of
        the PCG word so that 1/32 achieves the probability of a bit set.

        1/64 would be less wasteful of the bit stream but it creates a ~1.5%
        probability of mutation. This is more wasteful of the mutation operator
        since most strings will not induce a change in the child chromosome.

        so I find 1/32 to be a decent heuristic tradeoff of bit stream resources
        against the computational expense of running a mutation, and against the
        exploration versus exploitation of the search space.
        """
        self.rand = self.prng()

        #of the five mutator genes we need, set the first one
        mutator = self.rand & self.chr_mask
        self.rand >>= self.chr_len

        for i in range(4):
            #refresh stream if empty
            if self.rand == 0:
                self.rand = self.prng()

            #create a mutator gene and immediately AND with the current gene
            mutator &= (self.rand & self.chr_mask)

            #dump used bits
            self.rand >>= self.chr_len

        #reset rand since no guarantee that the bit stream is empty (failsafe)
        self.rand = 0

        #if self.debug:
        #    print(bin(child)[2:].zfill(self.chr_len))
        #    print(bin(mutator)[2:].zfill(self.chr_len))
        #    print(bin(child ^ mutator)[2:].zfill(self.chr_len))
        #    print("\n\n")

        #xor mutator with child to mutate each bit with probably 1/32
        return child ^ mutator

decoder = (5.12 - -5.12)/(2**8 -1)
min_val = -5.12
def sphere(chromosome):
    """
    sample objective function, a toy problem taking a 16 bit chromosome and
    using two 8 bit sections as binary encodings of real numbers. The
    function is the famous 3D sphere, a convex minimization with a gradient.
    """
    param_1 = (chromosome & 255) * decoder + min_val
    param_2 = (chromosome >> 8) * decoder + min_val

    return (param_1 ** 2) + (param_2 ** 2)

#debugging
if __name__ == "__main__":
    genetic_algorithm = GeneticAlgorithm(sphere, 8,16, debug=0)

    print("best: {}".format(min(genetic_algorithm.fitness)))

    start = process_time_ns()
    genetic_algorithm >> 10000
    end = process_time_ns()

    print("best: {}".format(min(genetic_algorithm.fitness)))
    print("time: {}".format((end-start)/(10**9)))
