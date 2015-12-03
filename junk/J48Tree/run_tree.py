' Run a decision tree. Python 2.7! Legacy code... '

import pydot
import csv
import pprint

def main():
    with open('total.csv') as csv_file:
        reader = csv.DictReader(csv_file, delimiter=';')
        result = {}
        for row in reader:
            for column, value in row.iteritems():
                result.setdefault(column, []).append(value)
    pprint.pprint(result)

if __name__ == "__main__":
    # execute only if run as a script.
    main()
