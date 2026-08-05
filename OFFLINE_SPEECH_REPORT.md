# Offline Speech Integration Report

## Scope

The camera VLM GUI now supports a local half-duplex, hands-free voice workflow:

1. Select **开启语音对话** once; the ES8388 microphone then listens continuously.
2. Local amplitude detection finds speech and submits an utterance after the speaker pauses.
3. Run local Chinese transcription with Whisper.
4. Submit the recognized text to the already deployed Qwen3-VL camera session.
5. Synthesize the completed answer with Piper and play it through the default PulseAudio output.
6. Resume listening automatically after playback; select **停止语音对话** to exit.

No network connection is required after the speech models and application are installed.

## Installed Components

- ASR: `ggml-org/whisper.cpp` v1.9.2, commit `306c88f4d1286aec1bf96e544632897886af5501`
- ASR model: multilingual `ggml-base.bin`, SHA256 `60ed5bc3dd14eea856493d334349b405782ddcaf0028d4b5df4088345fba2efe`
- TTS: Piper 1.6.0 in the private Python environment `runtime/piper-1.6.0`
- TTS model: `zh_CN-huayan-medium`, SHA256 `9929917bf8cabb26fd528ea44d3a6699c11e87317a14765312420be230be0f3d`
- Capture: ALSA `hw:2,0`, 16-bit stereo at 16 kHz, converted to mono for Whisper
- Playback: PulseAudio `paplay` through the paired `Round-X09` A2DP sink

## System Changes

`python3.10-venv`, `python3-pip-whl`, and `python3-setuptools-whl` were installed. Ubuntu's Python 3.10 packages were upgraded from `3.10.12-1~22.04.6` to `3.10.12-1~22.04.16` as part of that package transaction. Piper itself and its Python dependencies remain application-private.

## Privacy And Runtime Behavior

Recorded audio, transcripts, and synthesized answers are stored under `logs/voice/`. Speech recognition and synthesis execute locally. Listening is paused while Qwen3-VL answers and while Piper audio is playing, preventing the speaker output from becoming a new question. The GUI does not start an NPU model at launch; Qwen3-VL still runs only after speech is detected and transcribed.

## Remaining Validation

- Live microphone input was recorded and transcribed by Whisper; transcript files were produced successfully.
- The user confirmed hearing the synthesized Chinese test phrase from `Round-X09`.
- One complete voice question through Qwen3-VL and back to speech remains to be confirmed by the user.
- The `zh_CN-huayan-medium` model card declares the source dataset license as unknown. Keep it limited to local prototype use until redistribution rights are resolved.
