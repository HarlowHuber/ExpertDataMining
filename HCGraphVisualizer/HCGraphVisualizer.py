import networkx as nx
import matplotlib.pyplot as plt
import csv


graph = nx.Graph()

# read in complete graph file 
completeGraphFile = open('completeGraph.csv', newline='')
data = csv.reader(completeGraphFile)

for row in data:
    graph.add_edge(int(row[0]), int(row[1]), weight=float(row[2]))

# get automatic minimum spanning tree
#MST = nx.minimum_spanning_tree(graph, algorithm="kruskal") # default algo is kruskal

# or read in own custom MST file
MSTFile = open('MST.csv', newline='') 
data = csv.reader(MSTFile)

MST = nx.Graph()

for row in data:
    MST.add_edge(int(row[0]), int(row[1]), weight=float(row[2]))


# visualize complete graph and minimum spanning tree together as one
pos = nx.spring_layout(graph)
nx.draw_networkx_nodes(graph, pos, node_color="lightblue", node_size=500)
nx.draw_networkx_edges(graph, pos, edge_color="grey")
nx.draw_networkx_edges(MST, pos, edge_color="red", width=3)
nx.draw_networkx_labels(graph, pos, font_size=12, font_family="sans-serif")
nx.draw_networkx_edge_labels(
    graph, pos, font_size=8, edge_labels={(u, v): d["weight"] for u, v, d in graph.edges(data=True)}
)

plt.axis("off")
plt.show()


completeGraphFile.close()
