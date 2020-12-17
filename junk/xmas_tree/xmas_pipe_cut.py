' Plan tree. Visualize in 2D but should be 3D. '

import sys
import matplotlib.pyplot as plt

MINIMUM_SEGMENT = 4 * 6 # 10cm branch is the minimum

def generate_tree(lines, level, base_size, factor, remaining):
    ' Compute material usage and dimensions '
    if remaining <= MINIMUM_SEGMENT:
        return 0
    needed_material = 4 * base_size
    if remaining <= needed_material or needed_material == 0:
        return 0
    print(f'level {level} needs {needed_material:g}, branch of {base_size}')
    lines.append([[-base_size, level], [base_size, level]])
    return 1 + generate_tree(lines,
                             level + 1,
                             base_size * factor,
                             factor,
                             remaining - needed_material)

def main():
    ' duh '

    lines = []
    height = generate_tree(lines=lines, level=0, base_size=25, factor=0.7, remaining=300)

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
