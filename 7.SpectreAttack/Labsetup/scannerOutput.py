def filter_output(input_file, output_file):
    with open(input_file, 'rb') as f:
        lines = f.readlines()

    filtered_lines = [line.strip() for line in lines if b'*****' not in line]

    with open(output_file, 'wb') as f:
        f.write(b'\n'.join(filtered_lines))

input_file = 'output.txt'
output_file = 'filtered_output.txt'
filter_output(input_file, output_file)
