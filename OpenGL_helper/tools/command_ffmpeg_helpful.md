ffmpeg -i input.jpg -vf "colorkey=0x000000:0.1:0.1" out.png

ffmpeg -i input.jpg -vf "format=rgba,lumakey=threshold=0.1:tolerance=0.01" out.png

ffmpeg -i C_background.png -vf "colorkey=white:0.5:0.5" C_nobgr.png

ffmpeg -i C_background.png -vf "colorkey=white:0.5:0.5,format=rgba,colorchannelmixer=aa=0.7" C_nobgr.png -y && feh C_nobgr.png

ffmpeg -i C_background.png -filter_complex "[0:v]colorkey=white:0.5:0.5,format=rgba,colorchannelmixer=aa=0.4[src];[src]split[bg][fg];[bg]gblur=sigma=5[shadow];[shadow][fg]overlay=5:5" C_nobgr.png -y && feh C_nobgr.png

ffmpeg -i C_background.png -vf "colorkey=white:0.5:0.5,format=rgba,colorchannelmixer=aa=0.4,geq=r='r(X,Y)':a='if(gt(alpha(X,Y),0),alpha(X,Y)*(1-(pow(X-W/2,2)+pow(Y-H/2,2))/pow(hypot(W,H)/1.5,2)),0)'" C_nobgr.png -y && feh C_nobgr.png


ffmpeg -i C_background.png -vf \
"colorkey=white:0.5:0.5,format=rgba, \
lenscorrection=k1=0.4:k2=0.4, \
vignette='PI/2':eval=frame:dither=0, \
lutrgb=a='val*0.6', \
unsharp=7:7:1.5" \
C_nobgr.png -y && feh C_nobgr.png


 ffmpeg -i C_Green.png -filter_complex "[0:v]colorkey=white:0.1:0.1,format=rgba,colorchannelmixer=aa=0.3[src];[src]split[bg][fg];[bg]gblur=sigma=5[shadow];[shadow][fg]overlay=5:9" C_nobgr.png -y && feh C_nobgr.png
