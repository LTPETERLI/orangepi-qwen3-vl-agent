#!/usr/bin/env python3
import datetime
import os
import pathlib
import pty
import signal
import subprocess
import threading

import gi

gi.require_version("Gtk", "3.0")
gi.require_version("GdkPixbuf", "2.0")
from gi.repository import GdkPixbuf, GLib, Gtk


WORKSPACE = pathlib.Path(os.environ.get("ORANGEPI_WORKSPACE", "/home/orangepi/qwen3_vl_2b_ws"))
REALSENSE_PREFIX = WORKSPACE / "runtime/librealsense-2.56.5-rsusb"
RKLLM_PREFIX = WORKSPACE / "runtime/rkllm-1.3.0"
KEYFRAME_APP = WORKSPACE / "app/realsense-keyframe"
PREVIEW_APP = WORKSPACE / "app/realsense-preview"
DETECTOR_MODEL = WORKSPACE / "models/yolov8n-rknn-model-zoo-2.3.2/yolov8n-rk3588-i8.rknn"
DETECTOR_LABELS = WORKSPACE / "models/yolov8n-rknn-model-zoo-2.3.2/coco_80_labels_list.txt"
DEMO = WORKSPACE / "vendor-src/rknn-llm-878f936/examples/multimodal_model_demo/deploy/install/demo_Linux_aarch64/demo"
MODEL_DIR = WORKSPACE / "models/qwen3-vl-2b/rkllm-model-zoo-1.2.3"
VISION_MODEL = MODEL_DIR / "qwen3-vl-2b_vision_rk3588.rknn"
LANGUAGE_MODEL = MODEL_DIR / "qwen3-vl-2b-instruct_w8a8_rk3588.hf.rkllm"


