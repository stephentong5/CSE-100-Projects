#include "Graph.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <string>
#include <utility>
#include <queue>
#include <tuple>
#include <algorithm>
#include <limits>
#include <unordered_set>
using namespace std;

Graph::Graph(const char* const & edgelist_csv_fn)
{
    ifstream my_file(edgelist_csv_fn);
    string line;
    while(getline(my_file, line))
    {
        istringstream ss(line);
        string first, second, third;
        getline(ss, first, ',');
        getline(ss, second, ',');
        getline(ss, third, '\n');
        
        vector<pair<string, string>> neighbors1;
        if(graph.find(first) != graph.end())
        {
            neighbors1 = (graph.find(first))->second;
            graph.erase(first);
        }
        neighbors1.push_back({second, third});
        graph.insert({first, neighbors1});

        vector<pair<string, string>> neighbors2;
        if(graph.find(second) != graph.end())
        {
            neighbors2 = (graph.find(second))->second;
            graph.erase(second);
        }
        neighbors2.push_back({first, third});
        graph.insert({second, neighbors2});
    }
my_file.close();
}

unsigned int Graph::num_nodes()
{
    return graph.size();
}

vector<string> Graph::nodes()
{
    auto it = graph.begin();
    auto end = graph.end();
    vector<string> nodes;
    
    while(it != end)
    {
        string node = it->first;
        nodes.push_back(node);
        it++;
    }
    
    return nodes;
}

unsigned int Graph::num_edges()
{
    auto it = graph.begin();
    auto end = graph.end();
    unsigned int edges = 0;

    while(it != end)
    {
        string node = it->first;
        edges += num_neighbors(node);
        it++;
    }
    return edges/2;
}

unsigned int Graph::num_neighbors(string const & node_label)
{
    auto it = graph.find(node_label);
    return it->second.size();
}

double Graph::edge_weight(string const & u_label, string const & v_label)
{
    auto it = graph.find(u_label);
    vector<pair<string, string>> neighbors = it->second;

    for(unsigned int i = 0; i < neighbors.size(); i++)
    {
        if(neighbors[i].first == v_label)
        {
            return stod(neighbors[i].second);
        }
    }
    return -1;
}

vector<string> Graph::neighbors(string const & node_label)
{
    auto it = graph.find(node_label);
    vector<pair<string, string>> edges = it->second;
    vector<string> neighbors;
    for(unsigned int i = 0; i < edges.size(); i++)
    {
        neighbors.push_back(edges[i].first);
    }
    return neighbors;
}

vector<string> Graph::shortest_path_unweighted(string const & start_label, string const & end_label)
{
    vector<string> path;

    if(start_label == end_label)
    {
        path.push_back(start_label);
        return path;
    }

    queue<tuple<string, int, string>> q;
    unordered_map<string, pair<int, string>> visited;

    q.push({start_label, 0, start_label});

    while(q.size() != 0)
    {
        string curr = get<0>(q.front());
        int dist = get<1>(q.front());
        string prev = get<2>(q.front());

        q.pop();

        if(visited.find(curr) == visited.end())
        {
            visited.insert({curr, {dist, prev}});
            if(curr == end_label)
            {
                while(prev != curr)
                {
                    path.push_back(curr);
                    curr = prev;
                    prev = visited.find(curr)->second.second;
                }
                path.push_back(curr);
                reverse(path.begin(), path.end());
                return path;
            }
            vector<string> neighs = neighbors(curr);
            for(string neigh : neighs)
            {
                if(visited.find(neigh) == visited.end())
                {
                    q.push({neigh, dist + 1, curr});
                }
            }
        }
    }
    return path;
}

