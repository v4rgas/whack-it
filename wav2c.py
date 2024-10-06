import sys

# Function to convert a WAV file to C uint8_t array and optionally write to a file
def wav_to_c_array(wav_file, output_file=None):
    try:
        # Open the wav file in binary mode
        with open(wav_file, "rb") as f:
            byte_data = f.read()

        # Prepare the C array as a string
        c_array = "#include <stdint.h>\n"
        c_array += f'const uint8_t wav_data[] = {{\n'

        # Process and format each byte as hexadecimal
        for i, byte in enumerate(byte_data):
            if i % 12 == 0:
                c_array += "\n   "  # Add new line every 12 bytes for readability
            c_array += f"0x{byte:02X}, "

        # Close the array
        c_array += "\n};\n"
        # Add the length of the array
        c_array += f"const unsigned int wav_data_len = {len(byte_data)};\n"

        # Output to file if provided, otherwise print to console
        if output_file:
            with open(output_file, "w", encoding="utf-8") as f_out:
                f_out.write(c_array)
            print(f"Output written to {output_file}")
        else:
            print(c_array)

    except FileNotFoundError:
        print(f"Error: File '{wav_file}' not found.")
    except Exception as e:
        print(f"Error processing file: {e}")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python wav_to_c_array.py <wav_file> [output_file]")
    else:
        wav_file = sys.argv[1]
        output_file = sys.argv[2] if len(sys.argv) > 2 else None
        wav_to_c_array(wav_file, output_file)