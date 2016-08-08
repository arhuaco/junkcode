from binary_solver import Frac

class Vertex:
    def __init__(self, x_coord, y_coord):
        self.x_coord = x_coord
        self.y_coord = y_coord

    def translate(self, x_off=0, y_off=0):
        self.x_coord = self.x_coord + x_off
        self.y_coord = self.y_coord + y_off

    def __str__(self):
        return '{x_coord},{y_coord}'.format(**vars(self))

    def __repr__(self):
        return '{x_coord},{y_coord}'.format(**vars(self))


def parse_solution(filename):
    with open(filename) as sol_file:
        lines = [line.strip() for line in sol_file.readlines()]

    total_vertices = int(lines.pop(0))
    vertices = []
    for vertex_num in range(total_vertices):
        vertices.append(Vertex(*map(Frac, lines.pop(0).split(','))))

    facets = []
    for facet_num in range(int(lines.pop(0))):
        facets.append(list(map(int, lines.pop(0).split(' ')))[1:])

    pos_of_vertices = []
    for vertex_num in range(len(vertices)):
        pos_of_vertices.append(Vertex(*map(Frac, lines.pop(0).split(','))))

    return Solution(
        vertices=vertices,
        facets=facets,
        pos_of_vertices=pos_of_vertices,
    )

class Solution:
    def __init__(self, vertices=None, facets=None, pos_of_vertices=None):
        self.vertices = vertices or []

        self.facets = facets or []

        self.pos_of_vertices = pos_of_vertices or []

    def serialize(self):
        lines = []

        lines.append(str(len(self.vertices)))
        lines = lines + list(map(str, self.vertices))

        lines.append(str(len(self.facets)))
        lines = lines + [
            ' '.join([str(len(facet))] + list(map(str, facet)))
            for facet in self.facets
        ]

        lines = lines + list(map(str, self.pos_of_vertices))

        return '\n'.join(lines) + '\n'

    def translate(self, x_off=0, y_off=0):
        for vertex in self.pos_of_vertices:
            vertex.translate(x_off, y_off)
