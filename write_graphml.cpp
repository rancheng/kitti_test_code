#include <boost/config.hpp>
#include <boost/graph/graphml.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/property_map/property_map.hpp>
#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <utility>

using namespace boost;
using namespace std;


// CSV parser: https://stackoverflow.com/questions/1120140/how-can-i-read-and-parse-csv-files-in-c
class CSVRow
{
public:
    std::string const& operator[](std::size_t index) const
    {
        return m_data[index];
    }
    std::size_t size() const
    {
        return m_data.size();
    }
    void readNextRow(std::istream& str)
    {
        std::string         line;
        std::getline(str, line);

        std::stringstream   lineStream(line);
        std::string         cell;

        m_data.clear();
        while(std::getline(lineStream, cell, ','))
        {
            m_data.push_back(cell);
        }
        // This checks for a trailing comma with no data after it.
        if (!lineStream && cell.empty())
        {
            // If there was a trailing comma then add an empty element.
            m_data.push_back("");
        }
    }
private:
    std::vector<std::string>    m_data;
};

std::istream& operator>>(std::istream& str, CSVRow& data)
{
    data.readNextRow(str);
    return str;
}

class CSVIterator
{
public:
    typedef std::input_iterator_tag     iterator_category;
    typedef CSVRow                      value_type;
    typedef std::size_t                 difference_type;
    typedef CSVRow*                     pointer;
    typedef CSVRow&                     reference;

    CSVIterator(std::istream& str)  :m_str(str.good()?&str:NULL) { ++(*this); }
    CSVIterator()                   :m_str(NULL) {}

    // Pre Increment
    CSVIterator& operator++()               {if (m_str) { if (!((*m_str) >> m_row)){m_str = NULL;}}return *this;}
    // Post increment
    CSVIterator operator++(int)             {CSVIterator    tmp(*this);++(*this);return tmp;}
    CSVRow const& operator*()   const       {return m_row;}
    CSVRow const* operator->()  const       {return &m_row;}

    bool operator==(CSVIterator const& rhs) {return ((this == &rhs) || ((this->m_str == NULL) && (rhs.m_str == NULL)));}
    bool operator!=(CSVIterator const& rhs) {return !((*this) == rhs);}
private:
    std::istream*       m_str;
    CSVRow              m_row;
};

struct Vertex{ std::string v_name;};
struct Edge{std::string e_name;};

std::size_t str2indx(string ind_str){
    std::string::size_type n = ind_str.rfind( '_' );
    std::size_t _idx = -1;
    if ( n != std::string::npos )
        _idx = std::stoi( ind_str.substr( n + 1 ) );
    return _idx;
}

int main(){
    typedef pair<string, string> _Edge;
    vector<string> vertx;
    vector<_Edge> edges;
    std::ifstream edgefile("edge_list.csv");
    std::ifstream vertfile("vertex_list.csv");
    for(CSVIterator loop(edgefile); loop != CSVIterator(); ++loop)
    {
        // std::cout << "2nd Element(" << (*loop)[1] << ")\n";
        _Edge e((*loop)[0], (*loop)[1]);
        edges.push_back(e);
    }
    for(CSVIterator loop(vertfile); loop != CSVIterator(); ++loop)
    {
        // std::cout << "2nd Element(" << (*loop)[1] << ")\n";
        vertx.push_back((*loop)[0]);
    }
    const int nedge = edges.size();
    const int nvertex = vertx.size();

    //Define the graph using those classes
    typedef boost::adjacency_list<boost::listS, boost::vecS, boost::undirectedS, Vertex, Edge > Graph;
    //Some typedefs for simplicity
    typedef boost::graph_traits<Graph>::vertex_descriptor vertex_t;
    typedef boost::graph_traits<Graph>::edge_descriptor edge_t;

    //Instanciate a graph
    Graph g;

    vector<vertex_t> vtx;
    for(auto v: vertx){
        vertex_t _v = boost::add_vertex(g);
        vtx.push_back(_v);
        g[_v].v_name = v;
    }
    for(auto e: edges){
        vertex_t v1 = vtx[str2indx(e.first)];
        vertex_t v2 = vtx[str2indx(e.second)];
        // Create an edge conecting those two vertices
        edge_t _e; bool b;
        boost::tie(_e,b) = boost::add_edge(v1,v2,g);
        g[_e].e_name = to_string(str2indx(e.first)) + "_" + to_string(str2indx(e.second));
    }

    boost::dynamic_properties dp;
    dp.property("vertex_name",get(&Vertex::v_name,g));
    dp.property("edge_name",get(&Edge::e_name,g));

    ofstream graphMLfile;
    graphMLfile.open ("../kyber_graphml.xml", fstream::out);
    write_graphml(graphMLfile, g, dp);
    graphMLfile.close();
    return 0;
}
