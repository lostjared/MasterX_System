#!/bin/bash

source ./projects.sh

for dir in "${directories[@]}"; do
    echo "Entering directory: $dir"
    cd "$dir"
    
    if make > stdout.txt 2>&1; then
        echo "Compilation successful in $dir"
	echo "Executing..."
        if ./program ; then
		echo "Success.."
	else
		echo "Failure program crashed"
		exit 1
	fi
    else
        echo "Compilation failed in $dir. Check stdout.txt for details."
        cd ..
        exit 1
    fi
    
    cd ..
done

echo "All compilations completed successfully."
