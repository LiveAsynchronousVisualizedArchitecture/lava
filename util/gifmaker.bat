
echo %1

ffmpeg.exe -threads 12 -i %1 -vf fps=8,scale=iw/3:ih/3:flags=lanczos,palettegen %1.png

ffmpeg.exe -threads 12 -i %1 -i %1.png -filter_complex "fps=12,scale=iw/3:ih/3:flags=lanczos[x];[x][1:v]paletteuse" %1.gif

ffmpeg.exe -threads 12 -i %1 -i %1.png -filter_complex "fps=12,scale=iw/12:ih/12:flags=lanczos[x];[x][1:v]paletteuse" Thumb_%1.gif

