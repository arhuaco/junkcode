' Run a decision tree. Python 2.7! Legacy code... '

# TODO(nel): This is slow. Track what is happening. I guess that
# tree.get_edges() is being called too much. But it does not make sense.

import pydot
import csv
import pprint

def resolve_variable(node_or_edge, inputs):
    ' Get the value of a variable of the model. '
    var_name = node_or_edge.get_label()[1:-1]
    if var_name in inputs:
        return True, inputs[var_name]
    return False, var_name

def eval_tree(tree, inputs, root_node_name):
    ' Evaluate the tree. '
    root_node = tree.get_node(root_node_name)[0]
    lookup_result, root_val = resolve_variable(root_node, inputs)
    if not lookup_result:
        return root_val
    for edge in [e for e in tree.get_edges() if e.get_source() == root_node_name]:
        # Evaluate the condition. If true, descend in this branch if True.
        if eval(root_val + edge.get_label()[1:-1]):
            return eval_tree(tree, inputs, edge.get_destination())
    assert False

def process_inputs(data_file, graph_file, root_node_name, csv_row_id):
    ' Read a decision graph and a CSV file with values for tree evaluation. '
    # The tree.
    tree = pydot.graph_from_dot_data(open(graph_file).read())
    # Read the variables.
    result = {}
    with open(data_file) as csv_file:
        for variables in csv.DictReader(csv_file, delimiter=';'):
            result[variables[csv_row_id]] = eval_tree(tree=tree, inputs=variables, root_node_name=root_node_name)
    return result


def main():
    ' Our main function. '
    result =process_inputs(data_file='total.csv',
                           graph_file='ModeloB.dot',
                          root_node_name='N0',
                          csv_row_id='Fecha')
    pprint.pprint(result)

if __name__ == "__main__":
    # execute only if run as a script.
    main()
