from random import shuffle
from queue import PriorityQueue
import networkx as nx
import matplotlib.pyplot as plt


SCOUTS_BEE = 10
BEST_SCOUTS_BEE = 4
RANDOM_SCOUTS_BEE = SCOUTS_BEE - BEST_SCOUTS_BEE
FORAGERS_BEE = 60
BEST_FORAGERS_BEE = 25
RANDOM_FORAGERS_BEE = 5


class Bee:
    def __init__(self,color_node,f):
        self.f = f
        self.color_node = color_node
        

    def __lt__(self, other):
        return self.f.__lt__(other.f)

    def __repr__(self):
        return f"\nChromatic number: {self.f} Coloring: {self.color_node}"

# створення випадкового графу
def create_graph(nodes,max_power,min_power):
    g = nx.watts_strogatz_graph(nodes, max_power, min_power)
    graph = {}
    for item in g.edges:
        node, neighbour = item
        node += 1
        neighbour += 1
        if node not in graph:
            graph[node] = []
        if neighbour not in graph:
            graph[neighbour] = []
        graph[node].append(neighbour)
        graph[neighbour].append(node)
    return graph,g
# намалювати граф
def show_graph(color_node,graph):
    colors = ['red', 'blue', 'yellow', 'lime', 'orange', 'black', 'green', 'grey', 'purple', 'pink','brown','maroon','aqua','azure','teal','wheat','navy','salmon']
    new_colors=[]
    for item in list(color_node.values()):
        new_colors.append(colors[item-1])
    nx.draw(graph, with_labels=True,node_color=list(new_colors))
    plt.show() 
# початкове розфарбування графа
def greedy(graph):
    color_node = {node: 0 for node in graph.keys()}
    graph = list(graph.items())
    shuffle(graph)
    number_color = 0
    for node, neighbours in graph:
        for color in range(1, 100):
            if not check_neighbour_color(color, neighbours, color_node):
                color_node[node] = color
                if color > number_color:
                    number_color += 1
                break
    return color_node, number_color
# Перевірити колір вершини на співпадіння кольорів вершин її сусідів
def check_neighbour_color(color, neighbours, color_node):
    if neighbours:
        for neighbour in neighbours:
            if neighbour in color_node and color_node[neighbour] == color:
                return True
    return False


def create_scouts(graph, bee_scouts, n):
    while bee_scouts.qsize() < n:
        scout = Bee(*greedy(graph))
        if scout not in bee_scouts.queue:
            bee_scouts.put(scout)



# Основна частина бджолинного алгоритму
def bee_optimization(bee_scout, graph, bee_local_foragers):
    queue = []

    for node, neighbours in sorted(list(graph.items()), key=lambda x: len(x[1]), reverse=True):
        if len(queue) >= bee_local_foragers:
            break
        for neighbour in neighbours:
            queue.append((node, neighbour))

    results = []

    for node, neighbour in queue:
        new_color_node = dict(bee_scout.color_node)
        new_color_node[neighbour], new_color_node[node] = new_color_node[node], new_color_node[neighbour]

        if check_neighbour_color(new_color_node[node], graph[node], new_color_node) or \
                check_neighbour_color(new_color_node[neighbour], graph[neighbour], new_color_node):
            continue
        else:
            for color in range(1, bee_scout.f + 1):
                if not check_neighbour_color(color, graph[neighbour], new_color_node):
                    new_color_node[neighbour] = color
                    results.append(Bee(new_color_node, len(set(new_color_node.values()))))

    return min(results, key=lambda x: x.f) if results else bee_scout

def local_search(graph, bee_new_scouts, bee_scouts, n):
    for scout in bee_scouts:
        scout = bee_optimization(scout, graph, n)
        bee_new_scouts.put(scout)

def get_best_scouts(bee_scouts):
    best = []
    for i in range(BEST_SCOUTS_BEE):
        best.append(bee_scouts.get())
    return best

def add_best_scouts(bee_scouts, bee_best_scouts):
    for scout in bee_best_scouts:
        bee_scouts.put(scout)

  
    

def main():
    graph,g = create_graph(400,50,1)

    # Створення початкових бджіл-розвідників
    bee_scouts = PriorityQueue()
    create_scouts(graph, bee_scouts, SCOUTS_BEE)

    for i in range(1000):
        if  (i % 20==0):
            best = bee_scouts.get()
            print(f"i: {i}, min color_number: {best.f}")
            bee_scouts.put(best)

        bee_best_scouts = get_best_scouts(bee_scouts)
        bee_random_scouts = bee_scouts.queue

        bee_scouts = PriorityQueue()
        local_search(graph, bee_scouts, bee_best_scouts, BEST_FORAGERS_BEE)
        local_search(graph, bee_scouts, bee_random_scouts, RANDOM_FORAGERS_BEE)

    #    Зберігаємо найкращий результат та шукаємо нові рішення
        bee_best_scouts = get_best_scouts(bee_scouts)
        bee_scouts = PriorityQueue()
        create_scouts(graph, bee_scouts, RANDOM_SCOUTS_BEE)
        add_best_scouts(bee_scouts, bee_best_scouts)

    best = bee_scouts.get()
    color_node = {k: v for k, v in sorted(best.color_node.items(), key=lambda item: item[0])}
    print(f"Best coloring: {best.f}\nNumber of colors: {color_node}")
    print(show_graph(color_node,g))

if __name__ == "__main__":
    main()