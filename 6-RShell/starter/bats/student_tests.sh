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

@test "Check pwd" {
    run ./dsh <<EOF
pwd
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ ^/ ]]
}

@test "Invalid command" {
    run ./dsh <<EOF
invalidcommand
EOF
    [[ "$output" =~ "No such file or directory" ]] || [[ "$output" =~ "execvp failed" ]]
}

@test "File redirection" {
    run ./dsh <<EOF
echo "Hello World" > testfile.txt && cat testfile.txt
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "Hello World" ]]
}

@test "Append redirection" {
    echo "Line 1" > test_append.txt
    echo "Line 2" >> test_append.txt 
    run ./dsh <<EOF
cat test_append.txt
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "Line 1" ]] && [[ "$output" =~ "Line 2" ]]
}

@test "Input redirection" {
    run ./dsh <<EOF
echo "Hello" > input_test.txt && cat < input_test.txt
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "Hello" ]]
}

@test "Pipeline" {
    run ./dsh <<EOF
ls | grep .txt
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ \.txt ]]
}

@test "Background process" {
    run ./dsh <<EOF
sleep 2 &
EOF
    [ "$status" -eq 0 ]
}

@test "cd failure" {
    run ./dsh <<EOF
cd /cdfailure
EOF
    [[ "$output" =~ "cd: missing operand" ]] || [[ "$output" =~ "Error: Failed to change directory" ]] || [[ "$output" =~ "cd: No such file or directory" ]]
}

@test "Exit command" {
    run ./dsh <<EOF
exit
EOF
    [ "$status" -eq 0 ]
}

@test "Stop server" {
    run ./dsh <<EOF
stop-server
EOF
    [ "$status" -eq 0 ]
}