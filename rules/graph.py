from xml.dom import minidom
import json

my_nodes = []
my_edges = []

# parse an xml file by name
file = minidom.parse('matera/matera_graph.graphml')

# get each element by tag name
nodes = file.getElementsByTagName('node')

# get attribute of node
for node in nodes:
    n = {'id': node.getAttribute('id')}
    for data in node.getElementsByTagName('data'):
        # check if the value is int or float or string
        if data.firstChild.nodeValue.isdigit():
            n[data.getAttribute('key')] = int(data.firstChild.nodeValue)
        elif data.firstChild.nodeValue.replace('.', '', 1).isdigit():
            n[data.getAttribute('key')] = float(data.firstChild.nodeValue)
        else:
            n[data.getAttribute('key')] = data.firstChild.nodeValue
    my_nodes.append(n)

# get each element by tag name
edges = file.getElementsByTagName('edge')

# get attribute of edge
for edge in edges:
    e = {'source': edge.getAttribute('source'), 'target': edge.getAttribute('target')}
    for data in edge.getElementsByTagName('data'):
        # check if the value is int or float or string
        if data.firstChild.nodeValue.isdigit():
            e[data.getAttribute('key')] = int(data.firstChild.nodeValue)
        elif data.firstChild.nodeValue.replace('.', '', 1).isdigit():
            e[data.getAttribute('key')] = float(data.firstChild.nodeValue)
        else:
            e[data.getAttribute('key')] = data.firstChild.nodeValue
    my_edges.append(e)

# we print the number of nodes and edges
print('Number of nodes: ' + str(len(my_nodes)))
print('Number of edges: ' + str(len(my_edges)))

graph = {'nodes': my_nodes, 'edges': my_edges}
# save to json file
with open('matera/matera_graph.json', 'w') as outfile:
    json.dump(graph, outfile)
