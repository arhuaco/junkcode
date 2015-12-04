' Run a decision tree. Python 2.7! Legacy code... '

import pydot
import csv
import pprint

def get_label():
    pass

def resolve_variable(node_or_edge, inputs):
    ' Get the value of a variable of the model. '
    var_name = node_or_edge.get_label()[1:-1]
    return var_name

def eval_tree(tree, inputs, root_node_name):
    root_val  = resolve_variable(tree.get_node(root_node_name)[0])
    for edge in [e for e in tree.get_edges() if e.get_source() == root_node_name]:
        #print edge.get_destination()
        #print edge.get_label()
        print root_val + edge.get_label()[1:-1]
    #print edge
    #print root, edge

def process_inputs(data_file, graph_file, root_node_name):
    ' Read a decision graph and a CSV file with values for tree evaluation. '
    # First, read the variables open(data_file) as csv_file:
    with open(data_file) as csv_file:
        reader = csv.DictReader(csv_file, delimiter=';')
        inputs  = {}
        for row in reader:
            for column, value in row.iteritems():
                inputs.setdefault(column, []).append(value)
        #return
        #tree = pydot.graph_from_dot_data(open(graph_file).read())
        # Evaluate the graph
        #result = eval_tree(tree=tree, inputs=inputs, root_node_name=root_node_name)

def main():
    process_inputs(data_file='total.csv',
                   graph_file='ModeloB.dot',
                   root_node_name='N0')

if __name__ == "__main__":
    # execute only if run as a script.
    main()
