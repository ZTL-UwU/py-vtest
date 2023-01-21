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

print("Start Making data for {}.".format(name))
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

err_cnt = 0

for i in range(1, sub_tasks + 1):
    print("Making subtask #{}".format(i))

    for j in range(1, task_cnt[i - 1] + 1):
        tot_id += 1
	if platform.system() == "Windows":
            mk_path = "./mk_{}.exe".format(name)
            std_path = "./std_{}.exe".format(name)
	else:
            mk_path = "./mk_{}".format(name)
            std_path = "./std_{}".format(name)
        in_path = "{}/.input.tmp".format(folder)
        out_path = "{}/{}.{}.{}.in".format(folder, name, i, j)
        ans_path = "{}/{}.{}.{}.out".format(folder, name, i, j)

        check_file(mk_path, "[ERR] Executable file {} not found.".format(mk_path))
        check_file(std_path, "[ERR] Executable file {} not found.".format(std_path))

        input_tmp = open(in_path, "w")
        input_tmp.write(str(i) + " " + str(j))
        input_tmp.close()

        mk_time, mk_code = run(mk_path, in_path, out_path)
        std_time, std_code = run(std_path, out_path, ans_path)

        if mk_code != 0:
            print(
                "****[ERR] {} Runtime Error on making test-case #{}.{}".format(
                    mk_path, i, j
                )
            )

            err_cnt += 1
        elif std_code != 0:
            print(
                "****[ERR] {} Runtime Error on running test-case #{}.{}".format(
                    std_path, i, j
                )
            )

            err_cnt += 1
        else:
            if std_time > max_time:
                max_time = std_time
                max_case = (i, j)

            print(
                "    [{:>3}%] Made case #{}.{}: ({}ms)".format(
                    round((tot_id / tot_tasks) * 100),
                    i,
                    j,
                    round(std_time * 1000, 2),
                )
            )

tot_end_time = time.time()

if os.path.exists("{}/.input.tmp".format(folder)):
    os.remove("{}/.input.tmp".format(folder))

print("\nSummary:")
print(
    "    Slowest case: #{}.{} ({}ms)".format(
        max_case[0], max_case[1], round(max_time * 1000, 2)
    )
)

if err_cnt != 0:
    print(
        "****[ERR] {} errors occurred. [{}%]".format(
            err_cnt, (err_cnt * 100 // tot_tasks)
        )
    )
