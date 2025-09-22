from ultralytics import YOLO
import time
import streamlit as st
import cv2
import settings
import serial

SERIAL_PORT = '/dev/ttyUSB0'  
BAUD_RATE = 9600
try:
    arduino = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
    time.sleep(2)
    print("[INFO] Arduino connected.")
except Exception as e:
    arduino = None
    print(f"[ERROR] Arduino not connected: {e}")


arm_is_moving = False
last_action_time = None
startup_delay_done = False

def load_model(model_path):
    return YOLO(model_path)

def classify_waste_type(detected_items):
    recyclable = set(detected_items) & set(settings.RECYCLABLE)
    non_recyclable = set(detected_items) & set(settings.NON_RECYCLABLE)
    hazardous = set(detected_items) & set(settings.HAZARDOUS)
    return recyclable, non_recyclable, hazardous

def play_webcam(model):
    global arm_is_moving, last_action_time, startup_delay_done

    source_webcam = settings.WEBCAM_PATH
    if st.button('Detect Objects'):
        try:
            vid_cap = cv2.VideoCapture(source_webcam)
            st_frame = st.empty()

            if 'object_detected' not in st.session_state:
                st.session_state['object_detected'] = None

            cam_start_time = time.time()

            while vid_cap.isOpened():
                success, frame = vid_cap.read()
                if not success:
                    break

                current_time = time.time()
                st_frame.image(frame, channels="BGR")  # Always keep camera live

                # Wait 3s on startup before first prediction
                if not startup_delay_done and current_time - cam_start_time < 10:
                    continue
                else:
                    startup_delay_done = True

                # Wait 3s after Arduino completes
                if last_action_time and (current_time - last_action_time < 10):
                    continue

                # Skip prediction if arm is executing
                if arm_is_moving:
                    continue

                # Skip detection if waiting for object to clear
                if st.session_state['object_detected']:
                    continue

                # Now predict
                result = model.predict(frame, conf=0.6)[0]
                names = model.names
                detected_classes = set([names[int(c)] for c in result.boxes.cls])
                print(f"[DEBUG] Detected: {detected_classes}")

                rec, nonrec, haz = classify_waste_type(detected_classes)

                if rec:
                    _trigger_waste_type(st_frame, model, result, vid_cap, 'recyclable', b'1', rec)
                elif nonrec:
                    _trigger_waste_type(st_frame, model, result, vid_cap, 'non_recyclable', b'2', nonrec)
                elif haz:
                    _trigger_waste_type(st_frame, model, result, vid_cap, 'hazardous', b'3', haz)

        except Exception as e:
            st.sidebar.error("Error loading video: " + str(e))

def _trigger_waste_type(st_frame, model, result, vid_cap, category, command_byte, detected_items):
    global arm_is_moving, last_action_time
    st.session_state['object_detected'] = category
    st.sidebar.success(f"{category.replace('_',' ').title()} detected: {', '.join(detected_items)}")
    st_frame.image(result.plot(), channels="BGR")
    _send_to_arduino(command_byte)
    _wait_until_object_disappears(model, vid_cap, st_frame, detected_items)
    st.session_state['object_detected'] = None
    last_action_time = time.time()

def _send_to_arduino(command_byte):
    global arm_is_moving
    arm_is_moving = True
    if arduino:
        try:
            print(f"[INFO] Sending command: {command_byte}")
            arduino.write(command_byte)
            arduino.flush()
            time.sleep(6)  # Allow full routine
            print("[INFO] Arduino routine complete.")
        except Exception as e:
            print(f"[ERROR] Failed to send to Arduino: {e}")
    else:
        print("[ERROR] Arduino not initialized.")
    arm_is_moving = False

def _wait_until_object_disappears(model, vid_cap, st_frame, items_to_track):
    names = model.names
    while True:
        success, frame = vid_cap.read()
        if not success:
            break
        resized = cv2.resize(frame, (640, int(640 * (9 / 16))))
        result = model.predict(resized, conf=0.6)[0]
        current_classes = set([names[int(c)] for c in result.boxes.cls])
        st_frame.image(result.plot(), channels="BGR")
        if not current_classes & set(items_to_track):
            st.sidebar.info("Object cleared. Ready for next detection.")
            break
