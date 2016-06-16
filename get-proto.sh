#!/bin/bash

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
    "dota_usermessages.proto"
    "dota_commonmessages.proto"
)

# mkdir ${FOLDER}
cd ${FOLDER}

for FILE in ${FILES[@]}; do
    echo ${URL}${USERNAME}/${PROJECT}/master/${REPO_PATH}/${FILE}
    curl -O ${URL}${USERNAME}/${PROJECT}/master/${REPO_PATH}/${FILE}
done