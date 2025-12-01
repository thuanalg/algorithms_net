import onnxruntime as ort
import numpy as np
from torchvision import datasets, transforms
from torch.utils.data import DataLoader
from PIL import Image

ONNX_FILE = "D:/x/algorithms_net/xffmpeg/python-3.11.8/faceA_model.onnx"

transform = transforms.Compose([
    transforms.Resize((224, 224)),
    transforms.ToTensor(),
    transforms.Normalize([0.5]*3, [0.5]*3),
])

dataset = datasets.ImageFolder("C:/Users/DEll/Desktop/test", transform=transform)
loader = DataLoader(dataset, batch_size=1, shuffle=False)

# Load ONNX
session = ort.InferenceSession(ONNX_FILE, providers=['CPUExecutionProvider'])

correct = 0
total = 0

for img, label in loader:
    img_np = img.numpy().astype(np.float32)

    output = session.run(None, {"input": img_np})
    pred = (1 / (1 + np.exp(-output[0])) > 0.5).astype(np.float32)

    if pred[0][0] == label.item():
        correct += 1
    total += 1

print("ONNX accuracy:", correct / total * 100, "%")
