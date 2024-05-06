# Description

## Pre-Requisites
- pip install torch
- pip install onnx
- pip install ninja
  - after installing, make sure to set PATH to the `ninja` executable

## Example steps

1. Build custom kernel library `make`
2. Build pylib library `python build.py`
3. Run example `python test.py`