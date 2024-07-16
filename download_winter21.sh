#!/bin/bash

# Base URL
BASE_URL="https://web.stanford.edu/class/archive/cs/cs106b/cs106b.1214"

# Declare an associative array with assignment names and their corresponding paths
declare -A assignments=(
    ["Assignment 9"]="assignments/Assignment%209.zip handouts/YEAH_slides/YEAH%20A9.pdf build/assignments/9-huffman"
    ["Assignment 8"]="assignments/Assignment%208.zip handouts/YEAH_slides/YEAH%20A8.pdf build/assignments/8-links"
    ["Assignment 7"]="assignments/Assignment%207.zip handouts/YEAH_slides/YEAH%20A7.pdf build/assignments/7-hash-off"
    ["Assignment 6"]="assignments/Assignment%206.zip handouts/YEAH_slides/YEAH%20Hours%20A6.pdf build/assignments/6-data-sagas"
    ["Assignment 5"]="assignments/Assignment%205.zip handouts/YEAH_slides/YEAH%20A5.pdf build/assignments/5-big-o"
    ["Assignment 4"]="assignments/Assignment%204.zip handouts/YEAH_slides/YEAH_A4.pdf build/assignments/4-backtracking"
    ["Assignment 3"]="https://forms.gle/v2rfyifLgE1xxAhu9 handouts/YEAH_slides/YEAH%20A3.pdf build/assignments/3-recursion"
    ["Assignment 2"]="assignments/Assignment%202.zip handouts/100%20Assignment%202.pdf handouts/YEAH_slides/YEAH%20A2%20(Winter%202021).pdf"
    ["Assignment 1"]="assignments/Assignment%201.zip handouts/050%20Assignment%201.pdf handouts/YEAH_slides/YEAH%20A1.pdf"
    ["Assignment 0"]="assignments/Assignment%200.zip handouts/040%20Assignment%200.pdf materials/DebuggerTutorial.pdf"
)

# Create directories and download files
for assignment in "${!assignments[@]}"; do
    # Create directory for the assignment
    mkdir -p "$assignment"
    cd "$assignment" || exit

    # Split the file paths into an array
    IFS=' ' read -r -a files <<< "${assignments[$assignment]}"

    # Download each file
    for file in "${files[@]}"; do
        wget "$BASE_URL/$file"
    done

    # Return to the base directory
    cd ..
done
