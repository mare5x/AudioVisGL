# AudioVisGL

An audio visualizer.

## About

**AudioVisGL** is a simple audio visualizer written using **SDL2** for window and audio handling, **OpenGL** for rendering and [**AudioFFT**](https://github.com/HiFi-LoFi/AudioFFT) for a FFT implementation.

AudioVisGL can visualize an audio file's wavedata, volume and audio frequencies.

Supported audio formats: those supported by SDL2_mixer.

## Usage

```AudioVisGL <path>```

 * **Q** to toggle the volume circle.
 * **W** to toggle the wavedata.
 * **E** to toggle the frequencies.

## Setup

 1. ```git clone https://github.com/mare5x/AudioVisGL.git```
 2. Open _AudioVisGL.sln_ and include/link: 
    * SDL2, 
    * [SDL2_mixer](https://www.libsdl.org/projects/SDL_mixer/), 
    * [AudioFFT](https://github.com/HiFi-LoFi/AudioFFT) and 
    * [glad](https://glad.dav1d.de/).
 3. Build project _AudioVisGL_.
 4. Copy required _.dll_ files to _AudioVisGL.exe_ location.

## Demo

(_Moon River_)  

![Demo gif](./demo.gif)