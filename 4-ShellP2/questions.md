1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**:  I think we use `fork/execvp` rather than just calling `execvp` because `fork` creates a new child process, allowing the parent to continue operating while the child executes a new program using `execvp`. The benefit that I believe `fork` brings is that it avoids the shell from being replaced by the executed command.

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**:  If the `fork()` system call fails, a new process cannot be established, typically due to system resource constraints. My approach handles this situation by printing an error and continues the program without crashing.

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**:  `execvp()` finds the command to execute by looking into the folders given in the `PATH` environment variable.

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**:  The purpose of calling `wait()` in the parent process after forking is to ensure that the child process finishes before the parent process resumes execution. If we don't call it, the child process will finish but remain inactive in the system until the parent gets the termination status.

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**:  `WEXITSTATUS()` returns the exit code of a terminated process. This is crucial because it helps the shell to assess whether a command was successful or unsuccessful.

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**:  My implementation of `build_cmd_buff()` handles quoted arguments as single arguments while keeping spaces within quotes. This is required to guarantee that commands such as `echo "hello world"` work properly rather than breaking `"hello world"` into separate arguments.

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**:  Compared to the prior assignment, I refactored my parsing code to handle a single `cmd_buff` instead of a command list and improved whitespace handling. One unexpected problem I encountered was ensuring that quoted arguments were properly interpreted without breaking usual argument splitting.

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**:  Signals in a Linux system enable asynchronous communication between processes by transmitting notifications or control instructions. Signals differ from other forms of interprocess communication in that they are event-driven and do not require direct data transfer, unlike pipes or shared memory.

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**:  Three often used signals and their use cases are `SIGALRM`, which triggers a timer in a process, `SIGSTOP`, which pauses a process until resumed, and `SIGCONT`, which resumes a stopped process.

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**:  When a process receives the `SIGSTOP` signal, it is suspended instantly. It cannot be intercepted or disregarded like `SIGINT` because it is intended to be an unconditional stop that can only be resumed with `SIGCONT`.
