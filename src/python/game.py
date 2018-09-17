import numpy as np
# import simulation


# def my_callback(value):
#     print("I've been called!", value)
#     return None


# simulation.system("ls -la")
# simulation.set_callback(my_callback)
# simulation.call_callback()

def hello():
    print("Hello World!")


def main():
    arr = np.array([
        [0, 0, 0, 0, 0],
        [0, 1, 0, 0, 0],
        [0, 0, 0, 0, 0],
        [0, 1, 0, 0, 0],
        [0, 0, 0, 0, 0],
    ])
    arr1 = arr[0:-2, 0:-2]
    arr2 = arr[0:-2, 1:-1]
    print(arr1)
    print(arr2)
    print(arr1 + arr2)


if __name__ == '__main__':
    main()
