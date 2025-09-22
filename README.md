# Trash-Sorting-Robotic-Arm

A 5-DOF trash-sorting robotic arm that uses a YOLO object detector to classify items from a webcam/USB camera and an ESP32-driven arm (16-ch I²C 12-bit servo driver) to pick & place items into appropriate bins (recyclable / non-recyclable / hazardous). The web UI is built with Streamlit showing the live camera and detection overlay.

https://github.com/user-attachments/assets/178b6c41-1f15-4fc7-aa5b-73d2985e2a01

https://github.com/user-attachments/assets/309ed856-c838-49c0-8e08-598442d9adb2


## Hardware 
- 5-DOF 3D-printed robotic arm
- Motors: 3 × MG996 (high-torque) at critical joints, 2 × SG90 for claw & head
- 16-channel 12-bit I²C servo driver (PCA9685 or equivalent)
- ESP32 microcontroller controlling servo motions (serial receiver)
- USB Webcam for detection stream

## Software Stack & Config

- Streamlit app: app.py — page layout + calls helper to load model & stream camera. 
- Detection + control logic: helper.py — loads YOLO model, runs live predictions, classifies detected items into RECYCLABLE / NON_RECYCLABLE / HAZARDOUS and sends byte commands over serial (default /dev/ttyUSB0, 9600bps) to the microcontroller. It also contains routines to wait until the detected object is cleared before next detection. 
- Settings: settings.py — model path (weights/best.pt), webcam index (0) and lists of class → category mapping. 
- Training: train.py — YOLOv8 training example (yolov8n), validation, and ONNX export. Update path to your dataset YAML when re-training. 
- Microcontroller sketch: recycle2.ino — contains servo routines used by the ESP32/Arduino to perform pick/place motions. 
