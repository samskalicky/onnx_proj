import os
import torch
from torch.utils import cpp_extension

cpp_extension.load(
    name='libpylib',
    sources=['pylib.cpp'],
    is_python_module=False,
    build_directory=os.getcwd()
)