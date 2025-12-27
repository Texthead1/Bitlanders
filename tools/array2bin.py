import re
import sys

def c_array_to_bin(c_array, output_file):
    num = c_array.index("=")
    c_array = c_array[num:]
    numbers = re.findall(r'0x[0-9A-Fa-f]+|\d+', c_array)

    try:
        byte_data = bytes(int(n, 0) for n in numbers)
    except ValueError as e:
        print(f"Error parsing numbers: {e}")
        sys.exit(1)

    with open(output_file, "wb") as f:
        f.write(byte_data)

    print(f"Binary file '{output_file}' created successfully. ({len(byte_data)} bytes)")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python array2bin.py <input_file> [output_file]")
        sys.exit(1)

    input_file = sys.argv[1]
    input_file = input_file.strip('"')
    output_file = sys.argv[2] if len(sys.argv) > 2 else "output.bin"

    with open(input_file, "r") as f:
        c_array = f.read()

    c_array_to_bin(c_array, output_file)