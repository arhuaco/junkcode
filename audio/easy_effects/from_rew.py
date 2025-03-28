""" import from REW to easyeffects """

import csv
import json
import sys

def read_peq_table_from_file(file_path):
    """ From file to imported data """
    peq_table = []
    with open(file_path, mode="r", encoding="ascii") as file:
        csv_reader = csv.reader(file, delimiter="\t")
        # Skip the header if it exists
        next(csv_reader, None)
        for row in csv_reader:
            # Extract and convert the relevant columns
            try:
                peq_table.append(
                    {
                        "Frequency(Hz)": float(row[4].strip()),
                        "Gain(dB)": float(row[5].strip()),
                        "Q": float(row[6].strip()),
                        "Bandwidth(Hz)": float(row[7].strip()),
                    }
                )
            except ValueError as e:
                print(f"Error parsing row {row}: {e}")
    return peq_table


def generate_easyeffects_json(peq_table):
    """ format the output """
    equalizer = {
        "balance": 0.0,
        "bypass": False,
        "input-gain": 0.0,
        "left": {},
        "mode": "IIR",
        "num-bands": len(peq_table),
        "output-gain": 0.0,
        "pitch-left": 0.0,
        "pitch-right": 0.0,
        "right": {},
        "split-channels": False,
    }

    for i, band in enumerate(peq_table):
        band_data = {
            "frequency": band["Frequency(Hz)"],
            "gain": band["Gain(dB)"],
            "mode": "RLC (BT)",
            "mute": False,
            "q": band["Q"],
            "slope": "x1",
            "solo": False,
            "type": "Bell",
            "width": band["Bandwidth(Hz)"],
        }
        equalizer["left"][f"band{i}"] = band_data
        equalizer["right"][f"band{i}"] = band_data.copy()

    output = {
        "output": {
            "blocklist": [],
            "equalizer#0": equalizer,
            "filter#0": {
                "balance": 0.0,
                "bypass": False,
                "equal-mode": "IIR",
                "frequency": 30.0,
                "gain": 0.04,
                "input-gain": -16.0,
                "mode": "RLC (BT)",
                "output-gain": 0.0,
                "quality": 0.0,
                "slope": "x2",
                "type": "High-pass",
                "width": 4.0,
            },
            "plugins_order": ["filter#0", "equalizer#0"],
        }
    }

    return json.dumps(output, indent=4)


def main():
    """ Main """
    peq_table = read_peq_table_from_file(sys.argv[1])
    print(generate_easyeffects_json(peq_table))


if __name__ == "__main__":
    main()
