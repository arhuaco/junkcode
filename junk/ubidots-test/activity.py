from __future__ import division

from apiclient  import ApiClient

import os
import sys
import time

# 120 is the amount of seconds to wait before each update.
# This allow us to use the free Ubidots plan.
INTERVAL_SECONDS = 120

def get_idle_time_ms():
    with os.popen('xprintidle') as xpi:
        return int(xpi.read())

def main():
    api_key = sys.argv[1]
    api = ApiClient(api_key)
    try:
        activity_variable = api.get_variable('542268a276254253d58c9406')
    except Exception, e:
        print >> sys.stderr, 'Exception:', str(e)
        print >> sys.stderr, 'Could not open variable'
        return 1
    while True:
        busy_sum = 0
        for _ in range(INTERVAL_SECONDS):
            busy_sum += 1.0 - min(1.0, get_idle_time_ms() / 1000)
            time.sleep(1)
        busy_percent = busy_sum / INTERVAL_SECONDS
        print >> sys.stderr, busy_percent
        try:
            activity_variable.save_value({'value': busy_percent})
            print >> sys.stderr, 'Saved variable'
        except Exception, e:
            print >> sys.stderr, 'Could not save variable. Exception:', str(e)
    return 0 # Never reached.

if __name__ == "__main__":
    sys.exit(main())
