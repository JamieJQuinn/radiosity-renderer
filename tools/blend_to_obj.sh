#!/usr/bin/env bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
obj_name=`echo $1 | sed 's/\.blend/\.obj/'`
echo $obj_name
blender $1 --background --python $DIR/blend_to_obj.py -- $obj_name
sed -i -e "s/\/\//\/0\//g" $obj_name