vector<tuple<string,string,double>> Graph::shortest_path_weighted(string const & start_label, string const & end_label)
{
    vector<tuple<string, string, double>> path;

    if(start_label == end_label)
    {
        path.push_back({start_label, start_label, -1});
        return path;
    }

    unordered_map<string, double> init;
    priority_queue<tuple<double, string, string>, vector<tuple<double, string, string>>, greater<tuple<double, string, string>>> pq;
    unordered_map<string, pair<double, string>> visited;

    vector<string> nodelist = nodes();
    for(string node : nodelist)
    {
        if(node == start_label)
        {
            init.insert({node, 0});
        }
        else
        {
            init.insert({node, numeric_limits<double>::max()});
        }
    }

    pq.push({0, start_label, start_label});

    while(pq.size() != 0)
    {
        double dist = get<0>(pq.top());
        string curr = get<1>(pq.top());
        string prev = get<2>(pq.top());

        pq.pop();

        if(visited.find(curr) == visited.end())
        {
            visited.insert({curr, {dist, prev}});
            if(curr == end_label)
            {
                while(prev != curr)
                {
                    path.push_back({prev, curr, edge_weight(prev, curr)});
                    curr = prev;
                    prev = visited.find(curr)->second.second;
                }
                reverse(path.begin(), path.end());
                return path;
            }
            vector<string> neighs = neighbors(curr);
            for(string neigh : neighs)
            {
                double neigh_dist = dist + edge_weight(curr, neigh);
                if(neigh_dist < init.find(neigh)->second)
                {
                    init.erase(neigh);
                    init.insert({neigh, neigh_dist});
                    pq.push({neigh_dist, neigh, curr});
                }
            }
        }
    }
    return path;
}

vector<vector<string>> Graph::connected_components(double const & threshold)
{
    queue<tuple<string, int, string>> q;
    unordered_map<string, pair<int, string>> visited;
    unordered_set<string> unvisited;
    vector<vector<string>> ccs;
    vector<string> cc;

    vector<string> nodelist = nodes();
    for(string node : nodelist)
    {
        unvisited.insert(node);
    }

    while(unvisited.size() != 0)
    {
        auto it = unvisited.begin();
        string start = *it;

        q.push({start, 0, start});

        while(q.size() != 0)
        {
            string curr = get<0>(q.front());
            int dist = get<1>(q.front());
            string prev = get<2>(q.front());

            q.pop();

            if(visited.find(curr) == visited.end())
            {
                visited.insert({curr, {dist, prev}});
                unvisited.erase(curr);
                cc.push_back(curr);
                
                vector<string> neighs = neighbors(curr);
                for(string neigh : neighs)
                {
                    if(visited.find(neigh) == visited.end() && edge_weight(curr, neigh) <= threshold)
                    {   
                        q.push({neigh, dist + 1, curr});
                    }
                }
            }
        }
        ccs.push_back(cc);
        cc.clear();
    }
    return ccs;
}

double Graph::smallest_connecting_threshold(string const & start_label, string const & end_label)
{
    if(start_label == end_label)
    {
        return 0;
    }

    auto it = graph.begin();
    auto end = graph.end();
    unordered_set<double> edges;
    vector<double> edgelist;

    while(it != end)
    {
        string node = it->first;
        vector<string> neighs = neighbors(node);
        for(string neigh : neighs)
        {
            double edge = edge_weight(node, neigh);
            if(edges.find(edge) == edges.end())
            {
                edges.insert(edge);
            }
        }
        it++;
    }
    auto itr = edges.begin();
    while(itr != edges.end())
    {
        edgelist.push_back(*itr);
        itr++;
    }
    sort(edgelist.begin(), edgelist.end());
    for(unsigned int i = 0; i < edgelist.size(); i++)
    {
        queue<tuple<string, int, string>> q;
        unordered_map<string, pair<int, string>> visited;

        q.push({start_label, 0, start_label});

        while(q.size() != 0)
        {
            string curr = get<0>(q.front());
            int dist = get<1>(q.front());
            string prev = get<2>(q.front());

            q.pop();

            if(visited.find(curr) == visited.end())
            {
                visited.insert({curr, {dist, prev}});
                if(curr == end_label)
                {
                    return edgelist[i];
                }
                vector<string> neighs = neighbors(curr);
                for(string neigh : neighs)
                {
                    if(visited.find(neigh) == visited.end() && edge_weight(curr, neigh) <= edgelist[i])
                    {
                        q.push({neigh, dist + 1, curr});
                    }
                }
            }
        }
    }
    return -1;
}
