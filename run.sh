#!/bin/sh

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

if ! [ "$1" = "-ne" ]; then
    set -x
    ./build/main.out $LOG_DIR"/"$LOG_NAME
fi