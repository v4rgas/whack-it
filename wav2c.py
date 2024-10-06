import sys

def read_wav_file(wav_file):
    """Reads the WAV file and returns its byte data."""
    try:
        with open(wav_file, "rb") as f:
            return f.read()
    except FileNotFoundError:
        print(f"Error: File '{wav_file}' not found.")
        raise
    except Exception as e:
        print(f"Error reading file: {e}")
        raise

def generate_c_array(byte_data):
    """Generates the C array string from byte data."""
    c_array = (
        "#include <stdint.h>\n"
        "const uint8_t wav_data[] = {\n"
    )

    c_array += f"const unsigned int wav_data_len = {len(byte_data)};\n"

    for i, byte in enumerate(byte_data):
        if i % 12 == 0:
            c_array += "\n   "
        c_array += f"0x{byte:02X}, "

    c_array += "\n};\n"
    
    return c_array

def write_c_array_to_file(c_array, output_file):
    """Writes the generated C array to an output file."""
    try:
        with open(output_file, "w", encoding="utf-8") as f_out:
            f_out.write(c_array)
        print(f"Output written to {output_file}")
    except Exception as e:
        print(f"Error writing to file: {e}")
        raise

def convert_wav_to_c_array(wav_file, output_file=None):
    """Main function to convert a WAV file to a C array and optionally output to a file."""
    try:
        byte_data = read_wav_file(wav_file)
        c_array = generate_c_array(byte_data)

        if output_file:
            write_c_array_to_file(c_array, output_file)
        else:
            print(c_array)

    except Exception as e:
        print(f"Error processing file: {e}")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python script.py <wav_file> [output_file]")
    else:
        wav_file = sys.argv[1]
        output_file = sys.argv[2] if len(sys.argv) > 2 else None
        convert_wav_to_c_array(wav_file, output_file)
