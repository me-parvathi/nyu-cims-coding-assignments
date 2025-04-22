#!/bin/sh

mkdir -p data
curl https://raw.githubusercontent.com/dwyl/english-words/refs/heads/master/words.txt > data/words.txt
echo "data/words.txt created"