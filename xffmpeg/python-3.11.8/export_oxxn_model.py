import torch
import torch.nn as nn
from torchvision import models

# ================================
# 1. Đường dẫn file
# ================================
PTH_FILE = "D:/x/algorithms_net/xffmpeg/python-3.11.8/faceA_model.pth"
ONNX_FILE = "D:/x/algorithms_net/xffmpeg/python-3.11.8/faceA_model.onnx"

# ================================
# 2. Tạo lại model đúng cấu trúc
# ================================
model = models.resnet18(weights=None)        # KHÔNG dùng pretrained
num_ftrs = model.fc.in_features
model.fc = nn.Linear(num_ftrs, 1)            # giống lúc train

# Load weight
model.load_state_dict(torch.load(PTH_FILE, map_location="cpu"))
model.eval()

# ================================
# 3. Tạo input mẫu để export
# ================================
dummy_input = torch.randn(1, 3, 224, 224)

# ================================
# 4. Export ONNX
# ================================
torch.onnx.export(
    model,
    dummy_input,
    ONNX_FILE,
    input_names=["input"],
    output_names=["output"],
    opset_version=18,
    do_constant_folding=True
)

print("Đã tạo ONNX:", ONNX_FILE)
