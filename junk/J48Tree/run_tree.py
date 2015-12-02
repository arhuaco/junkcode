' Run a decision tree. Python 2.7! Legacy code... '

import pydot
import csv

def main():
    tree = pydot.graph_from_dot_data(open('ModeloB.dot').read())
    with open('total.csv') as csv_file:
         for row in csv.reader(csv_file):
             print ', '.join(row)

if __name__ == "__main__":
    # execute only if run as a script.
    main()
