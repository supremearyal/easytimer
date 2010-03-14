#!/bin/bash

for i in {0..9}
do
	convert -background lightblue -fill blue -size 50x100\
	-font Tuffy-Regular -pointsize 72 -gravity center label:$i\
	digit${i}.png
done

montage -label '' digit*.png -geometry +0+0 -tile x1 numbers.png
rm digit*.png
