' Submission system '

import glob
import subprocess
import logging
import time

import solver

# Where the problems to solve are.
PROBLEM_DIR = '../contest_problems_per_id/'
# Where the solutions are stored.
SOLUTION_DIR = './solutions/'

def problem_id_from_glob(pattern):
    ' Get problem ids from glob '
    # Note that this will return the .metadata files
    ret = []
    for path in glob.glob(pattern):
        ret.append(path.split('/')[-1])
    return ret

def get_unsolved_problems():
    ' Get the probelms that we need to solve '
    # TODO: Don't return problems already submitted.
    problems = set(problem_id_from_glob(PROBLEM_DIR + '*'))
    for solution_hash in problem_id_from_glob(SOLUTION_DIR + '*'):
        if solution_hash in problems:
            logging.info('Problem %s already solved', solution_hash)
            problems.remove(solution_hash)
    return problems

def parse_metadata_file(metadata_file):
    ' Parse a metadata file '
    key_value = {}
    with open(metadata_file) as input_file:
        line = input_file.readline()
        while len(line):
            line = line.strip()
            key, value = line.split(': ', 1)
            key_value[key] = value
            line = input_file.readline()
    return key_value

def submit_with_curl(problem_id, solution_path):
    ' Submit a solution given its problem id and solution spec '

    output = subprocess.check_output(['curl',
                                      '--compressed', '-L',
                                      '-H', 'Expect:',
                                      '-H', 'X-API-Key: 111-xxx',
                                      '-F',
                                      'problem_id={}'.format(problem_id),
                                      '-F', 'solution_spec=@{}'.format(solution_path),
                                      'http://2016sv.icfpcontest.org/api/solution/submit'])
    return output.decode('utf-8')

def solve_pending_problems():
    ' Solve the pending problems. '
    for problem_id in get_unsolved_problems():
        # Compute input and output paths
        problem_spec_file = PROBLEM_DIR + problem_id
        solution_path = SOLUTION_DIR + problem_id
        solution_metadata_path = SOLUTION_DIR + problem_id + '.metadata'

        # Solve the problem and store it in the solutions dir.
        engine = solver.BinarySolver(problem_spec_file)
        engine.process(solution_path) # Writes the file.

        # Try to submit with curl.
        try:
            #time_start = time.time()
            output = submit_with_curl(problem_id, solution_path)
            #elapsed_time = time.time() - time_start
            time.sleep(1.05)

            with open(solution_metadata_path, 'w') as output_file:
                print(output, file=output_file)
                print(output)
            # Delete.
        except Exception as exc:
            logging.error('Something failed :-/ Exception: %s', str(exc))

def main():
    ' Our main function '
    logging.basicConfig(level=logging.INFO)
    solve_pending_problems()

if __name__ == "__main__":
    main()
