//=======================================================================
// Copyright 2001 Jeremy G. Siek, Andrew Lumsdaine, Lie-Quan Lee, 
//
// This file is part of the Boost Graph Library
//
// You should have received a copy of the License Agreement for the
// Boost Graph Library along with the software; see the file LICENSE.
// If not, contact Office of Research, Indiana University,
// Bloomington, IN 47405.
//
// Permission to modify the code and to distribute the code is
// granted, provided the text of this NOTICE is retained, a notice if
// the code was modified is included with the above COPYRIGHT NOTICE
// and with the COPYRIGHT NOTICE in the LICENSE file, and that the
// LICENSE file is distributed with the modified code.
//
// LICENSOR MAKES NO REPRESENTATIONS OR WARRANTIES, EXPRESS OR IMPLIED.
// By way of example, but not limitation, Licensor MAKES NO
// REPRESENTATIONS OR WARRANTIES OF MERCHANTABILITY OR FITNESS FOR ANY
// PARTICULAR PURPOSE OR THAT THE USE OF THE LICENSED SOFTWARE COMPONENTS
// OR DOCUMENTATION WILL NOT INFRINGE ANY PATENTS, COPYRIGHTS, TRADEMARKS
// OR OTHER RIGHTS.
//=======================================================================
#include <boost/config.hpp>
#include <iostream>
#include <boost/graph/adjacency_list.hpp>
using namespace boost;

template < typename UndirectedGraph > void
undirected_graph_demo1()
{
  const int V = 3;
  UndirectedGraph undigraph(V);
  typename graph_traits < UndirectedGraph >::vertex_descriptor zero, one, two;
  typename graph_traits < UndirectedGraph >::out_edge_iterator out, out_end;
  typename graph_traits < UndirectedGraph >::in_edge_iterator in, in_end;

  zero = vertex(0, undigraph);
  one = vertex(1, undigraph);
  two = vertex(2, undigraph);
  add_edge(zero, one, undigraph);
  add_edge(zero, two, undigraph);
  add_edge(one, two, undigraph);

  std::cout << "out_edges(0): ";
  for (tie(out, out_end) = out_edges(zero, undigraph); out != out_end; ++out)
    std::cout << *out;
  std::cout << std::endl << "in_edges(0): ";
  for (tie(in, in_end) = in_edges(zero, undigraph); in != in_end; ++in)
    std::cout << *in;
  std::cout << std::endl;
}

template < typename DirectedGraph > void
directed_graph_demo()
{
  const int V = 2;
  DirectedGraph digraph(V);
  typename graph_traits < DirectedGraph >::vertex_descriptor u, v;
  typedef typename DirectedGraph::edge_property_type Weight;
  typename property_map < DirectedGraph, edge_weight_t >::type
    weight = get(edge_weight, digraph);
  typename graph_traits < DirectedGraph >::edge_descriptor e1, e2;
  bool found;

  u = vertex(0, digraph);
  v = vertex(1, digraph);
  add_edge(u, v, Weight(1.2), digraph);
  add_edge(v, u, Weight(2.4), digraph);
  tie(e1, found) = edge(u, v, digraph);
  tie(e2, found) = edge(v, u, digraph);
  std::cout << "in a directed graph is ";
#ifdef __GNUC__
  // no boolalpha
  std::cout << "(u,v) == (v,u) ? " << (e1 == e2) << std::endl;
#else
  std::cout << "(u,v) == (v,u) ? "
    << std::boolalpha << (e1 == e2) << std::endl;
#endif
  std::cout << "weight[(u,v)] = " << get(weight, e1) << std::endl;
  std::cout << "weight[(v,u)] = " << get(weight, e2) << std::endl;
}

template < typename UndirectedGraph > void
undirected_graph_demo2()
{
  const int V = 2;
  UndirectedGraph undigraph(V);
  typename graph_traits < UndirectedGraph >::vertex_descriptor u, v;
  typedef typename UndirectedGraph::edge_property_type Weight;
  typename property_map < UndirectedGraph, edge_weight_t >::type
    weight = get(edge_weight, undigraph);
  typename graph_traits < UndirectedGraph >::edge_descriptor e1, e2;
  bool found;

  u = vertex(0, undigraph);
  v = vertex(1, undigraph);
  add_edge(u, v, Weight(3.1), undigraph);
  tie(e1, found) = edge(u, v, undigraph);
  tie(e2, found) = edge(v, u, undigraph);
  std::cout << "in an undirected graph is ";
#ifdef __GNUC__
  std::cout << "(u,v) == (v,u) ? " << (e1 == e2) << std::endl;
#else
  std::cout << "(u,v) == (v,u) ? "
    << std::boolalpha << (e1 == e2) << std::endl;
#endif
  std::cout << "weight[(u,v)] = " << get(weight, e1) << std::endl;
  std::cout << "weight[(v,u)] = " << get(weight, e2) << std::endl;
}


int
main()
{
  typedef property < edge_weight_t, double >Weight;
  typedef adjacency_list < vecS, vecS, undirectedS,
    no_property, Weight > UndirectedGraph;
  typedef adjacency_list < vecS, vecS, directedS,
    no_property, Weight > DirectedGraph;
  undirected_graph_demo1 < UndirectedGraph > ();
  undirected_graph_demo2 < UndirectedGraph > ();
  directed_graph_demo < DirectedGraph > ();
  return 0;
}
