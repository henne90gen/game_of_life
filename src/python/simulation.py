from datetime import datetime
initialized = False

# Any live cell with fewer than two live neighbors dies, as if by under population.
# Any live cell with two or three live neighbors lives on to the next generation.
# Any live cell with more than three live neighbors dies, as if by overpopulation.
# Any dead cell with exactly three live neighbors becomes a live cell, as if by reproduction.

pattern = [
    [0, 0, 1, 0, 0],
    [0, 1, 1, 1, 0],
    [1, 1, 1, 1, 1],
    [],
    [],
    [],
    [],
    [],
    [],
    [1, 1, 1, 1, 1],
    [0, 1, 1, 1, 0],
    [0, 0, 1, 0, 0],
]


def update(board: list, width: int, height: int) -> list:
    global initialized
    if not initialized:
        initialized = True
        start_row = 5
        start_col = 5
        for row, row_list in enumerate(pattern):
            for col, cell in enumerate(row_list):
                board[(start_row + row) * width + start_col + col] = cell
        return board

    def alive_neighbors(position):
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

    def flip(args):
        alive, alive_counter = args
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

    return list(map(flip, map(alive_neighbors, range(len(board)))))
