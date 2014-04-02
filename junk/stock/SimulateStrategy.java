import java.util.Random;

/**
 * @author Nelson Castillo -- nelsoneci@gmail.com
 */

class SimulateStrategy
{
    public static final double INITIAL_CAPITAL = 10000;


    /**
    * Simulate the life of a trading account with the given parameters.
    *
    * This is an oversimplification! But it helps you understand the probabilities of the game.
    *
    * You should call this function many times and average the results.
    * @param maxSteps Maximum number of steps of the simulation.
    * @param startingCapital Starting amount of (say) USD dollars.
    * @param maxCapital After this much we consider ourselves rich and we do
                        not care about having more money.
    * @param winningProbability Is a number in the range [0, 1].
                                The losing probability will be 1.0 - winningProbability.
    * @param winningPercent How much of the account will we earn if we do win? Range [0, 100]
                            The losing percentage will be 100 - winningPercent.
    * @param losingPercent How much of the account will we lose if we do lose? Range [0, 100]
                            The losing percentage will be 100 - winningPercent.
    */

    static double Simulate (Random rand, int maxSteps,
                            double startingCapital, double maxCapital, double winningProbability,
                            double winningPercent, double losingPercent)
    {
        double capital = startingCapital;

        for (int i = 0; i < maxSteps; ++i) {
            if (capital <= 0.0) {
                return 0.0;  // We lost all the money.
            }
            if (capital >= maxCapital) {
                return maxCapital;  // We are rich, baby. We don't need more money.
            }

            if (rand.nextFloat() < winningProbability) {
                capital *= 1.0 + winningPercent / 100.0;
            } else {
                capital *= 1.0 - losingPercent / 100.0;
            }
        }

        return capital;
    }

    public static void main (String args[]) throws java.io.IOException
    {
        Random rand = new Random();
        int ntimes = 30000;
        for (double winningProb = 0.3; winningProb <= 0.7; winningProb += 0.01) {
            double sum = 0.0;
            for (int i = 0; i < ntimes; ++i) {
                sum += Simulate(rand, 1000, 10000, 1000000, winningProb, 1.0, 2.0);
            }
            System.out.printf("%f %.2f\n", winningProb, sum / ntimes);
        }
    }
}
