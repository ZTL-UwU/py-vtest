# py-Vtest

![[LICENSE](https://github.com/zhangtianli2006/py-vtest/blob/main/LICENSE)](https://img.shields.io/github/license/zhangtianli2006/py-vtest?style=flat-square)
![Code Size](https://img.shields.io/github/languages/code-size/zhangtianli2006/py-vtest?style=flat-square)

An OI test-data maker & std test tool.

## Mode

1. **vmake**: Automatically generate test-data with user-provided std and Maker (described below).
2. **vcheck**: Judge a solution like a OnlineJudge does.  
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

1. Make an empty folder.
2. Create `vtest.conf` with the following content:

    ```plaintext
    AplusB 2
    data
    6
    4
    ```

3. Download `vmake.py` and `vcheck.py`.

    You can use these commands if you are a command-line user:

    ```bash
    wget https://github.com/zhangtianli2006/py-vtest/raw/main/vmake.py
    wget https://github.com/zhangtianli2006/py-vtest/raw/main/vcheck.py
    ```

4. Create an executable named `std_AplusB`.

    For example, here is a C++ version of it, just compile it:

    ```cpp
    #include <iostream>

    int main()
    {
        long long a, b;
        std::cin >> a >> b;
        std::cout << a + b;
        return 0;
    }
    ```

5. Create an executable named `mk_AplusB` which is a Maker (described above).

    For example, here is a C++ version of it, just compile it:

    ```cpp
    #include <iostream>
    #include <random>

    int main()
    {
        int subtask_id;
        std::cin >> subtask_id;

        if (subtask_id == 1) 
        {
            std::mt19937 rng(std::random_device{}());
            std::cout << rng() << " " << rng();
        }

        if (subtask_id == 2)
        {
            // In this subtask, we will try to hack solutions without using long long
            std::mt19937_64 rng(std::random_device{}());
            std::cout << rng() << " " << rng();
        }

        return 0;
    }
    ```

6. Run `vmake.py`.

    You can use these commands if you are a command-line user:

    ```bash
    python3 vmake.py
    ```

    The program outputs like this:

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

    This is a C++ version of it, just compile it to `run_AplusB`:

    ```cpp
    #include <iostream>

    int main()
    {
        int a, b;
        std::cin >> a >> b;
        std::cout << a + b;
        return 0;
    }
    ```

8. Run `vcheck.py`

    You can use these commands if you are a command-line user:

    ```bash
    python3 vcheck.py
    ```

    The program may outputs like this:

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
