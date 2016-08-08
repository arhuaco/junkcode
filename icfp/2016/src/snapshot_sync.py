import os
import time
import requests
from json import JSONDecodeError
from collections import defaultdict

HEADERS = {
    'X-API-Key': '111-xxx'
}

ids_per_problem = defaultdict(set)

def fetch_snapshots():
    time.sleep(1.5)
    response = requests.get('http://2016sv.icfpcontest.org/api/snapshot/list', headers=HEADERS)

    if response.status_code != 200:
        raise Exception('Snapshots request failed: status: {}; body: {}'.format(
            response.status_code,
            response.content,
        ))

    return response.json()


def fetch_blob(blob_hash):
    time.sleep(1.1)
    response = requests.get('http://2016sv.icfpcontest.org/api/blob/{}'.format(blob_hash), headers=HEADERS)

    if response.status_code != 200:
        raise Exception('Blob fetch request failed: status: {}; body: {}'.format(
            response.status_code,
            response.content,
        ))

    try:
        return response.json()
    except JSONDecodeError:
        return response.content.decode('utf-8')


def fetch_problem(prb):
    global ids_per_problem

    ids_per_problem[prb['problem_spec_hash']].add(int(prb['problem_id']))

    problem_path = 'contest_problems/{problem_spec_hash}.txt'.format(**prb)
    metadata_path = 'contest_problems/{problem_spec_hash}.metadata'.format(**prb)
    if os.access(problem_path, os.R_OK):
        print("CACHED")
        return

    problem = fetch_blob(prb['problem_spec_hash'])
    with open(problem_path, 'w') as f:
        f.write(problem)

    with open(metadata_path, 'w') as f:
        for k in sorted(prb.keys()):
            f.write('{}: {}\n'.format(k, prb[k]))


def sync_snapshot(sn):
    print('SNAPSHOT: {snapshot_hash}'.format(**sn))
    snapshot = fetch_blob(sn['snapshot_hash'])
    for i, prb in enumerate(snapshot['problems'], 1):
        print('PROBLEM {} of {}: {}'.format(i, len(snapshot['problems']), prb['problem_spec_hash']))
        fetch_problem(prb)


def sync_problems():
    snapshots = fetch_snapshots()
    for s in reversed(snapshots['snapshots']):
        sync_snapshot(s)
    for problem_hash in ids_per_problem:
        file_content = "\n".join(map(str, sorted(ids_per_problem[problem_hash]))) + "\n"
        with open('contest_problems/{}.problem_ids'.format(problem_hash), 'w') as ids_file:
            ids_file.write(file_content)
