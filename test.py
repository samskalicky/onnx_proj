import torch
import onnx
import ctypes

pylib = ctypes.CDLL('libpylib.so')
torch.ops.load_library('libpylib.so')

def load_lib(path):
  pylib.load(ctypes.create_string_buffer(path.encode('utf-8')))

load_lib('libMatMul.so')

######################################
# create model
class MLP(torch.nn.Module):
  def __init__(self):
    super().__init__()
    self.layers = torch.nn.Sequential(
      torch.nn.Linear(5, 64, bias=False)
    )

  def forward(self, x):
    return self.layers(x)
#######################################

# instantiate model
model = MLP()

# test inference
input = torch.ones((5))
output = model(input)

# export to onnx
torch.onnx.export(model,
                  input,
                  "mlp.onnx",
                  input_names=["input_0"],
                  output_names=["output_0"])

# load onnx model in Python
onnx_model = onnx.load('mlp.onnx')
#print(onnx_model)

# create a dictionary of tensors mapped to names to use while executing model
# add inputs to tensor dict
tensors = {"input_0":input}

# add params to tensor dict
for param in onnx_model.graph.initializer:
  tensors[param.name] = torch.tensor(onnx.numpy_helper.to_array(param))

##############
# ONNX nodes are in topological order, so we can just loop over them and execute without considering edge/dependencies
##############
for node in onnx_model.graph.node:
  #print(node)

  # create list of input tensors for the node/op
  inputs = [tensors[n] for n in node.input]

  # actually execute the op
  if node.op_type == "MatMul":
    #onnx matMul uses transpose of weight
    inputs[1] = torch.transpose(inputs[1], 0,1)

    # execute op by calling PyTorch function (for testing)
    outputs = torch.nn.functional.linear(*inputs, bias=None)

    outputs = torch.ops.my_ops.my_exec(inputs, node.op_type)

  # make outputs of node/op a list
  if not isinstance(outputs,(list,tuple)):
    outputs = [outputs]

  # add outputs to tensor map
  for idx,n in enumerate(node.output):
    tensors[n] = outputs[idx]

# print final model output
print(tensors['output_0'])