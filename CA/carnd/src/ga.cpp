#include <fitness.h>

#include <stdio.h>
#include <stdlib.h>
#include <ga/std_stream.h>
#include <ga/ga.h>

#define cout STD_COUT

// set GENERATE_STATISTICS to zero to avoid generating statistics

#define GENERATE_STATISTICS 1

inline unsigned int
gen2rule (GAGenome& g)
{
  GA1DBinaryStringGenome & genome = (GA1DBinaryStringGenome &)g;

  unsigned int rule = 0;

  for(int i=0; i<genome.length(); i++) // genome.length == 32
    rule |=  genome.gene(i) << (31 - i);

  return rule;
}

float
MinEntropy(GAGenome& g)
{
  GA1DBinaryStringGenome & genome = (GA1DBinaryStringGenome &)g;

  return fitness(gen2rule(genome));
}

int
main(int argc, char *argv[])
{

  // See if we've been given a seed to use (for testing purposes).
  // When you specify a random seed, the evolution will be exactly
  // the same each time you use that seed number.

  unsigned int seed = 0;

  for(int ii=1; ii<argc; ii++) {
    if(strcmp(argv[ii++],"seed") == 0) {
      seed = atoi(argv[ii]);
    }
  }

  // our genome is 32 bits long

  GA1DBinaryStringGenome genome(32, MinEntropy);

  GASimpleGA ga(genome);

  ga.selectScores(GAStatistics::AllScores);
  ga.recordDiversity(gaTrue);
  ga.scoreFilename("bog.dat");
  ga.flushFrequency(1);
  ga.scoreFrequency(1);
  ga.parameters(argc, argv);
  ga.evolve(seed);


  // These parameters are unusual for genetic algorithms.
  // The mutation is way too high.
  // But only with this mutation good rules were found.
  // Aren't we too close to a random search?

  ga.populationSize(100);
  ga.pCrossover(1);
  ga.pMutation(0.5);
  ga.nGenerations(200);

  while(!ga.done()) {
    ga.step();
  }

  // Let's do a fine tunning second pass

  ga.pCrossover(0);
  ga.pMutation(0.001);
  ga.nGenerations(600);

  while(!ga.done()) {
    ga.step();
  }

//  cout << "\nthe statistics for the run are:\n" << ga.statistics();
//  cout << "\nthe parameters for the run are:\n" << ga.parameters();
//  cout << "\nthe ga generated:\n";

  cout << ga.statistics().bestIndividual() << "\n";
  cout.flush();

  return EXIT_SUCCESS;
}
