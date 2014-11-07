import numpy as np
import pyopencl as cl
import sys

N = 256

a_np = np.zeros((N,),  dtype=np.int32)

a_np[N // 2] = 1

ctx = cl.create_some_context()
queue = cl.CommandQueue(ctx)

mf = cl.mem_flags
a_g = cl.Buffer(ctx, mf.READ_ONLY | mf.COPY_HOST_PTR, hostbuf=a_np)


prg = cl.Program(ctx, '''
__kernel void ca_1d_r2_step(__global const int *input, __global int *output, const int N, const unsigned int rule) {
  int gid = get_global_id(0);
  int lookup = input[(gid - 2 + N) % N] << 4 |  input[(gid - 1 + N) % N] << 3 | input[(gid)] << 2 | input[(gid + 1) % N] << 1 | input[(gid + 2) % N];
  output[gid] = (rule & (1 << lookup)) >> lookup;
}
''').build()

res_g = cl.Buffer(ctx, mf.WRITE_ONLY, a_np.nbytes)
res_np = np.zeros((N,),  dtype=np.int32)
for _ in range(64):
  prg.ca_1d_r2_step(queue, a_np.shape, None, a_g, res_g, np.int32(N), np.uint32(3133097257)) # It's important to send the rule as unsigned.
  cl.enqueue_copy(queue, res_np, res_g)
  s = ''
  for i in range(len(res_np)):
    s += 'O' if res_np[i] else ' '
  print(s)
  a_g, res_g = res_g, a_g
# FIX
#res_np = np.empty_like(a_np)
# Do not zero.

# Check on CPU with Numpy:
