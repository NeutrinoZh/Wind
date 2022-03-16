#!/bin/bash

Configure=Debug

echo -e "Build programm...\n"

cd src
SUB_DIR=`../get_directory`
cd ..

if [[ $Configure == Release ]]
then
    make build configure=$Configure SUB_DIR=$SUB_DIR

    echo -e "\nRun Program...\n"

    if bin/$Configure/main
    then
        echo -e "\n\033[32mThe program completed successfully.\033[37m"
    else
        echo -e "\n\033[31mThe program ended incorrectly."
        echo -e "\n----------------------------------STACKDUMP----------------------------------\033[37m\n"

        while IFS= read -r line
        do
            echo "$line"
        done < './main.exe.stackdump'

        echo -e "\n\033[31m-------------------------------------------------------------------------------\033[37m\n"
    fi
else
    make build configure=$Configure SUB_DIR=$SUB_DIR compile_flags=-g

    echo -e "\nRun Debug Program...\n"

    gdb bin/$Configure/main
fi

read -N 1 -p "Press any key to continue..."