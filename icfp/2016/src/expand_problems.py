' Pipe this to bash to get a dir with problems by id '

import glob
import json
import logging
import os

# Where the problems to solve are.
PROBLEMS_DIR = './contest_problems/'

# Where we are going to expand them.
EXPANDED_PROBLEMS = './contest_problems_per_id/'

def get_hashes_from_glob(pattern):
    ' Get the hashes for a given glob '
    ret = []
    for file_name in glob.iglob(pattern):
        # Get the hash for the file name
        problem_hash = file_name.split('/')[-1].split('.')[0].strip()
        ret.append(problem_hash)
    return ret

def main():
    ' Our main function '
    for solution_hash in get_hashes_from_glob(PROBLEMS_DIR + '*.txt'):
        problem_file = PROBLEMS_DIR + solution_hash + '.txt'
        index_file = PROBLEMS_DIR + solution_hash + '.problem_ids'
        if not os.path.exists(index_file):
            logging.error('file %s was not created', index_file)
            continue
        with open(index_file) as input_file:
            try:
                problems_ids = []
                line = input_file.readline()
                while len(line) > 0:
                    number = int(line.strip())
                    problems_ids.append(number)
                    line = input_file.readline()
                for problem_id in problems_ids:
                    print('cp {} {}'.format(problem_file, EXPANDED_PROBLEMS + str(problem_id)))
            except:
                pass

if __name__ == "__main__":
    main()
