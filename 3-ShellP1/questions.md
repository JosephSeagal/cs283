1. In this assignment I suggested you use `fgets()` to get user input in the main while loop. Why is `fgets()` a good choice for this application?

    > **Answer**: Using `fgets()` is a good choice for this application because it safely reads user input line by line, prevents buffer overflows, and handles EOF properly.

2. You needed to use `malloc()` to allocte memory for `cmd_buff` in `dsh_cli.c`. Can you explain why you needed to do that, instead of allocating a fixed-size array?

    > **Answer**: Allocating memory for `cmd_buff` with `malloc()` instead of a fixed-size array enables dynamic memory management, optimizing space usage and flexibility.

3. In `dshlib.c`, the function `build_cmd_list()` must trim leading and trailing spaces from each command before storing it. Why is this necessary? If we didn't trim spaces, what kind of issues might arise when executing commands in our shell?

    > **Answer**: Trimming spaces in `build_cmd_list()` prevents errors by ensuring commands are correctly recognized and executed. Without trimming, it could cause incorrect parsing, create empty or unintended commands, and lead to execution errors.

4. For this question you need to do some research on STDIN, STDOUT, and STDERR in Linux. We've learned this week that shells are "robust brokers of input and output". Google _"linux shell stdin stdout stderr explained"_ to get started.

- One topic you should have found information on is "redirection". Please provide at least 3 redirection examples that we should implement in our custom shell, and explain what challenges we might have implementing them.

    > **Answer**: Three redirection examples we should implement in our custom shell are `<` (input), `>` and `>>` (output), and `2>` (error). A challenge in implementing them is handling file permissions, missing files, and correctly managing input and output streams.

- You should have also learned about "pipes". Redirection and piping both involve controlling input and output in the shell, but they serve different purposes. Explain the key differences between redirection and piping.

    > **Answer**: Redirection sends input or output to files, while piping passes output from one command directly into another.

- STDERR is often used for error messages, while STDOUT is for regular output. Why is it important to keep these separate in a shell?

    > **Answer**: Keeping `STDERR` and `STDOUT` separate makes sure that error messages don’t mix with regular output, making it easier to debug issues and process command results.

- How should our custom shell handle errors from commands that fail? Consider cases where a command outputs both STDOUT and STDERR. Should we provide a way to merge them, and if so, how?

    > **Answer**: Our shell should handle errors by keeping `STDERR` separate from `STDOUT` but allow merging them with `2>&1` when needed.