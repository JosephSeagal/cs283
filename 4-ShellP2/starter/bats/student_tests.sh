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

@test "Prints current working directory with pwd" {
    current=$(pwd)
    run ./dsh <<EOF
pwd
exit
EOF
    [[ "$output" == *"$current"* ]]
    [ "$status" -eq 0 ]
}

@test "cd with no arguments leaves directory unchanged" {
    current=$(pwd)
    run ./dsh <<EOF
cd
pwd
exit
EOF
    [[ "$output" == *"$current"* ]]
    [ "$status" -eq 0 ]
}

@test "Invalid cd command displays an error" {
    run ./dsh <<EOF
cd fakedirectory
exit
EOF
    [[ "$output" == *"No such file"* || "$output" == *"not found"* ]]
    [ "$status" -eq 0 ]
}

@test "Empty input triggers a warning message" {
    run ./dsh <<EOF

exit
EOF
    [[ "$output" == *"No command"* ]]
    [ "$status" -eq 0 ]
}

@test "Unknown command produces an execvp error" {
    run ./dsh <<EOF
fakecommand
exit
EOF
    [[ "$output" == *"execvp:"* ]]
    [ "$status" -eq 0 ]
}

@test "Processes multiple commands sequentially" {
    current=$(pwd)
    run ./dsh <<EOF
pwd
echo testing
ls
exit
EOF
    [[ "$output" == *"$current"* ]]
    [[ "$output" == *"testing"* ]]
    [ "$status" -eq 0 ]
}

@test "Collapses extra spaces in echo command output" {
    run ./dsh <<EOF
echo   test    testing
exit
EOF
    [[ "$output" == *"test testing"* ]]
    [ "$status" -eq 0 ]
}

@test "Executes external command with arguments properly" {
    run ./dsh <<EOF
echo testing tests
exit
EOF
    [[ "$output" == *"testing tests"* ]]
    [ "$status" -eq 0 ]
}

@test "Displays shell prompt after command execution" {
    run ./dsh <<EOF
pwd
exit
EOF
    [[ "$output" == *"dsh2>"* ]]
    [ "$status" -eq 0 ]
}
