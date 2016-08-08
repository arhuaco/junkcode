' Make sure we send solutions again when we need to retry due to API limits '

import glob
import json
import logging
import os

# Where the problems to solve are.
SOLUTION_DIR = './solutions/'

def problem_id_from_glob(pattern):
    ' Get problem ids from glob '
    # Note that this will return the .metadata files
    ret = []
    for path in glob.glob(pattern):
        ret.append(path.split('/')[-1])
    return ret

def main():
    ' Our main function '
    for solution_hash in problem_id_from_glob(SOLUTION_DIR + '*'):
        if solution_hash.find('metadata') != -1:
            continue
        metadata_file_name = SOLUTION_DIR + solution_hash + '.metadata'
        solution_file_name = SOLUTION_DIR + solution_hash
        if not os.path.exists(metadata_file_name):
            logging.error('file %s was not created, removing solution', metadata_file_name)
            os.remove(solution_file_name)
            continue
        with open(metadata_file_name) as input_file:
            try:
                json_text = input_file.read()
                response = json.loads(json_text)
            except:
                logging.error('Invalid json for file %s: %s', metadata_file_name, json_text)
                continue
            if not response['ok']:
                if 'Rate' in response['error']:
                    try:
                        os.remove(metadata_file_name)
                        os.remove(solution_file_name)
                    except:
                        pass
                elif 'Can not submit' in response['error']:
                    pass
                else:
                    print(response)

if __name__ == "__main__":
    main()
