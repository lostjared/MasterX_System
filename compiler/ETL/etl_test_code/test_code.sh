#!/bin/bash


source ./projects.sh

for dir in "${directories[@]}"; do
    echo "Entering directory: $dir"
    cd "$dir"
    
    if make > stdout.txt 2>&1; then
        echo "Compilation successful in $dir"
    else
        echo "Compilation failed in $dir. Check stdout.txt for details."
        cd ..
        exit 1
    fi
    
    cd ..
done

echo "All compilations completed successfully."
