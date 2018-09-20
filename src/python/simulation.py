from datetime import datetime
import numpy as np

initialized = False

# Any live cell with fewer than two live neighbors dies, as if by under population.
# Any live cell with two or three live neighbors lives on to the next generation.
# Any live cell with more than three live neighbors dies, as if by overpopulation.
# Any dead cell with exactly three live neighbors becomes a live cell, as if by reproduction.


def alive_neighbors(board: list, width: int, height: int, position: int):
    neighbors = []

    if position % width > 0:
        neighbors.append(position - 1)  # left
    if position % width < width - 1:
        neighbors.append(position + 1)  # right

    if position >= width:
        neighbors.append(position - width)  # top
    if position < width * height - width:
        neighbors.append(position + width)  # bottom

    if position < width * height - width and position % width > 0:
        neighbors.append(position + width - 1)  # top left
    if position < width * height - width and position % width < width - 1:
        neighbors.append(position + width + 1)  # top right

    if position >= width and position % width > 0:
        neighbors.append(position - width - 1)  # bottom left
    if position >= width and position % width < width - 1:
        neighbors.append(position - width + 1)  # bottom right

    return board[position], sum(map(int, map(lambda index: board[index], neighbors)))


def flip(alive, alive_counter):
    if not alive:
        if alive_counter == 3:
            return True
        else:
            return False
    else:
        if alive_counter < 2:
            return False
        elif 2 <= alive_counter <= 3:
            return True
        else:
            return False


def update_numpy(board: np.ndarray, width: int, height: int):
    Z = np.reshape(board, (width, height)).astype(int)

    N = (Z[0:-2, 0:-2] + Z[0:-2, 1:-1] + Z[0:-2, 2:] +
         Z[1:-1, 0:-2] + Z[1:-1, 2:] +
         Z[2:, 0:-2] + Z[2:, 1:-1] + Z[2:, 2:])

    # Apply rules
    birth = (N == 3) & (Z[1:-1, 1:-1] == 0)
    survive = ((N == 2) | (N == 3)) & (Z[1:-1, 1:-1] == 1)
    Z[...] = 0
    Z[1:-1, 1:-1][birth | survive] = 1

    return Z


def update(board: list, width: int, height: int) -> list:
    return list(
        map(lambda arr: flip(arr[0], arr[1]),
            map(lambda i: alive_neighbors(board, width, height, i),
                range(len(board)))))


def update_cell(cell, neighbors):
    # cell: tuple(species, strength), neighbors: dict(left, right, top, bottom)
    # ->
    # cell: tuple(species, strength), neighbors: dict(left, right, top, bottom)

    new_neighbors = dict([("left", None), ("right", None),
                          ("top", None), ("bottom", None)])
    if cell[1] >= 10:
        cell = (cell[0], cell[1] // 2)
        new_neighbors["right"] = tuple(cell)
    # print(cell)
    # if cell[1] >= 10:
    #     new_species = (cell[0] + 1) % 4
    #     if new_species == 0:
    #         new_species += 1
    #     cell = (new_species, cell[1] - 1)
    return cell, new_neighbors
