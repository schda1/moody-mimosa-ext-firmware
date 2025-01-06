#!/bin/bash

file="tests/cpputest/configure.ac"      # Specify the file to modify
line_number=27        
line="AC_PROG_RANLIB"

# Check if the file has enough lines to insert
total_lines=$(wc -l < "$file")
if [ "$line_number" -gt "$((total_lines + 1))" ]; then
    echo "Error: Line number exceeds the total number of lines in the file."
    exit 1
fi

# Add the line at the specified line number if it's not already there
if ! sed -n "${line_number}p" "$file" | grep -Fxq "$line"; then
    sed -i "${line_number}i $line" "$file"
    echo "Line added at line number $line_number in $file."
else
    echo "The line already exists at line number $line_number."
fi