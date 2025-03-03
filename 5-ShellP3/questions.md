1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

My implementation ensures that all child processes complete before the shell continues accepting user input by calling `waitpid()` on each child process after forking. This prohibits the shell from running new commands while the old ones are still active. If I forget to call `waitpid()` on all child processes, they may continue to run in the background, resulting in resource leaks and unexpected behavior.

2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

It is necessary to close unused pipe ends after calling `dup2()` to prevent resource leaks and blocking behavior. If pipes are left open, processes that expect an EOF may hang endlessly since the write end is still open, leading `read()` to never return EOF. Leaving pipes open wastes system resources.

3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

`cd` is implemented as a built-in rather than an external command because changing directories has an effect on the shell's process. If `cd` was implemented as an external process, it would only alter the working directory of the child process, leaving the shell's working directory unchanged, leaving `cd` ineffective.

4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

I would change my approach to allow an unlimited amount of piped commands by dynamically allocating memory for commands with `malloc()` and `realloc()` rather than a fixed-size array. This method would allow the shell to process an infinite number of commands while effectively managing memory. The trade-offs include higher memory management complexity, the possibility of memory leaks, and the additional overhead of dynamic allocation when compared to static arrays.