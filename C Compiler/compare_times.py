#!/usr/bin/env python3

from collections import OrderedDict
import csv
import math
import sys

def read_times(filename):
    times = OrderedDict()
    with open(filename) as file:
        for row in csv.DictReader(file):
            times[row['test_name']] = {
                'mean_log': float(row['mean_log_duration']),
                'variance_log': float(row['variance_log_duration'])
            }
    return times

if __name__ == '__main__':
    (_, reference_file, new_file, speedup_file) = sys.argv
    reference_times = read_times(reference_file)
    new_times = read_times(new_file)
    with open(speedup_file) as f:
        expected_speedup = float(f.read().strip())

    ((test_name, new_time),) = new_times.items()
    time = reference_times[test_name]
    mean_speedup = math.expm1(time['mean_log'] - new_time['mean_log'])
    speedup_percent = abs(mean_speedup * 100)
    speedup_direction = 'fast' if mean_speedup > 0 else 'slow'
    variance_log_sum = time['variance_log'] + new_time['variance_log']
    variance_percent = math.expm1(math.sqrt(variance_log_sum)) * 100
    print(f'{test_name} ran {speedup_percent}% {speedup_direction}er than reference solution (+/- {variance_percent}%)')

    print(f'Expected speedup: {expected_speedup * 100}% faster')
    if mean_speedup < expected_speedup:
        print('Make it faster!')
        sys.exit(1)
    else:
        print("That's fast!")
