# OpenGL Demo

This is a sample application to show how an OpenGL output can be outputted to a video file.

## TOFIX

* image reading color incorrect on window view, BGR vs RGB?
* window view seems to flip the image but video output looks ok

## Run

```
make
./hello
```
Close the window after a few secs.
A video file named out.avi should be created in the same directory.

You may need VLC to view the video.

Reqs:
* OpenGL
* GLEW
* ffmpeg
* libjpeg
* OpenCV

On Mac OSX:
```
brew install glew
brew install libjpeg
brew install ffmpeg
brew tap homebrew/science
brew install opencv --env=std
```

NOTE: you will need to build ffmpeg with x264 from source to write the
video file as x264.
http://stackoverflow.com/questions/15662827/writing-an-mp4-file-on-the-mac-with-opencv-ffmpeg

