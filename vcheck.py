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

check_file("vtest.conf", "[ERR] Configure file 'vtest.conf' not found.")
vconf = open("vtest.conf", "r")

name, sub_tasks = vconf.readline().split()
folder = vconf.readline().strip()
if (not os.path.exists(folder)):
    os.makedirs(folder)

print("Start Checking solution for {}.".format(name))
print()

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

tot_time = 0

for i in range(1, sub_tasks + 1):
    print("Start checking subtask #{}".format(i))

    for j in range(1, task_cnt[i - 1] + 1):
        tot_id += 1
        exe_path = "./run_{}".format(name)
        in_path = "{}/{}.{}.{}.in".format(folder, name, i, j)
        out_path = "{}/.output.tmp".format(folder)
        ans_path = "{}/{}.{}.{}.out".format(folder, name, i, j)

        check_file(exe_path, "[ERR] Executable file {} not found.".format(exe_path))
        check_file(in_path, "[ERR] Input file {} not found.".format(in_path))

        elapsed_time, code = run(exe_path, in_path, out_path)
        if elapsed_time > max_time:
            max_time = elapsed_time
            max_case = (i, j)
        tot_time += elapsed_time

        res_str, res_code = check_res(code, out_path, ans_path)

        if res_code == 0:
            ac += 1
        elif res_code == 1:
            wa += 1
        elif res_code == 2:
            re += 1

        print(
            "    [{:>3}%] Case #{}.{}: {:^14} ({}ms)".format(
                round((tot_id / tot_tasks) * 100),
                i,
                j,
                res_str,
                round(elapsed_time * 1000, 2),
            )
        )

tot_end_time = time.time()

if os.path.exists("{}/.output.tmp".format(folder)):
    os.remove("{}/.output.tmp".format(folder))

print("\nSummary:", end="")
if wa != 0:
    print(" WA", end="")
if re != 0:
    print(" RE", end="")
if wa == 0 and re == 0:
    print(" AC", end="")

print()
print("    Total time:   {}ms".format(round(tot_time * 1000, 2)))
print(
    "    Slowest case: #{}.{} ({}ms)".format(
        max_case[0], max_case[1], round(max_time * 1000, 2)
    )
)
print("--------------------------------")
print("    AC: {:>3} [{:>3}%]".format(ac, ac * 100 // tot_tasks))
print("    WA: {:>3} [{:>3}%]".format(wa, wa * 100 // tot_tasks))
print("    RE: {:>3} [{:>3}%]".format(re, re * 100 // tot_tasks))
