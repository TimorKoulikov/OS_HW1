#!/bin/bash

# Define your source and destination paths.
# Update the SRC_DIR to point to exactly where your shared folder is mounted.
SRC_DIR="/mnt/hgfs/shared_folder_wet1/OS_HW1"
DEST_DIR="$HOME/linux-4.4.0-custom"

# 1. Verify the source directory exists
if [ ! -d "$SRC_DIR" ]; then
    echo "Error: Source directory $SRC_DIR not found."
    echo "Make sure your VMware shared folder is mounted."
    exit 1
fi

# 2. Verify the destination directory exists
if [ ! -d "$DEST_DIR" ]; then
    echo "Warning: Target directory $DEST_DIR does not exist."
    read -p "Do you want to create it? (y/n) " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        mkdir -p "$DEST_DIR"
    else
        echo "Aborting."
        exit 1
    fi
fi

echo "Applying custom kernel files..."

# 3. Use rsync to overlay the files
# The trailing slash on $SRC_DIR/ is critical! It means "copy the contents of this folder" 
# rather than "copy the folder itself".
rsync -av --update --exclude="sync.sh" --exclude="user/" --exclude=".git/" "$SRC_DIR/" "$DEST_DIR/"

echo "Overlay complete!"
