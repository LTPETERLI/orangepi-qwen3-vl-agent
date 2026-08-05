# Offline Speech Integration Report

## Scope

The camera VLM GUI now supports a local half-duplex, hands-free voice workflow:

1. The ES8388 microphone starts listening automatically three seconds after the GUI launches.
2. Local amplitude detection finds speech and submits an utterance after the speaker pauses.
3. Run local Chinese transcription with Whisper.
4. Submit the recognized text to the already deployed Qwen3-VL camera session.
5. Synthesize the completed answer with Piper and play it through the default PulseAudio output.
6. Resume listening automatically after playback; select **停止语音对话** to exit and use the same button to resume.

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
- A complete microphone -> Whisper -> Qwen3-VL -> Piper -> Round-X09 turn was verified on 2026-08-06. Listening resumed automatically after playback.
- The `zh_CN-huayan-medium` model card declares the source dataset license as unknown. Keep it limited to local prototype use until redistribution rights are resolved.

## Latency And Microphone Correction

On 2026-08-06, two orphaned Qwen demo processes were found consuming approximately 6.4 GB RSS in total and more than two CPU cores. The GUI now launches Qwen in its own process group and escalates cleanup from a graceful exit to `SIGTERM` and then `SIGKILL` when required.

The orphaned processes were blocked in RKNPU driver work and produced `failed to allocate IOVA: -12` when another model attempted to load. A board reboot was required to release them. After reboot, approximately 14 GiB was available and no new IOVA allocation error appeared during the accepted voice round trip.

The first continuous-listening threshold was too high for measured speech near -36 dBFS. Continuous mode now calibrates ambient noise for 0.8 seconds, clamps its adaptive speech threshold between 120 and 350 RMS, requires two consecutive speech windows, and ends an utterance after 1.2 seconds of silence. Whisper uses six CPU threads and single-candidate decoding to reduce latency.

The deployed demo may delimit a completed answer with either an `I rkllm:` statistics line or the next `user:` prompt. The GUI accepts both formats before starting Piper playback.

Voice turns cap Qwen output at 64 tokens to reduce generation and playback latency. The accepted post-reboot turn began Bluetooth playback approximately 19 seconds after the Qwen process started; later turns reuse the resident model.

Every Qwen voice prompt includes the persistent identity instruction that the assistant is named `嘚包`, the user is `主人`, and each response begins with `主人，`. Playback explicitly targets the `Round-X09` A2DP sink whenever it is available instead of relying only on the PulseAudio default.

Because the official demo binds one image when the process starts, the GUI stops that model process after every spoken answer, resumes the live RGB-D preview, waits for a fresh frame, and only then resumes listening. Each new question therefore analyzes a current frame, at the cost of reloading the model for every turn.
