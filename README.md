# 🎬 Qt5 Video Player

Простой видеопроигрыватель, реализованный с использованием Qt5 и `QMediaPlayer`. Поддерживает воспроизведение видео, управление ползунком, регулировку громкости и запись фрагмента видео с сохранением в отдельный файл.

---

## 🚀 Возможности

- ▶️ Воспроизведение и пауза
- 🔊 Управление громкостью
- 📂 Открытие видеофайлов (mp4, avi, mkv и др.)
- 🎯 Перемотка по таймлайну
- 🔴 Запись фрагмента видео от текущей позиции до нажатия Стоп
- 💾 Сохранение записанного фрагмента с помощью FFmpeg
- ⏱ Многопоточная запись (через QtConcurrent) — не блокирует интерфейс
- 🟠 Визуальный мигающий индикатор записи

---

## 🛠 Зависимости

- **Qt 5.15+** (`QtMultimedia`, `QtConcurrent`)
- **FFmpeg** — для обрезки и сохранения фрагмента видео
- **GStreamer (Ubuntu)** — необходим для работы `QMediaPlayer`:

### 📦 Установка GStreamer в Ubuntu / Debian:

```bash
sudo apt update
sudo apt install \
  gstreamer1.0-plugins-base \
  gstreamer1.0-plugins-good \
  gstreamer1.0-plugins-bad \
  gstreamer1.0-plugins-ugly \
  gstreamer1.0-libav \
  gstreamer1.0-tools
```

Проверь работу GStreamer:
```bash
gst-play-1.0 your_video.mp4
```

---

## 🧱 Сборка

```bash
git clone https://github.com/your-username/qt5-video-player.git
cd qt5-video-player
qmake
make
./qt5-video-player
```
