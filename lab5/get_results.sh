#!/bin/bash

# Check if the password has been provided as an argument
if [ $# -ne 2 ]; then
    echo "Usage: $0 <user> <password>"
    exit 1
fi

# The password for the remote connection
REMOTE_PASSWORD="$2"

# Local directories
LOCAL_BASE="./"
LOCAL_IMAGES="./images"
LOCAL_KERNELS="./kernels"
LOCAL_LOGS="./logs"
LOCAL_OUTPUT="./output"

# Remote directories
REMOTE_USER="$1"
REMOTE_HOST="central.cps.unizar.es"
REMOTE_BASE="/home/$REMOTE_USER/PACS/lab5"
REMOTE_IMAGES="$REMOTE_BASE/images"
REMOTE_KERNELS="$REMOTE_BASE/kernels"
REMOTE_LOGS="$REMOTE_BASE/logs"
REMOTE_OUTPUT="$REMOTE_BASE/output"

# Check if sshpass is installed
if ! command -v sshpass &> /dev/null; then
    echo "Error: sshpass is not installed. Install it with 'sudo apt install sshpass'."
    exit 1
fi

# Download logs and results from the remote machine without overwriting
echo "Downloading logs and results from the remote machine..."
if [ ! -d "$LOCAL_LOGS" ]; then mkdir -p "$LOCAL_LOGS"; fi
if [ ! -d "$LOCAL_OUTPUT" ]; then mkdir -p "$LOCAL_OUTPUT"; fi

sshpass -p "$REMOTE_PASSWORD" rsync -av --ignore-existing "$REMOTE_USER@$REMOTE_HOST:$REMOTE_LOGS/" "$LOCAL_LOGS/"
sshpass -p "$REMOTE_PASSWORD" rsync -av  "$REMOTE_USER@$REMOTE_HOST:$REMOTE_OUTPUT/" "$LOCAL_OUTPUT/"

echo "Synchronization completed."
