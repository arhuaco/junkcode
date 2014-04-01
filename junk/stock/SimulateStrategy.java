import java.util.Random;

/**
 * @author Nelson Castillo.
 */

class SimulateStrategy
{
    /**
    * Simulate the life of an account with the given parameters - Rough simplification.
    * You should call this function many times and average the results.
    * @param maxSteps Maximum number of steps of the simulation.
    * @param startingCapital Starting amount of (say) USD dollars.
    * @param maxCapital After this much we consider ourselves rich and we do
                        not care about having more money.
    * @param winningProbability Is a number in the range [0, 1].
                                The losing probability will be 1.0 - winningProbability.
    * @param winningPercent How much of the account will we win if we do win? Range [0, 100]
                            The losing percentage will be 100 - winningPercent.
    */

    static double Simulate (int maxSteps,
                            double startingCapital, double maxCapital, double winningProbability,
                            double winningPercent)
    {
        int i = 0;

        double capital = startingCapital;

        while (i < maxSteps) {
            if (capital <= 0.0) {
                return 0.0;  // We lost all the money.
            }
            if (capital >= maxCapital) {
                return maxCapital;  // We are rich, baby. We don't need more money.
            }
        }

        return 0;
    }

    public static void main (String args[]) throws java.io.IOException
    {
    }
}
