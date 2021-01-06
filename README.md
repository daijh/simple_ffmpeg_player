# simple_ffmpeg_player
A very simple [FFmpeg](https://github.com/FFmpeg/FFmpeg) player

## Setup
- Install dependancies, FFmpeg libraries

```bash
./scripts/install_deps.sh
```

- Build

```bash
mkdir build
cd build
cmake ..
make
```

## Run

```bash
# set library path
source ./scripts/setupvars.sh

# Usage: simple_ffmpeg_player
./build/simple_ffmpeg_player -h

# play local file
./build/simple_ffmpeg_player  test.mp4

# create hls
ffmpeg -i test.mp4 -acodec copy -vcodec copy test.m3u8
# play hls
./build/simple_ffmpeg_player  test.m3u8
```

## B-frames
B-frames usually are not used for low latency streaming. Only use non-B-frames clip as the input.

```bash
# check B-frames
ffprobe -show_frames test.mp4 | grep "pict_type"

# transcode to non B-frames h264
ffmpeg -i test.mp4 -acodec copy -vcodec libx264 -x264opts bframes=0:ref=1 non-b-frames-test-h264.mp4

# transcode to non B-frames hevc
ffmpeg -i test.mp4 -acodec copy -vcodec libx265 -x265opts bframes=0:ref=1 non-b-frames-test-hevc.mp4
```

## Known limitation
- Under development, functionality not ready