class CameraVlmWindow(Gtk.Window):
    def __init__(self):
        super().__init__(title="Orange Pi Qwen3-VL")
        self.set_default_size(1600, 780)
        self.set_border_width(12)
        self.connect("destroy", self.on_destroy)

        self.model_process = None
        self.model_pty = None
        self.keyframe_path = None
        self.preview_process = None
        self.last_frame = None
        self.preview_generation = 0

        root = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=10)
        self.add(root)

        toolbar = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL, spacing=8)
        root.pack_start(toolbar, False, False, 0)

        self.capture_button = Gtk.Button.new_with_label("恢复实时画面")
        self.capture_button.connect("clicked", self.on_capture)
        toolbar.pack_start(self.capture_button, False, False, 0)

        self.clear_button = Gtk.Button.new_with_label("清除对话")
        self.clear_button.connect("clicked", self.on_clear)
        toolbar.pack_start(self.clear_button, False, False, 0)

        self.stop_button = Gtk.Button.new_with_label("停止模型")
        self.stop_button.connect("clicked", self.on_stop_model)
        toolbar.pack_start(self.stop_button, False, False, 0)

        self.status = Gtk.Label(label="准备就绪")
        self.status.set_xalign(0)
        toolbar.pack_start(self.status, True, True, 8)

        paned = Gtk.Paned.new(Gtk.Orientation.HORIZONTAL)
        root.pack_start(paned, True, True, 0)

        image_box = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=8)
        camera_grid = Gtk.Grid(column_spacing=8, row_spacing=6)
        self.image = Gtk.Image()
        self.depth_image = Gtk.Image()
        rgb_frame = Gtk.Frame(label="RGB 物品检测与逐目标测距")
        rgb_frame.add(self.image)
        depth_frame = Gtk.Frame(label="对齐深度图")
        depth_frame.add(self.depth_image)
        camera_grid.attach(rgb_frame, 0, 0, 1, 1)
        camera_grid.attach(depth_frame, 1, 0, 1, 1)
        image_box.pack_start(camera_grid, True, True, 0)
        detection_frame = Gtk.Frame(label="物品识别结果与距离")
        self.detection_label = Gtk.Label(label="正在等待检测结果...")
        self.detection_label.set_xalign(0)
        self.detection_label.set_yalign(0)
        self.detection_label.set_line_wrap(True)
        self.detection_label.set_margin_start(10)
        self.detection_label.set_margin_end(10)
        self.detection_label.set_margin_top(8)
        self.detection_label.set_margin_bottom(8)
        detection_frame.add(self.detection_label)
        image_box.pack_start(detection_frame, False, False, 0)
        paned.pack1(image_box, resize=True, shrink=False)

        chat_box = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=8)
        output_frame = Gtk.Frame(label="Qwen3-VL 回答")
        output_scroll = Gtk.ScrolledWindow()
        self.output = Gtk.TextView()
        self.output.set_editable(False)
        self.output.set_wrap_mode(Gtk.WrapMode.WORD_CHAR)
        output_scroll.add(self.output)
        output_frame.add(output_scroll)
        chat_box.pack_start(output_frame, True, True, 0)

        question_row = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL, spacing=8)
        self.question = Gtk.Entry()
        self.question.set_placeholder_text("输入关于当前画面的问题")
        self.question.connect("activate", self.on_send)
        question_row.pack_start(self.question, True, True, 0)
        self.send_button = Gtk.Button.new_with_label("发送")
        self.send_button.connect("clicked", self.on_send)
        question_row.pack_start(self.send_button, False, False, 0)
        chat_box.pack_start(question_row, False, False, 0)
        paned.pack2(chat_box, resize=True, shrink=False)
        paned.set_position(1110)

        GLib.idle_add(self.start_preview)

    def set_status(self, message):
        self.status.set_text(message)

    def append_output(self, text):
        buffer = self.output.get_buffer()
        buffer.insert(buffer.get_end_iter(), text)
        mark = buffer.create_mark(None, buffer.get_end_iter(), False)
        self.output.scroll_mark_onscreen(mark)
        return False

    def on_capture(self, _button):
        if self.model_process and self.model_process.poll() is None:
            self.set_status("请先停止模型，再刷新画面")
            return
        self.start_preview()

    def start_preview(self):
        self.stop_preview()
        self.preview_generation += 1
        generation = self.preview_generation
        env = os.environ.copy()
        env["LD_LIBRARY_PATH"] = os.pathsep.join(
            [str(REALSENSE_PREFIX / "lib"), str(RKLLM_PREFIX / "lib")]
        )
        env["YOLO_MODEL_PATH"] = str(DETECTOR_MODEL)
        env["YOLO_LABELS_PATH"] = str(DETECTOR_LABELS)
        try:
            self.preview_process = subprocess.Popen(
                [str(PREVIEW_APP)], stdout=subprocess.PIPE, stderr=subprocess.PIPE, env=env
            )
        except Exception as error:
            self.capture_failed(str(error))
            return False
        self.capture_button.set_sensitive(False)
        self.send_button.set_sensitive(False)
        self.set_status("正在启动 640x480@15 实时画面...")
        threading.Thread(target=self.preview_reader, args=(generation,), daemon=True).start()
        threading.Thread(target=self.preview_status_reader, args=(generation,), daemon=True).start()
        return False

    def stop_preview(self):
        self.preview_generation += 1
        process = self.preview_process
        self.preview_process = None
        if process and process.poll() is None:
            process.terminate()
            try:
                process.wait(timeout=3)
            except subprocess.TimeoutExpired:
                process.kill()

    def preview_reader(self, generation):
        frame_size = 640 * 480 * 3
        try:
            while self.preview_process and generation == self.preview_generation:
                packet = self.preview_process.stdout.read(frame_size * 2)
                if len(packet) != frame_size * 2:
                    break
                frame = packet[:frame_size]
                depth_frame = packet[frame_size:]
                self.last_frame = frame
                GLib.idle_add(self.show_live_frame, frame, depth_frame, generation)
        except Exception as error:
            GLib.idle_add(self.capture_failed, str(error))

    def preview_status_reader(self, generation):
        process = self.preview_process
        if not process:
            return
        for raw_line in iter(process.stderr.readline, b""):
            if generation != self.preview_generation:
                break
            line = raw_line.decode("utf-8", errors="replace").strip()
            if line.startswith("detections="):
                GLib.idle_add(self.show_detections, line[len("detections="):])
            elif line.startswith("error="):
                GLib.idle_add(self.capture_failed, line[6:])

    def show_detections(self, payload):
        colors = ["#ff4848", "#50d278", "#4196ff", "#ffc33c", "#d25aeb", "#32d7d7"]
        rows = []
        for item in filter(None, payload.split(";")):
            fields = item.split("|")
            if len(fields) != 8:
                continue
            index, label, confidence, _left, _top, _right, _bottom, distance = fields
            color = colors[(int(index) - 1) % len(colors)]
            distance_text = f"{float(distance):.2f} m" if float(distance) > 0 else "距离不可用"
            rows.append(
                f'<span foreground="{color}"><b>#{index}</b></span>  '
                f'<b>{GLib.markup_escape_text(label)}</b>  '
                f'置信度 {float(confidence):.0%}  距离 <b>{distance_text}</b>'
            )
        self.detection_label.set_markup("\n".join(rows) if rows else "当前画面未识别到物品")
        return False

    def show_live_frame(self, frame, depth_frame, generation):
        if generation != self.preview_generation:
            return False
        pixels = GLib.Bytes.new(frame)
        rgb_pixbuf = GdkPixbuf.Pixbuf.new_from_bytes(
            pixels, GdkPixbuf.Colorspace.RGB, False, 8, 640, 480, 640 * 3
        )
        depth_pixels = GLib.Bytes.new(depth_frame)
        depth_pixbuf = GdkPixbuf.Pixbuf.new_from_bytes(
            depth_pixels, GdkPixbuf.Colorspace.RGB, False, 8, 640, 480, 640 * 3
        )
        self.image.set_from_pixbuf(
            rgb_pixbuf.scale_simple(520, 390, GdkPixbuf.InterpType.BILINEAR)
        )
        self.depth_image.set_from_pixbuf(
            depth_pixbuf.scale_simple(520, 390, GdkPixbuf.InterpType.BILINEAR)
        )
        self.capture_button.set_sensitive(False)
        self.send_button.set_sensitive(True)
        self.set_status("实时画面 640x480@15")
        return False

    def start_capture(self):
        self.capture_button.set_sensitive(False)
        self.send_button.set_sensitive(False)
        self.set_status("正在采集 RGB-D 关键帧...")
        threading.Thread(target=self.capture_worker, daemon=True).start()
        return False

    def capture_worker(self):
        run_id = datetime.datetime.now().strftime("%Y%m%dT%H%M%S")
        run_dir = WORKSPACE / "logs/camera-vlm-gui" / run_id
        image_path = run_dir / "keyframe.ppm"
        run_dir.mkdir(parents=True, exist_ok=True)
        env = os.environ.copy()
        env["LD_LIBRARY_PATH"] = str(REALSENSE_PREFIX / "lib")
        try:
            result = subprocess.run(
                [str(KEYFRAME_APP), "--output", str(image_path)],
                env=env,
                text=True,
                capture_output=True,
                timeout=20,
                check=True,
            )
            depth = "unknown"
            for line in result.stdout.splitlines():
                if line.startswith("keyframe.center_depth_m="):
                    depth = line.split("=", 1)[1]
            GLib.idle_add(self.capture_complete, str(image_path), depth)
        except Exception as error:
            GLib.idle_add(self.capture_failed, str(error))

    def capture_complete(self, image_path, depth):
        self.keyframe_path = pathlib.Path(image_path)
        pixbuf = GdkPixbuf.Pixbuf.new_from_file_at_scale(image_path, 540, 540, True)
        self.image.set_from_pixbuf(pixbuf)
        self.detection_label.set_text("静态关键帧已采集；恢复实时画面后继续逐目标测距")
        self.capture_button.set_sensitive(True)
        self.send_button.set_sensitive(True)
        self.set_status("画面已更新，可以提问")
        return False

    def capture_failed(self, message):
        self.capture_button.set_sensitive(True)
        self.set_status("采集失败")
        self.append_output(f"\n[Camera error] {message}\n")
        return False

    def ensure_model(self):
        if self.model_process and self.model_process.poll() is None:
            return True
        if not self.last_frame:
            self.set_status("请先等待实时画面出现")
            return False
        self.stop_preview()
        run_id = datetime.datetime.now().strftime("%Y%m%dT%H%M%S")
        run_dir = WORKSPACE / "logs/camera-vlm-gui" / run_id
        run_dir.mkdir(parents=True, exist_ok=True)
        self.keyframe_path = run_dir / "keyframe.ppm"
        with self.keyframe_path.open("wb") as stream:
            stream.write(b"P6\n640 480\n255\n")
            stream.write(self.last_frame)
        required = [DEMO, VISION_MODEL, LANGUAGE_MODEL, RKLLM_PREFIX / "lib/librkllmrt.so"]
        missing = [str(path) for path in required if not path.is_file()]
        if missing:
            self.append_output("\n[Missing files]\n" + "\n".join(missing) + "\n")
            return False

        master, slave = pty.openpty()
        env = os.environ.copy()
        env["LD_LIBRARY_PATH"] = str(RKLLM_PREFIX / "lib")
        command = [
            str(DEMO), str(self.keyframe_path), str(VISION_MODEL), str(LANGUAGE_MODEL),
            "128", "4096", "3", "rk3588",
        ]
        self.model_process = subprocess.Popen(
            command, stdin=slave, stdout=slave, stderr=slave, env=env, close_fds=True
        )
        os.close(slave)
        self.model_pty = master
        self.capture_button.set_sensitive(False)
        self.set_status("模型正在加载，问题会在加载后处理")
        threading.Thread(target=self.model_reader, daemon=True).start()
        return True

    def model_reader(self):
        try:
            while True:
                chunk = os.read(self.model_pty, 4096)
                if not chunk:
                    break
                GLib.idle_add(self.append_output, chunk.decode("utf-8", errors="replace"))
        except OSError:
            pass
        finally:
            GLib.idle_add(self.model_stopped)

    def on_send(self, _widget):
        prompt = self.question.get_text().strip()
        if not prompt or not self.ensure_model():
            return
        self.question.set_text("")
        full_prompt = "<image>" + prompt
        os.write(self.model_pty, (full_prompt + "\n").encode("utf-8"))
        self.append_output(f"\n你：{prompt}\n")

    def on_clear(self, _button):
        if self.model_process and self.model_process.poll() is None:
            os.write(self.model_pty, b"clear\n")
        self.output.get_buffer().set_text("")

    def on_stop_model(self, _button):
        self.stop_model()

    def stop_model(self):
        if self.model_process and self.model_process.poll() is None:
            try:
                os.write(self.model_pty, b"exit\n")
                self.model_process.wait(timeout=5)
            except Exception:
                self.model_process.send_signal(signal.SIGTERM)

    def model_stopped(self):
        self.capture_button.set_sensitive(True)
        self.set_status("模型已停止；正在恢复实时画面")
        self.start_preview()
        return False

    def on_destroy(self, _widget):
        self.stop_preview()
        self.stop_model()
        Gtk.main_quit()


if __name__ == "__main__":
    window = CameraVlmWindow()
    window.show_all()
    Gtk.main()
