# py-Vtest

![[LICENSE](https://github.com/ZTL-UwU/py-vtest/blob/main/LICENSE)](https://img.shields.io/github/license/ZTL-UwU/py-vtest?style=flat-square)
![Code Size](https://img.shields.io/github/languages/code-size/ZTL-UwU/py-vtest?style=flat-square)

An OI test-data maker & std test tool.

## Mode

1. **vmake**: Automatically generate test-data with user-provided standard solution and Maker (described below).
2. **vcheck**: Execute a solution on set of test-data like an OnlineJudge does.
**WARNING: No sandbox protection, don't run any untrusted code!**

## Maker

A Maker is an executable that generates the input of a single test case.

**How to write a Maker?**

1. Read the subtask ID (starting from 1), a single integer from `stdin`.
2. Output the input data to `stdout`.

## Usage

1. Create `vtest.conf`.

    **The format of `vtest.conf`**:

    ```plaintext
    <name> <subtask count>
    <data path>
    <case count for subtask #1>
    <case count for subtask #2>
    ...
    <case count for subtask #n>
    ```

    **Example**:

    ```plaintext
    AplusB 5
    data
    5
    15
    20
    20
    40
    ```

2. The file structure will be like this

    ```plaintext
    .
    ├─ <data path> // Auto Generated
    │  ├─ <name>.1.1.in
    │  ├─ <name>.1.1.out
    │  ├─ ...
    │  ├─ <name>.<subtask count>.<n>.in
    │  └─ <name>.<subtask count>.<n>.out
    │
    ├─ mk_<name>  // Compile yourself
    ├─ std_<name> // Compile yourself
    ├─ run_<name> // Compile yourself
    ├─ vmake.py   // Downloaded
    └─ vcheck.py  // Downloaded
    ```

## Example

Here is a example of generating test-data of the _A + B problem_ and testing a solution of it.

1. Create an empty folder.
2. Create `vtest.conf` with the following content:

    ```plaintext
    AplusB 2
    data
    6
    4
    ```

3. Download `vmake.py` and `vcheck.py`.

    You can use the following commands if you prefer CLI or simply click `Download Zip` in the project repository page and extract `vmake.py` and `vcheck.py`:

    ```bash
    wget https://github.com/ZTL-UwU/py-vtest/raw/main/vmake.py
    wget https://github.com/ZTL-UwU/py-vtest/raw/main/vcheck.py
    ```

4. Create a standard solutoion as an executable named `std_AplusB`.

    For example, the following code is a C++ version of a standard solution of _A + B problem_, compile it into `std_AplusB`:

    ```cpp
    // std_AplusB.cpp
    #include <iostream>

    int main() {
        long long a, b;
        std::cin >> a >> b;
        std::cout << a + b;
        return 0;
    }
    ```

    Compile commands:

    ```bash
    g++ std_AplusB.cpp -o std_AplusB
    ```

5. Create an executable named `mk_AplusB` which is a Maker (described above).

    For example, the following is a C++ version of the test-data Maker, compile it into `mk_AplusB`:

    ```cpp
    // mk_AplusB.cpp
    #include <iostream>
    #include <random>

    int main() {
        int subtask_id;
        std::cin >> subtask_id;

        if (subtask_id == 1) {
            std::mt19937 rng(std::random_device{}());
            std::cout << rng() << " " << rng();
        }

        if (subtask_id == 2) {
            // In this subtask, we will generate larger inputs
	    // which can hack solutions without using long long.
            std::mt19937_64 rng(std::random_device{}());
            std::cout << rng() << " " << rng();
        }

        return 0;
    }
    ```

    Compile commands:

    ```bash
    g++ mk_AplusB.cpp -o mk_AplusB
    ```

6. Run `vmake.py`.

    You can use the following command or simply double-click on `vmake.py`:

    ```bash
    python3 vmake.py
    ```

    The output is similar to the following:

    ```plaintext
    Start Making data for AplusB.

    Making subtask #1
        [ 10%] Made case #1.1: (9.0ms)
        [ 20%] Made case #1.2: (2.17ms)
        [ 30%] Made case #1.3: (5.08ms)
        [ 40%] Made case #1.4: (2.53ms)
        [ 50%] Made case #1.5: (4.01ms)
        [ 60%] Made case #1.6: (3.81ms)
    Making subtask #2
        [ 70%] Made case #2.1: (1.99ms)
        [ 80%] Made case #2.2: (3.06ms)
        [ 90%] Made case #2.3: (2.04ms)
        [100%] Made case #2.4: (3.4ms)

    Summary:
        Slowest case: #1.1 (9.0ms)
    ```

    Now you can see the generated data in the `data` folder.
    
    ```
    .
    ├─ data
    |  ├─ AplusB.1.1.in
    |  ├─ AplusB.1.1.out
    |  ├─ ...
    |  ├─ AplusB.2.4.in
    |  └─ AplusB.2.4.out
    ```

7. Lets try another solution without using `long long` (who cannot pass the test).

    This is a C++ version of a wrong solution, compile it into `run_AplusB`:

    ```cpp
    // AplusB_wrong.cpp
    #include <iostream>

    int main() {
        int a, b;
        std::cin >> a >> b;
        std::cout << a + b;
        return 0;
    }
    ```

    Compile commands:

    ```bash
    g++ AplusB_wrong.cpp -o run_AplusB
    ```

8. Run `vcheck.py`

    You can use the following command or simply double-click on `vcheck.py`:

    ```bash
    python3 vcheck.py
    ```

    The output is similar to the following:

    ```plaintext
    Start checking subtask #1
        [ 10%] Case #1.1: Answer Correct (2.56ms)
        [ 20%] Case #1.2: Answer Correct (2.33ms)
        [ 30%] Case #1.3: Answer Correct (2.6ms)
        [ 40%] Case #1.4: Answer Correct (7.18ms)
        [ 50%] Case #1.5: Answer Correct (2.22ms)
        [ 60%] Case #1.6: Answer Correct (2.24ms)
    Start checking subtask #2
        [ 70%] Case #2.1:  Wrong Answer  (3.17ms)
        [ 80%] Case #2.2:  Wrong Answer  (2.85ms)
        [ 90%] Case #2.3:  Wrong Answer  (2.28ms)
        [100%] Case #2.4:  Wrong Answer  (2.76ms)

    Summary: WA
        Total time: 30.2ms
        Slowest case: #1.4 (7.18ms)
    --------------------------------
        AC:   6 [ 60%]
        WA:   4 [ 40%]
        RE:   0 [  0%]
    ```

## Todo

1. Add `.exe` suffix on Windows in `vcheck.py` and `vhack.py`. (see #2)
1. Introduce `vhack.py` in README.
1. Add a `zh-cn` version of the README introduction.
1. Extract shared codes in `vmake.py`, `vhack.py` and `vcheck.py`.
1. Auto generate subtask configuration files for HustOJ, LibraOJ, HydroOJ and more (maybe `vconf.py` ?).
1. Use command-line arguments rather that fixed `std_xxx`, `mk_xxx` stuff.
1. Check inputs with a codeforces styled validator (maybe `vvalidate.py` ?).
1. Write an introduction and documention about libvmake.

