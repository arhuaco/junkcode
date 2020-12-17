' Plan tree. Visualize in 2D but should be 3D. '

import sys
import matplotlib.pyplot as plt

MINIMUM_SEGMENT = 5 # 10cm branch is the minimum

def generate_tree(lines, level, base_size, factor, remaining):
    ' Compute material usage and dimensions '
    if base_size <= MINIMUM_SEGMENT:
        return 0
    needed_material = 4 * base_size
    print(f'level{level+1}:{base_size:.2g}, total_level:{base_size*4:g}') # x4 branches
    if remaining <= needed_material:
        return 0
    lines.append([[-base_size, level], [base_size, level]])
    return 1 + generate_tree(lines,
                             level + 1,
                             base_size * factor,
                             factor,
                             remaining - needed_material)
def main():
    ' duh '

    lines = []
    height = generate_tree(lines=lines, level=0, base_size=25, factor=0.7445, remaining=600)

    plt.title('Tree')
    plt.xlabel('Width')
    plt.ylabel('Height')
    for line in lines:
        plt.plot([line[0][0], line[1][0]], [line[0][1], line[1][1]], '-g', linewidth=2)
    plt.plot([[0, 0], [0, 0]], [[0, 0], [0, height - 1]], '-g', linewidth=2)
    plt.show()


if __name__ == '__main__':
    main()
    sys.exit(0)
