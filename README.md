# audio_visualizer
* Library Pre-requisites : 
  * ```*nix```:
    * Debian Based Distributions : 
       ```build-essential``` ```libsdl2-dev``` ```libsdl2-image-dev``` ```libsdl2-ttf-dev``` ```libfftw3-dev```
    * Others : Install equaivalent packages of the above mentioned. 
    * ```P.S.```: Make sure you have ```GLIB_C2.29``` or above (comes preinstalled from ```Ubuntu_19.04``` onwards)
    * or alternatively you can run install.sh
  * ```Windows 10```:
    * run install.ps1 with powershell
    * [```minGW```](https://winlibs.com/) (```GCC 8.4.0 64-bit``` or above is recommended)
* Compile with ```make```(works on any OS) 
* Terminal usage:
    * ```./musico -f Path_to_wav_file``` to play a certain song
    * ```./musico -r``` for recording and then playback
    * ```./musico -R``` for real time audio visualization
* Known Bugs:
    * Window resizing doesn't work on Windows due to Microsoft's ```OpenGL``` implementaion issue mentioned [here](https://github.com/libsdl-org/SDL/issues/1059)

