import torch
import matplotlib.pyplot as plt
from datetime import datetime
import os
from picamera2 import Picamera2


absolute_path = os.path.dirname(__file__)
YOLOV5_DIR = os.path.join(absolute_path, 'yolov5')
WEIGHT_FILE_PATH = os.path.join(absolute_path, 'best.pt')
RPI_IP = '192.168.9.9'
MJPEG_STREAM_URL = 'http://' + RPI_IP + '/html/cam_pic_new.php'

class Snapper:

    # Snaps and saves image with RPi. Returns filename in string
    def retrieve_img():
        # captures an image from the RPi camera
        picam2 = Picamera2()
        # Save file
        filename = (str(datetime.now()) + '.jpg').replace(':','')
        picam2.start()
        picam2.capture_file(filename)
        #picam2.start_and_capture_file(name=filename,delay=0) # this does not work without HDMI connected, ignore
        picam2.stop()
        picam2.close()

        return filename

    def apply_model(img):

        # From local
        model = torch.hub.load(YOLOV5_DIR, 'custom', path=WEIGHT_FILE_PATH, source='local')
        # From yolov5 repo online
        # model = torch.hub.load('ultralytics/yolov5', 'custom', path=WEIGHT_FILE_PATH)

        results = model(img)

        # Annotate detection on model
        fig, ax = plt.subplots(figsize=(16, 12))
        ax.imshow(results.render()[0])
        plt.show()

        # Saves image with bounding box
        results.save()

        # src: https://github.com/ultralytics/yolov5/issues/8824
        objects_detected = results.pandas().xyxy[0]
        print('objects detected', objects_detected.head())

        # To handle if no object detected
        if objects_detected.empty:
            return None
        
        # Get object with highest confidence
        index_with_highest_confidence = objects_detected['confidence'].idxmax()
        print('index with highest conf: ', index_with_highest_confidence)
        result = objects_detected['name'][index_with_highest_confidence]

        print("Object detected: ", result)

        return result

    ### API exposed to ALGO team
    # Snaps photo from RPi webcam
    # Returns detected object
    def snap_and_identify():
        # snap and save photo
        print("Snapping photo...")
        filename = Snapper.retrieve_img()
        print(filename)
        # retrieve photo and apply the model
        print("Applying model...")
        result = Snapper.apply_model(filename)
        print(result)

        return result

# For testing purposes
# apply_model('bullseye.jpg')
# download_model()
# snap_and_identify()
# test_detect()
