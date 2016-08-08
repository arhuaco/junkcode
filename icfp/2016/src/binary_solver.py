''' Try a simple solution that only folds in half '''

# Usage : python3 input output

import math
from fractions import Fraction
import logging
import sys
import  geom_utils

class Frac(Fraction):
    ' Wrapper for printing '

    def __str__(self):
        if self.denominator == 1:
            return str(self.numerator)
        return '{}/{}'.format(self.numerator, self.denominator)

    def __repr__(self):
        if self.denominator == 1:
            return str(self.numerator)
        return '{}/{}'.format(self.numerator, self.denominator)


def determine_square(file_name):
    ' Square for problem '
    polygon_points = []
    with open(file_name) as input_file:
        n_poly = int(input_file.readline())
        for _ in range(n_poly):
            poly_points = int(input_file.readline())
            for _ in range(poly_points):
                point_x, point_y = input_file.readline().strip().split(',')
                point = (Frac(point_x), Frac(point_y))
                polygon_points.append(point)
    min_x = math.inf
    min_y = math.inf
    max_x = -math.inf
    max_y = -math.inf
    for x_coord, y_coord in geom_utils.get_envelope(polygon_points):
        min_x = min(min_x, x_coord)
        min_y = min(min_y, y_coord)
        max_x = max(max_x, x_coord)
        max_y = max(max_y, y_coord)
    #logging.info('The base should be %s %s', min_x, min_x)
    return (min_x, min_y, max_x - min_x, max_y - min_y)

def compute_divisions_of_line(obj_size):
    ' Compute the binary divisions that need to be made '
    n_divisions = 0
    size = Frac(1)
    while size / 2 > obj_size:
        size /= 2
        n_divisions += 1
    return n_divisions

def compute_points_after_dividing(n_divisions):
    ' Get the points once you know how much you have to divide '
    start = 0
    end = Frac(1)
    ret = [0]
    delta = Frac(1, 2 ** n_divisions)
    while start < end:
        start += delta
        ret.append(start)
    return ret

class BinarySolver:
    ' Binary solveer '
    def __init__(self, file_name):
        self.base_x, self.base_y, \
        self.initial_square_x_size, self.initial_square_y_size = determine_square(file_name)

    def process(self, file_name):
        ' Compute a solution '
        #logging.info('square of size %s %s', self.initial_square_x_size, self.initial_square_y_size)
        y_divisions = compute_divisions_of_line(self.initial_square_y_size)
        x_divisions = compute_divisions_of_line(self.initial_square_x_size)
        # logging.info('Y divisions %s', y_divisions)
        computed_y = compute_points_after_dividing(y_divisions)
        #logging.info('Computed Xs %s', computed_y)
        #logging.info('X divisions %s', x_divisions)
        computed_x = compute_points_after_dividing(x_divisions)
        #logging.info('Computed Ys %s', computed_x)

        point_matrix = [[0 for x_c in range(len(computed_x))] for y_c in range(len(computed_y))]
        point_index = [[0 for x_c in range(len(computed_x))] for y_c in range(len(computed_y))]

        point_index_count = 0
        for y_c in range(len(computed_y)):
            for x_c in range(len(computed_x)):
                point_matrix[y_c][x_c] = (computed_x[x_c], computed_y[y_c])
                point_index[y_c][x_c] = point_index_count
                point_index_count += 1


        with open(file_name, 'w') as output_file:
            # Print the number of vertices.
            print(len(computed_y) * len(computed_x), file=output_file)

            # Print the vertices, in the same order they were filled out.
            for y_c in range(len(computed_y)):
                for x_c in range(len(computed_x)):
                    x_point, y_point = point_matrix[y_c][x_c]
                    print('{},{}'.format(x_point, y_point), file=output_file)

            # Now print the facets. Let's compute them first because we don't have
            # a heuristic to know how many of them are there yet.
            facets = []
            for y_c in range(len(computed_y) - 1): # -1!
                for x_c in range(len(computed_x) - 1): # -1!
                    facet_points = []
                    for y_delta, x_delta in [(0, 0), (1, 0), (1, 1), (0, 1)]:
                        facet_points.append(str(point_index[y_c + y_delta][x_c + x_delta]))
                    facets.append('4 {}'.format(' '.join(facet_points)))
            print(len(facets), file=output_file)
            for facet in facets:
                print(facet, file=output_file)

            # Now the destination points. There is a heuristic: If a coordinate is odd, it will
            # end up being at the second position (say point at y=1) or (point at x=1).

            for y_c in range(len(computed_y)):
                for x_c in range(len(computed_x)):
                    x_point, y_point = point_matrix[y_c % 2][x_c % 2]
                    print('{},{}'.format(self.base_x + x_point, self.base_y + y_point), file=output_file)

def main():
    ' Our main function '
    logging.basicConfig(level=logging.WARNING)
    solver = BinarySolver(sys.argv[1])
    solver.process(sys.argv[2])

if __name__ == "__main__":
    main()
