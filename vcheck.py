import subprocess
import time
import sys
import os


def run(name: str, in_path, out_path):
    in_file = open(in_path, mode="r")
    out_file = open(out_path, mode="w")

    start_time = time.time()
    code = subprocess.call(name, stdin=in_file, stdout=out_file)
    end_time = time.time()

    in_file.close()
    out_file.close()
    return (end_time - start_time, code)


def cmp_file(file_path1, file_path2):
    file1 = open(file_path1, mode="r")
    file2 = open(file_path2, mode="r")

    in_1 = "#"
    in_2 = "#"
    while not len(in_1) == 0 and not len(in_1) == 0:
        in_1 = file1.readline()
        in_2 = file2.readline()

        while len(in_1.split()) == 0 and not len(in_1) == 0:
            in_1 = file1.readline()

        while len(in_2.split()) == 0 and not len(in_2) == 0:
            in_2 = file2.readline()

        if in_1.split() != in_2.split():
            file1.close()
            file2.close()
            return False

    file1.close()
    file2.close()
    return True


def check_res(code, in_path, ans_path):
    if not code == 0:
        return ("Runtime Error (return {})".format(code), 2)

    if not cmp_file(in_path, ans_path):
        return ("Wrong Answer", 1)

    return ("Answer Correct", 0)


def check_file(file_path: str, err_msg: str):
    if not os.path.exists(file_path):
        print(err_msg, file=sys.stderr, flush=True)
        exit(1)
    return


task_cnt = []

vconf = open("vaccept.conf", "r")

name, sub_tasks = vconf.readline().split()
sub_tasks = int(sub_tasks)
tot_tasks = 0

for i in range(0, sub_tasks):
    num = int(vconf.readline())
    task_cnt.append(num)
    tot_tasks += num

max_time = -1
max_case = (0, 0)

tot_start_time = time.time()
tot_id = 0

ac = 0
wa = 0
re = 0

for i in range(0, sub_tasks):
    print("Start checking subtask #{}".format(i))

    for j in range(0, task_cnt[i]):
        tot_id += 1
        exe_path = "./std_{}".format(name)
        in_path = "{}.{}.{}.in".format(name, i + 1, j + 1)
        out_path = ".output.tmp"
        ans_path = "{}.{}.{}.out".format(name, i + 1, j + 1)

        check_file(exe_path, "[ERR] Executable file {} not found.".format(exe_path))
        check_file(in_path, "[ERR] Input file {} not found.".format(in_path))

        elapsed_time, code = run(exe_path, in_path, out_path)
        if elapsed_time > max_time:
            max_time = elapsed_time
            max_case = (i, j)

        res_str, res_code = check_res(code, out_path, ans_path)

        if res_code == 0:
            ac += 1
        elif res_code == 1:
            wa += 1
        elif res_code == 2:
            re += 1

        print(
            "    Case #{}: {} ({}ms) [{}%]".format(
                tot_id,
                res_str,
                round(elapsed_time * 1000, 2),
                round((tot_id / tot_tasks) * 100),
            )
        )

tot_end_time = time.time()

print("\nSummary:")
print("    Total time: {}ms".format(round((tot_end_time - tot_start_time) * 1000, 2)))
print("    Slowest case: #{}.{} ({}ms)".format(max_case[0] + 1, max_case[1] + 1, round(max_time * 1000, 2)))
print("--------------------------------")
print("    AC: {} [{}%]".format(ac, ac * 100 // tot_tasks))
print("    WA: {} [{}%]".format(wa, wa * 100 // tot_tasks))
print("    RE: {} [{}%]".format(re, re * 100 // tot_tasks))
