#!/bin/bash
dt=0
for param in $@
do
    let "dt=dt+param"
done
let "dt=dt/$#"
echo $dt