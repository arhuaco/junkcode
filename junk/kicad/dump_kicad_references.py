import sys
import sexpdata


def recursive_find_symbol(tree: list) -> list[str]:
    if not isinstance(tree, list):
        return tree
    if isinstance(tree[0], sexpdata.Symbol):
        if (
            len(tree) >= 2
            and str(tree[0]) == "property"
            and str(tree[1]) == "Reference"
        ):
            reference = tree[2].strip()
            if len(reference):
                print(reference)
    for exp in tree[1:]:
        recursive_find_symbol(exp)


def main(file_name: str):
    tree = sexpdata.loads(open(file_name).read())
    recursive_find_symbol(tree)


if __name__ == "__main__":
    main(sys.argv[1])
