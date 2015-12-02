' Run a decision tree. Python 2.7! Legacy code... '

import pydot

def main():
    tree = pydot.graph_from_dot_data(open('ModeloB.dot').read())

if __name__ == "__main__":
    # execute only if run as a script.
    main()
