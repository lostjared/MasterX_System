#!/bin/bash
# for Linux
source ./projects.sh

for dir in "${directories[@]}"; do
    echo "Entering directory: $dir"
    cd "$dir"
    
    if make > stdout.txt 2>&1; then
        echo "Compilation successful in $dir"
	    echo "Executing..."
        if valgrind ./program > valgrind.txt 2>&1; then
		echo "Valgrind Success.."
        cat valgrind.txt
        make clean
        else
        echo "valgrind failure.."
        exit 1
        fi
    else
        echo "Compilation failed in $dir. Check stdout.txt for details."
        cd ..
        exit 1
    fi
    
    cd ..
done

echo "All compilations and tests completed successfully."
