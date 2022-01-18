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
    with open(file_path1, mode="r") as file1:
        with open(file_path2, mode="r") as file2:
            while True:
                a = file1.readline()
                b = file2.readline()
                
                if (len(a) == 0 and len(b) != 0) or (len(a) == 0 and len(b) != 0):
                    return False
                
                if len(a) == 0 and len(b) == 0:
                    break
                
                a = a.split()
                b = b.split()
                if a != b:
                    return False

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


check_file("vhack.conf", "[ERR] Configure file 'vhack.conf' not found.")
vconf = open("vhack.conf", "r")

name, type = vconf.readline().split()

print("Start HACKING data for {}.".format(name))
print()

atp = 0
while True:
    atp += 1

    std_path = "./std_{}".format(name)
    run_path = "./run_{}".format(name)
    mk_path = "./mk_{}".format(name)
    in_path = ".input.tmp"
    out_path = "hack.in"
    ans_path = "hack.out"
    fatout_path = "hack.fat"


    check_file(mk_path, "[ERR] Executable file {} not found.".format(mk_path))
    check_file(std_path, "[ERR] Executable file {} not found.".format(std_path))
    check_file(std_path, "[ERR] Executable file {} not found.".format(run_path))

    input_tmp = open(in_path, "w")
    input_tmp.write(str(type))
    input_tmp.close()


    mk_time, mk_code = run(mk_path, in_path, out_path)

    if mk_code != 0:
        print(
            "****[ERR] {} Runtime Error on making test-case of attemption#{}. (abort)".format(
                mk_path, atp
            )
        )
        exit(1)

    std_time, std_code = run(std_path, out_path, ans_path)

    if std_code != 0:
        print(
            "****[ERR] {} Runtime Error on making test-case of attemption#{}. (abort)".format(
                std_path, atp
            )
        )
        exit(1)

    fat_time, fat_code = run(run_path, out_path, fatout_path)

    if fat_code != 0:
        print(
            "Attemption#{}: Hack Success[{}ms --- Runtime Error]".format(
                atp, round(std_time, 2)
            )
        )
        break
    
    if not cmp_file(ans_path, fatout_path):
        print(
            "Attemption#{}: Hack Success[{}ms --- {}ms]".format(
                atp, round(std_time, 2), round(fat_time, 2)
            )
        )
        break

    print(
        "Attemption#{}: Hack Failed[{}ms --- {}ms]".format(
            atp, round(std_time, 2), round(fat_time, 2)
        )
    )