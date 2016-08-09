# Team fotocopia-al-150

# Members

- Andrés Velasquez
- Javier Rojas
- Luis Carvajal
- Nelson Castillo
- Vera Mazhuga

The problem this year was fascinating. In a few words "Reverse engineer origami".
We had a lot of fun. Our math guy (Luis Carvajal) had a few issues at home with his kids so this
made things a bit difficult for the rest of us. Also, only one team member could work full time and the rest didn't have a lot of availability.
Next year they all should take vacations :-)

# The task

The contest was about reverse engineering Origami. A very interesting problem: [Task in PDF](doc/TaskDescription.pdf). 

# How did we end up doing?

Our [rank at freeze time was 70](http://2016sv.icfpcontest.org/leaderboard). Now we have to wait for the final scores. 

The scores were granted to solutions of posted problems, calculated based on the similarity of the area of the generated shape intersected with the expected shape, and 
also to the problems submitted by the teams. A successful submitted problem has small area and not many solutions by other teams.

So, we created  a grid generator that generates rotated origami grids in angles that follow [pitagorean triples](https://en.wikipedia.org/wiki/Pythagorean_triple). The grids were also translated so that all coordinates were more difficult to discern. 

As a workaround while some members of the team started workign on a general utility to "fold points, segments and facets" that we called "geisha", we started a race about not being last :-P by trying to send as many solutions as we could with a simple algorithm.

This last algorithm only folds in halves to get a rectangle that when translated encircles the silhouette.

![Simple strategy](https://raw.githubusercontent.com/arhuaco/junkcode/master/icfp/2016/src/simple_binary_fold.png "Just fold in halves")

In the end we were very close but we didn't get a geisha drone. It would have been very useful. Part of the geisha that we completed: 

- Utilities to fold points and segments
- Utilities to find a set of facets from a set of segments. This problem was not trivial. We found a graphical approach using opencv. The idea is that by painting the segments, finding the facets becomes order of the size of the image and does not grow exponentially with the segments. By using a large enough image we can project all the segments and then find the regions by using flood fill with different colors per facet. We think this was a good solution to the problem of finding the [facets](img/facets.jpg). 

In order to score some points, we wrote a [utility program](src/prune_rate_limit.py) for the *smart* submitter system. Unfortunately, there was a 1-line bug in it that made us submit solutions for only about half of the problems and not for them all. **Bummer!** Nelson's fault. It had to do with cleaning solutions that couldn't been sent because rate limits of the contest's API.

## Simple algorithm performance

On average a score of 21.64% for each problem. Not too bad for such a [simple approach](src/binary_solver.py).

In the following list we have : score, number of solutions sent, percent of submissions.

- >= 0.0 1459 100.0%
- >= 0.1 1184 81.15%
- >= 0.2 821 56.27%
- >= 0.3 371 25.420%
- >= 0.4 88 6.031%
- >= 0.5 22 1.51%
- >= 0.6 8 0.54%
- >= 0.7 5 0.34%
- >= 0.8 3 0.20%
- >= 0.9 3 0.20%
- >= 1.0 2 0.13%

# Code

[](other_file.md)

- [Trivial binary solver] (src/binary_solver.py)
- [Problem creator](src/grid_generator_affine.py)
- [Problem downloader] (src/snapshot_sync.py)
- [Solution submitter] (src/solve_pending.py)
- [Facet finder] (src/facet_finder.py)
- [Rational utils] (src/fracutils.py)
- [Vector utils] (src/fracvecutils.py)

# Final words

And that was it. Thanks to the organizers. We need to study more math :-)
The submission system was impressive. Kudos to the organizers for this.
