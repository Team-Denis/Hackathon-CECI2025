from pyzbar.pyzbar import decode as qr_decode
import cv2
from PIL import Image
import numpy as np

# Load the image in grayscale
image = cv2.imread('res/qr.png', cv2.IMREAD_GRAYSCALE)

# Decode the QR code
decoded_objects = qr_decode(Image.fromarray(image))


# Print the decoded objects
for obj in decoded_objects:
    print('Type:', obj.type)
    print('Data:', obj.data)
    print()