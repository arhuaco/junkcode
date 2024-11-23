" Quick script to dump references "

import sys
import sexpdata


def print_refs(tree: list) -> None:
    "print all refs, also for children"
    if not isinstance(tree, list):
        return
    if isinstance(tree[0], sexpdata.Symbol):
        if (
            len(tree) >= 2
            and str(tree[0]) == "property"
            and str(tree[1]) == "Reference"
        ):
            reference = tree[2].strip()
            if reference:
                print(reference)
    for exp in tree[1:]:
        print_refs(exp)


def main(file_name: str):
    "print all refs"
    with open(file_name, encoding="utf-8") as in_f:
        print_refs(sexpdata.loads(in_f.read()))


if __name__ == "__main__":
    main(sys.argv[1])
