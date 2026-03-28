import argparse
import re
import sys

def get_tower_temps(gcode_filepath):
    # Regex to find the original temperatures
    temp_pattern = re.compile(r"^M104 S(\d+\.?\d*)\s*;AutoTowersGenerator")
    temps = []
    with open(gcode_filepath, 'r') as f:
        for line in f:
            match = temp_pattern.search(line)
            if match:
                temps.append(float(match.group(1)))
    return temps

def main():
    # 1. Setup Argparse for positional arguments
    parser = argparse.ArgumentParser(description="Modify AutoTowers G-code temperatures.")
    parser.add_argument('in_file', help="Input G-code file (e.g., input.gcode)")
    parser.add_argument('out_file', help="Output G-code file (e.g., output.gcode)")
    args = parser.parse_args()

    # 2. Extract existing temps from the input file
    try:
        original_temps = get_tower_temps(args.in_file)
    except FileNotFoundError:
        print(f"Error: Could not find input file '{args.in_file}'")
        sys.exit(1)

    if not original_temps:
        print("No AutoTowers temperature commands found. Exiting.")
        sys.exit(0)

    # 3. Prompt user for overrides
    new_temps = []
    cut_section = -1

    print("\n--- Temperature Override ---")
    print("Enter a new temperature, or press Enter to keep the original.")
    print("Type '0' to cut the tower at that section.\n")

    for i, temp in enumerate(original_temps):
        section_num = i + 1
        user_input = input(f"Section {section_num} (Current: {temp}°C): ").strip()
        
        if user_input == '0':
            cut_section = section_num
            print(f"\n[!] Tower will be safely cut at the start of Section {cut_section}.")
            break
        elif user_input == '':
            # User just hit enter, keep original
            new_temps.append(temp)
        else:
            try:
                new_temps.append(float(user_input))
            except ValueError:
                print(f"Invalid input. Keeping original {temp}°C.")
                new_temps.append(temp)

    # 4. Process and write the new file
    # Regex to find where a section starts and where the temp commands are
    section_pattern = re.compile(r"^;AutoTowersGenerator: Starting tower section number (\d+)")
    m104_pattern = re.compile(r"^(M104 S)(\d+\.?\d*)(\s*;AutoTowersGenerator.*)")
    m109_pattern = re.compile(r"^(M109 S)(\d+\.?\d*)(\s*;AutoTowersGenerator.*)")

    current_section = 0
    print(f"\nWriting to '{args.out_file}'...")
    
    with open(args.in_file, 'r') as f_in, open(args.out_file, 'w') as f_out:
        for line in f_in:
            # Check if we are entering a new tower section
            section_match = section_pattern.search(line)
            if section_match:
                current_section = int(section_match.group(1))
                
                # If this is where the user wants to cut, inject safety G-code and stop reading
                if current_section == cut_section:
                    f_out.write("\n; --- TOWER CUT BY PYTHON SCRIPT ---\n")
                    f_out.write("M104 S0 ; Turn off hotend\n")
                    f_out.write("M140 S0 ; Turn off bed\n")
                    f_out.write("G91 ; Relative positioning\n")
                    f_out.write("G1 Z10 E-2 F2400 ; Retract and lift Z\n")
                    f_out.write("G90 ; Absolute positioning\n")
                    f_out.write("G28 X Y ; Home X and Y to clear the print\n")
                    f_out.write("M84 ; Disable steppers\n")
                    break

            # If we are in a valid section, replace temps if necessary
            if 1 <= current_section <= len(new_temps):
                target_temp = new_temps[current_section - 1]
                
                # Overwrite M104 (Set Temp)
                m104_match = m104_pattern.search(line)
                if m104_match:
                    line = f"{m104_match.group(1)}{target_temp}{m104_match.group(3)}\n"
                
                # Overwrite M109 (Wait for Temp)
                m109_match = m109_pattern.search(line)
                if m109_match:
                    line = f"{m109_match.group(1)}{target_temp}{m109_match.group(3)}\n"

            f_out.write(line)

    print("Success! Your custom tower is ready to print.")

if __name__ == "__main__":
    main()

