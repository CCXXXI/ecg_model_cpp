#include <torch/script.h>

torch::jit::Module model;

extern "C" auto load_model(const char* const path) -> void {
  model = torch::jit::load(path);
  model.eval();
}

auto forward(double* const data, const int size) -> at::Tensor {
  std::vector<torch::jit::IValue> inputs;
  inputs.emplace_back(torch::from_blob(data, {1, 1, size}, torch::kDouble));

  const torch::NoGradGuard no_grad;
  return model.forward(inputs).toTensor()[0];
}

extern "C" auto forward_argmax(double* const data, const int size,
                               uint8_t* const out) -> void {
  const auto res = forward(data, size).argmax(0).to(torch::kU8);
  memcpy(out, res.data_ptr(), size);
}
