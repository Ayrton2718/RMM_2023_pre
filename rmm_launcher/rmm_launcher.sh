#!/bin/bash

TARGET_IP=172.22.4.11

bg_pid=0

function trap_ctrlc()
{
    if [ $bg_pid -ne 0 ]; then
        kill $bg_pid
        echo "Kill "$bg_pid
        bg_pid=0
    fi
    echo "exit"
    exit
}

trap trap_ctrlc SIGHUP SIGINT SIGTERM


function launch()
{
    START_TIME="$(date '+%Y-%m-%d_%H-%M-%S')"
    echo $START_TIME

    PROJECT_NAME="RobotRMM_2022"
    LOG_DIR=/tmp
    ARCHIVE_DIR=/var/tmp

    LOG_NAME="rmm_"$PROJECT_NAME"_current"
    ARCHIVE_NAME="rmm_"$PROJECT_NAME"_archive"

    if [ -d $LOG_DIR"/"$LOG_NAME ]; then
        if ! [ -d $ARCHIVE_DIR"/"$ARCHIVE_NAME ]; then
            mkdir $ARCHIVE_DIR"/"$ARCHIVE_NAME
        fi

        (cd $ARCHIVE_DIR"/"$ARCHIVE_NAME && rm -f `ls -tA1 . | grep .tar.gz | tail -n+61`)

        tar -zcvf $ARCHIVE_DIR"/"$ARCHIVE_NAME"/"$START_TIME".tar.gz" -C $LOG_DIR $LOG_NAME
        echo $ARCHIVE_DIR"/"$ARCHIVE_NAME"/"$START_TIME".tar.gz"

        if [ 0 -lt $(ls -1U $LOG_DIR"/"$LOG_NAME | wc -l) ]; then
            rm $LOG_DIR"/"$LOG_NAME"/"*
        fi
    else
        mkdir $LOG_DIR"/"$LOG_NAME
    fi

    (nohup /home/pi/RMMDeploy/build/main.out $LOG_DIR"/"$LOG_NAME 0<&- &>/dev/null) &
}

for (( ; ; ))
do
    if [ -e "/home/pi/RMMDeploy/run.sh" ] && [ -e "/home/pi/RMMDeploy/build/main.out" ]; then
    	if ping -c 1 -W 0.1 $TARGET_IP &> /dev/null ; then
            if [ $bg_pid -eq 0 ]; then
                launch
                bg_pid=$!
                echo "Start at "$bg_pid
            fi
            sleep 0.5
        else
            if [ $bg_pid -ne 0 ]; then
                if ! ping -c 1 -W 0.5 $TARGET_IP &> /dev/null ; then
                    kill $bg_pid
                    echo "Kill "$bg_pid
                    bg_pid=0
                fi
            fi
            sleep 0.05
        fi
    else
        echo "RMMDeploy dose not exist!"
        sleep 1
    fi
done
