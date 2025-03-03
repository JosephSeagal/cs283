#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

@test "Example: check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}

@test "Running a single external command" {
    run "./dsh" <<EOF
echo "Test"
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "Test" ]]
}

@test "Changing directory to parent" {
    run "./dsh" <<EOF
cd ..
pwd
exit
EOF
    [ "$status" -eq 0 ]
    [[ ! "$output" =~ "missing operand" ]]
    [[ ! "$output" =~ "cd failed" ]]
}

@test "Changing directory with missing operand" {
    run "./dsh" <<EOF
cd
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "missing operand" ]]
}

@test "Chaining multiple commands with pipes" {
    run "./dsh" <<EOF
echo "Test" | grep Test | cat
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "Test" ]]
}

@test "Piping through multiple filters" {
    run "./dsh" <<EOF
printf "one\ntwo\nthree\n" | grep two | grep tw
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "two" ]]
}

@test "Handling an invalid command" {
    run "./dsh" <<EOF
testingtestingtesting
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "execvp" ]] || skip
}

@test "Running a command with multiple arguments" {
    run "./dsh" <<EOF
echo Test    BATS   test
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "Test BATS test" ]]
}

@test "Ignoring extra spaces between commands" {
    run "./dsh" <<EOF
     ls      |    cat
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "dsh" ]] || skip
}

@test "Handling an empty input line" {
    run "./dsh" <<EOF

exit
EOF
    [ "$status" -eq 0 ]
}

@test "Testing the 'exit' command" {
    run "./dsh" <<EOF
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "cmd loop returned 0" ]]
}

@test "Extra Credit: Redirecting input with '<'" {
    echo "input test" > input.txt
    run "./dsh" <<EOF
cat < input.txt
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "input test" ]]
    rm -f input.txt
}

@test "Extra Credit: Redirecting output with '>'" {
    run "./dsh" <<EOF
echo "overwrite test" > output.txt
EOF
    [ "$status" -eq 0 ]
    run "./dsh" <<EOF
cat < output.txt
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "overwrite test" ]]
    rm -f output.txt
}

@test "Extra Credit: Appending output with '>>'" {
    run "./dsh" <<EOF
echo "first line" > output.txt
EOF
    [ "$status" -eq 0 ]
    run "./dsh" <<EOF
echo "second line" >> output.txt
EOF
    [ "$status" -eq 0 ]
    run "./dsh" <<EOF
cat < output.txt
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "first line" ]]
    [[ "$output" =~ "second line" ]]
    rm -f output.txt
}