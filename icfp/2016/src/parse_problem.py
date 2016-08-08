import os
import sys
from pprint import pprint
from fractions import Fraction
import matplotlib.pyplot as plt

if __name__ == "__main__":
    file_name = sys.argv[1]
    print('file name: ', file_name)
    axes = plt.gca()
    axes.set_xlim([0, 1])
    axes.set_ylim([0, 1])

    os.path.isfile(file_name)

    polygons = []

    # Parse input file
    lines = [line.strip() for line in open(file_name).readlines()]
    number_of_polygons = int(lines[0])
    polygons = []

    n = 1
    for pn in range(number_of_polygons):
        print('polygon #', pn)
        number_of_vertices = int(lines[n])
        print('\tnumber_of_vertices', number_of_vertices)
        vertices = []
        vert_start_line = n + 1
        for i in range(vert_start_line, vert_start_line + number_of_vertices):
            x, y = lines[i].split(',')
            vertices.append((
                Fraction(x.strip()),
                Fraction(y.strip()),
            ))
            n += 1
        print('\tvertices', vertices, len(vertices))
        n += 1
        number_of_segments = int(lines[n])
        print('\tnumber_of_segments', number_of_segments)
        segments = []
        segm_start_line = n + 1
        for i in range(segm_start_line, segm_start_line + number_of_segments):
            print(i, lines[i])
            points = lines[i].split(' ')
            segment = []
            for point in points:
                x, y = point.split(',')
                segment.append((
                    Fraction(x.strip()),
                    Fraction(y.strip()),
                ))
            segments.append(segment)
            n += 1
        polygons.append({
            'segments': segments,
            'vertices': vertices,
        })
        n += 1

    print('number of polygons: ', number_of_polygons)
    pprint(polygons)

    for polygon in polygons:
        for segment in polygon['segments']:
            for i in range(len(segment) - 1):
                x1, y1 = [float(segment[i][0]), float(segment[i][1])]
                x2, y2 = [float(segment[i + 1][0]), float(segment[i + 1][1])]
                plt.plot([x1, x2], [y1, y2], 'k-', lw=2)
        vs = polygon['vertices']
        for v in vs:
            plt.plot([float(v[0])], [float(v[1])], 'ro')

    print('showing plot...')
    plt.savefig('plots/{}.png'.format(file_name.replace('/', '_')))
