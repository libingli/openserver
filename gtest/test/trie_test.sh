#########################################################################
# File     : test.sh
# Author   : libingli
# Created  : Fri 15 Jan 2016 06:57:09 PM CST
#########################################################################
#!/bin/bash

for i in `seq 10`;do
    server=`echo "www."${i}".cc"`
    for j in `seq 20 50`;do
        url=`echo ${server}"/test"$j"/test"$i"/test"$j`
        echo ${url} >>trie_insert.txt
    done
done

for i in `seq 10`;do
    server=`echo www.${i}.cc`
    for j in `seq 20 30`;do
        url=`echo ${server}"/test"$j"/test"$i"/test"$j`
        echo ${url} >>trie_find.txt
    done
done
