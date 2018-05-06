import numpy as np
import matplotlib.pyplot as plt

def get_cities():
    hashmap = {}
    f_in = open("../input/coords.txt", "r")
    next(f_in)

    for i,line in enumerate(f_in):
        split = line.split()
        hashmap[i] = np.array([float(split[0]),float(split[1])])

    f_in.close()
    return hashmap

def get_route():
    f_in = open("../output/best_route.txt", "r")
    line = f_in.readline()
    split = line.split()[1:]
    f_in.close()

    for i in range(len(split)):
        split[i] = int(split[i])

    return split

def dist(v1, v2):
    return np.linalg.norm(v1-v2)

def weight(cities, route):
    dist_sum = 0
    n = len(route)

    for i in range(n):
        dist_sum += dist(cities[route[i]], cities[route[(i+1)%n]])

    return dist_sum

def get_arrays(cities, route):
    x = np.array([])
    y = np.array([])

    for i in range(len(route)):
        x = np.append(x, cities[route[i]][0])
        y = np.append(y, cities[route[i]][1])

    x = np.append(x, cities[route[0]][0])
    y = np.append(y, cities[route[0]][1])

    return x,y

def plot_route():
    cities = get_cities()
    route = get_route()
    x,y = get_arrays(cities, route)

    plt.title("Best route found = %.2f"%weight(cities, route))
    plt.plot(x,y)
    plt.savefig("../output/best_route")
    plt.show()

plot_route()
