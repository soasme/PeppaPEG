import sys
import json

def output_node(token, index):
    print('n%d[label="type=%s\nslice=[%d,%d]"];' % (
        index, token['type'], token['slice'][0], token['slice'][1]
    ))

def output_edge(token, parent_index, node_index):
    print('n%d -> n%d' % (parent_index, node_index))

def traverse(node, f):
    f(node)
    for child in node.get('children', []):
        traverse(child, f)

def assign_id(root, index=1):
    def _(token):
        nonlocal index
        token['id'] = index
        index += 1
    for elem in root:
        traverse(elem, _)
    return index

def gen_nodes(root):
    def _(node):
        print('    n%d[label="type=%s\\nslice=[%d,%d]"];' % (
            node['id'],
            node['type'],
            node['slice'][0],
            node['slice'][1]
        ))
    for elem in root:
        traverse(elem, _)

def gen_edges(root):
    def _(node):
        for child in node.get('children', []):
            print('    n%d -> n%d;' % (
                node['id'],
                child['id'],
            ))
    for elem in root:
        traverse(elem, _)

def gen_dot(tree):
    assign_id(tree)
    print('digraph ParseTree {')
    gen_nodes(tree)
    gen_edges(tree)
    print('}')

if __name__ == '__main__':
    gen_dot(json.loads(sys.stdin.read()))
