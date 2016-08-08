from fractions import Fraction
import sys

from fracvecutils import *


class Node:
    def __init__(self, x, y):
        self.x = x
        self.y = y

    def __str__(self):
        return '{},{}'.format(str(self.x), str(self.y))

    def __eq__(self, other):
        return self.x == other.x and self.y == other.y


class Solution:
    def __init__(self, node_list=None, facet_list=None, destination_list=None):
        self.node_list = node_list if node_list is not None else [Node(0, 0),
                                                                  Node(0, 1),
                                                                  Node(1, 1),
                                                                  Node(1, 0)]
        self.facet_list = facet_list if facet_list is not None else [[0, 1, 2, 3]]
        self.destination_list = destination_list if destination_list is not None else list(self.node_list)

    def fold(self, edge_node, node_1, node_2):
        if edge_node not in self.node_list:
            return
        if node_1 not in self.node_list:
            self.node_list.append(node_1)
        if node_2 not in self.node_list:
            self.node_list.append(node_2)
        edge_index = self.node_list.index(edge_node)
        # only str returns (1,1) when folding 0,0, to 1,1.
        # Direct access to fractions returns unusual fractions i don't get
        q = mirror_p_vw(point(node_1.x, node_1.y),
                        point(node_2.x, node_2.y),
                        point(edge_node.x, edge_node.y)).to_frac_str()
        x, y = q.split(',')
        destination_node = Node(Fraction(x), Fraction(y))
        self.destination_list[edge_index] = destination_node

    def __str__(self):
        s = "{}\n".format(str(len(self.node_list)))
        for node in self.node_list:
            s += "{}\n".format(str(node))
        s += "{}\n".format(str(len(self.facet_list)))
        for facet in self.facet_list:
            facet_str = ' '.join(str(n) for n in facet)
            s += "{} {}\n".format(str(len(facet)), facet_str)
        for destination in self.destination_list:
            s += "{}\n".format(str(destination))
        return s


def get_solution_obj(solution_string):
    solution = solution_string.split('\n')
    node_list = list()
    facet_list = list()
    destination_list = list()
    nodes_number = int(solution[0])
    solution = solution[1:]
    for point in solution[:nodes_number]:
        x, y = point.split(',')
        node = Node(Fraction(x), Fraction(y))
        node_list.append(node)
    solution = solution[nodes_number:]
    facets_number = int(solution[0])
    solution = solution[1:]
    for facet in solution[:facets_number]:
        tmp_list = facet.split(' ')
        facet_nodes_num = int(tmp_list[0])
        tmp_list = tmp_list[1:]
        for n in range(facet_nodes_num):
            tmp_list[n] = int(tmp_list[n])
        facet_list.append(tmp_list)
    solution = solution[facets_number:]
    for destination in solution[:nodes_number]:
        x, y = destination.split(',')
        node = Node(Fraction(x), Fraction(y))
        destination_list.append(node)
    return Solution(node_list, facet_list, destination_list)


def get_solution_obj_from_file(filename):
    with open(filename) as f:
        solution_string = f.read()
    return get_solution_obj(solution_string)

if __name__ == "__main__":
    print(locals()[sys.argv[1]](*sys.argv[2:]))
