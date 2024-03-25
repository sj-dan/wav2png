# wav2png
A simple C program to convert WAV audio into PNG images, and vice versa.

## Why did I make this?
Wanted to start learning C. Decided to do an "easy" project where not much can go wrong, and here we are.

## How does it work?
Well, if you do the math, it makes sense. Each sample in a 24-bit signed integer PCM WAV file is, well, 24 bits. And every pixel in a standard RGB PNG image has 3 values that range from 0-255, each 8 bits, which also adds up to... 24 bits!

So what if we encoded each sample from the WAV file as a pixel in the PNG file? I rest my case.

### Multi-channel support
The way multi-channel support is handled is pretty simple. By default, the width of the output PNG file is 2000px, and if there are multiple channels, the width is a multiple of 2000px.

For example, a stereo WAV file would produce a PNG file with the width 4000px (2000px * 2 channels).

### Sample rate support
The sample rate, as well as the sample count (since the amount of samples isn't always divisible by the width of the image) are both stored as PNG comments. If those PNG comments are not present, the program will default to 44.1kHz for the sample rate, and width*height for the sample count. In both cases, no detail is lost, as sample rate can be changed with programs such as Audacity, and the sample count is always rounded up, resulting in some silence at the end, instead of resulting in the audio being truncated (i.e. if it was rounded down).

## Compile instructions
**NOTE: This program is completely untested on Windows.**

<sup>maybe it works?</sup>

On macOS or Linux, run the following (copy-paste friendly):
```
git clone https://github.com/sj-dan/wav2png; cd ./wav2png; chmod +x ./build.sh; ./build.sh
```
Your executable will end up in a folder called `dist/` by default.

## Basic usage

When converting WAV -> PNG:
```
./wav2png <input_wav_file> <output_png_file>
```
When converting PNG -> WAV:
```
Usage: ./wav2png --reverse <input_png_file> <output_wav_file>
```

## Try it yourself!
Here's an image that should decode to [this](https://youtu.be/ptmHKa7KlMo):
![Egmont Overture Finale](https://raw.githubusercontent.com/sj-dan/wav2png/618eef4b0f510a6951be5d705545d91312060c17/examples/egmont.png)
Command: `./wav2png --reverse example.png example.wav`
