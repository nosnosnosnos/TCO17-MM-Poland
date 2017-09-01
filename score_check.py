import json
import io
import sys
import os
import re
from tqdm import tqdm
import subprocess
import time
from operator import itemgetter, attrgetter

args = sys.argv
sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8')

# --- regexp pattern
p_score = re.compile(r"Score = ([0-9]+.[0-9]+).+")
p_S = re.compile(r": S = ([0-9]+)")
# ----

TEST_SIZE = [50, 100, 200, 300, 400, 500]
data = {}
data_num = {}

if __name__ == "__main__":
    score_sum = 0
    START = 5000
    ITER = 100

    for i in tqdm(range(START, START + ITER, 1)):
        S = TEST_SIZE[i % len(TEST_SIZE)]
        out = subprocess.check_output(
            'java KnightsAttacksVis -exec "./a.out" -seed ' + str(i) + " -size " + str(S), shell=True, timeout=15)
        out = out.decode('utf-8')

        match = re.search(p_score, out)
        score = float(match.group(1))
        score_sum += score

        match = re.search(p_S, out)
        S = int(match.group(1))
        if not S in data:
            data[S] = 0
            data_num[S] = 0
        data[S] += score
        data_num[S] += 1

    keys = sorted(data.keys())

    # Sの出力
    f = open('S.txt', 'w')
    f_s = open('score.txt', 'w')
    for key in keys:
        sco = data[key] / data_num[key]
        f.write(str(key) + "\n")
        f_s.write(str(sco) + "\n")

    f.write("sum\n")
    f_s.write(str(score_sum) + "\n")
    f.close()
    f_s.close()

    elapsed_time = time.time() - start
    print("elapsed_time:{0}".format(elapsed_time) + "[sec]")
    print(score_sum)
