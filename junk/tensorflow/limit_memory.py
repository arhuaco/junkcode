# Using it for a single GPU with these docker flags : --runtime=nvidia -e NVIDIA_VISIBLE_DEVICES=$1
# https://stackoverflow.com/questions/34199233/how-to-prevent-tensorflow-from-allocating-the-totality-of-a-gpu-memory
# I haven't checked the easiest way to do this, I just want to keep the answer.
# The API is experimental _today_ anyway.
def limit_memory():
    ' limit GPU memory '
    gpus = tf.config.experimental.list_physical_devices('GPU')
    if gpus:
      try:
        # Currently, memory growth needs to be the same across GPUs
        for gpu in gpus:
          tf.config.experimental.set_memory_growth(gpu, True)
        logical_gpus = tf.config.experimental.list_logical_devices('GPU')
        print(len(gpus), "Physical GPUs,", len(logical_gpus), "Logical GPUs")
      except RuntimeError as e:
          # Memory growth must be set before GPUs have been initialized
          print(e)
limit_memory()
