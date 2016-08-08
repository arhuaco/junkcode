# Team fotocopia-al-150

# Members

- Andrés Velasquez
- Javier Rojas
- Luis Carvajal
- Nelson Castillo
- Vera Mazhuga

We had fun. Our math guy (Luis Carvajal) had a few issues at home so this
made things difficult for us. Also, only one team member could work full time.

# The task

The contest was about doing Origami. A very interesting problem: [Task in PDF](doc/TaskDescription.pdf).

# How did we do?

Our [rank at freeze time was 70](http://2016sv.icfpcontest.org/leaderboard). A race about not being last :-P So we tried to send as many solutions as we could with a simple algorithm.
The algorithm only folds in halves to get a rectangle that encircles the silhouette.

![Simple strategy](https://raw.githubusercontent.com/arhuaco/junkcode/master/icfp/2016/src/simple_binary_fold.png "Just fold in halves")

We didn't get a geisha drone and it would have been very useful.
The math behind this problem is very interesting.

Unfortunately, there was a 1-line bug in an [utility program](src/prune_rate_limit.py) for the *smart* submitter system that made us submit solutions for only about half of the problems
and not for them all. **Bummer!** Nelson's fault.

## Simple algorithm performance

On average 21.64% for each problem. Not too bad for such a simple approach.

In the following list we have : score, number of problems, percent of submissions.

- >= 0.00 1459 100.0%
- >= 0.10 1184 81.15%
- >= 0.20 821 56.27%
- >= 0.30 371 25.420%
- >= 0.40 88 6.031%
- >= 0.50 22 1.51%
- >= 0.60 8 0.54%
- >= 0.70 5 0.34%
- >= 0.80 3 0.20%
- >= 0.90 3 0.20%
- >= 1.00 2 0.13%

# Code

[](other_file.md)

- [Trivial binary solver] (src/binary_solver.py)
- [Problem creator](src/grid_generator_affine.py)
- [Problem downloader] (src/snapshot_sync.py)
- [Solution submitter] (src/solve_pending.py)

# Final words

And that was it. Thanks to the organizers. We need to study more math :-)
The submission system was impressive. Kudos to the organizers for this.
