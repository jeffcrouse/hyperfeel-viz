hyperfeel-viz
=============

visualization for the Hyperfeel Mindwave insallation.  Using openFrameworks 0.8

clone directly into apps/ folder


###Required Addons

* [ofxUI](https://github.com/rezaali/ofxUI)
* [ofxLibwebsockets](https://github.com/labatrockwell/ofxLibwebsockets)
	* as noted in README, make sure to delete ofxLibwebsockets/libs/libwebsockets/include/win32port
* [ofxAudioUnit](https://github.com/admsyn/ofxAudioUnit)* [ofxHttpUtils](https://github.com/arturoc/ofxHttpUtils)* [ofxVideoRecorder](https://github.com/timscaffidi/ofxVideoRecorder)

### Soundflower

In order to record the generative audio, we will use [the Soundflower "trick"](http://www.macworld.com/article/1159440/soundflower_capture.html). So, for the recording to work:

* Make sure that you have installed [SoundFlower](https://code.google.com/p/soundflower/) 
* And set System Preferences > Sound > Output to "Soundflower (2ch)"
* This will direct all audio from the entire system to a virtual 
* Since we are routing all sound to Soundflower, You'll also have to launch Soundflowerbed and select "Built-in Output" in order to be able to hear the sound output.
* Make sure to choose the "soundflower (2ch)" device in ofSoundStream. eg: ```` soundStream.setDeviceID(5); ````

