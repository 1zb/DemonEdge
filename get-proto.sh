#!/bin/sh

URL=https://raw.githubusercontent.com/
USERNAME=SteamDatabase
PROJECT=GameTracking
REPO_PATH=Protobufs/dota
FOLDER=proto

FILES=(
    "demo.proto" 
    "netmessages.proto" 
    "network_connection.proto" 
    "networkbasetypes.proto"
)

# mkdir ${FOLDER}
cd ${FOLDER}

for FILE in ${FILES[@]}; do
    echo ${URL}${USERNAME}/${PROJECT}/master/${REPO_PATH}/${FILE}
    curl -O ${URL}${USERNAME}/${PROJECT}/master/${REPO_PATH}/${FILE}
done