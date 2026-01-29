# WAP to implement DFS and BFS
from typing import Dict, List


def create_graph(numNodes: int, edges: List[List[int]]) -> List[List[int]]:
  graph = {} # using a hashMap to store "node -> neighs[]"
  for i in range(numNodes):
    graph[i] = []
  
  for edge in edges:
    graph[edge[0]].append(edge[1])
    graph[edge[1]].append(edge[0])
  
  return graph

def dfs(numNodes: int, graph: Dict[int, List[int]], startFrom: int) -> List[int]:
  visited = [False] * numNodes # to track visited nodes
  res = []                     # traversal order result
  
  def dfs_helper(node: int, graph: Dict[int, List[int]]):
    if (visited[node]):
      return
    
    res.append(node)
    visited[node] = True
    for neighbour in graph[node]:
      dfs_helper(neighbour, graph)
  
  dfs_helper(startFrom, graph)
  return res

def bfs(numNodes: int, graph: Dict[int, List[int]], startFrom: int) -> List[int]:
  visited = [False] * numNodes # to track visited nodes
  res = []                     # traversal order result
  queue = [startFrom]
  
  while len(queue) > 0:
    currentSize = len(queue)
    for _ in range(currentSize):
      node = queue.pop(0)
      if (visited[node]):
        continue
      
      visited[node] = True
      res.append(node)
      
      for neighbour in graph[node]:
        queue.append(neighbour)
  
  return res

def main():
  numNodes = 7
  edges = [[0,1], [1,3], [0,2], [3,4], [5,1], [6,1], [0,5], [6,3]]
  graph = create_graph(numNodes, edges)
  
  dfs_res = dfs(numNodes, graph, 0)
  bfs_res = bfs(numNodes, graph, 0)
  
  print("Graph: ", graph)
  print("DFS: ", dfs_res)
  print("BFS: ", bfs_res)

main()