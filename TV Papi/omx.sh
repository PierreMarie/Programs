#!/bin/bash
var=$(curl -s https://www.youtube.com/results?search_query=tv+libert%C3%A9s+ | grep -o 'https://i.ytimg.com/vi/.*/hqdefault')
var2=${var:23:11}
var3="https://www.youtube.com/watch?v="$var2
omxplayer `youtube-dl -g -f mp4 $var3`&