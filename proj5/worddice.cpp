#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

using namespace std;
typedef enum{SOURCE, SINK, WORD, DICE} Node_Type;

class Edge; //declare so it can be used in Node class

class Node{
 public:
	Node(int id, Node_Type type, string word = ""); //constructor for nodes
	~Node(); //default destructor
	bool has_letter(char c);
	friend ostream& operator<<(ostream& os, const Node& node);
	int id; //node id (ncrements as nodes are added)
	Node_Type type; //type of node it is (source, sink, word or dice)
	bool letters[26]; //length 26 with letters contained in word set to 1
	int visited; //for BFS
	vector<Edge *> adj; //adjacency list
	Edge *backedge; //previous edge for Edmonds-Karp
};

Node::Node(int id, Node_Type type, string word){
	for(int i = 0; i < 26; i++){ //sets letters to false initially
		this->letters[i] = false;
	}
	for(int i = 0; i < word.size(); i++){ //sets all letters included in the given word to true
		this->letters['A' + word[i]] = true;
	}
}

class Edge{
 public:
	//from -> to
	class Node *to; //node edge is pointing to
	class Node *from; //node edge is pointing from
	Edge(class Node *to, class Node *from, bool reverse_edge = false); //constructor for edges
	~Edge(){}; //default destructor
	Edge *reverse; //edge going the other way
	int original; //original weight per edge
	int residual; //allows for updated weighting during Edmonds-Karp
};

Edge::Edge(class Node *to, class Node *from, bool reverse_edge){

}

class Graph{
 public:
	Graph();//constructor initializes graph with source node
	~Graph(); //destructor to deallocate memory of graph
	Node *source; //not necessary but makes code more readable
	Node *sink;
	vector<Node *> nodes; //holds the nodes
	vector<int> spellingIds; //order of flow to spell word
	int min_nodes; //min number of dice nodes
	string word;
	void add_dice_to_graph(string die, int id){ //add dice nodes to graph
		Node *dice = new Node(id, DICE, die);
		nodes.push_back(dice);
	}
	void add_word_to_graph(string word, int id){ //add word (letter) nodes to graph
		Node *letter = new Node(id, WORD, word);
		nodes.push_back(letter);
	}
	bool BFS(); //breadth first search for Edmonds-Karp
	bool spell_word(); //runs Edmonds-Karp to see if we can spell the word
	void delete_word_from_graph(){ //deletes the word nodes but leaves the dice nodes
	}
	void print_node_order(string word); //print spelling Ids and word
	void print_graph(){
		for(int i = 0; i < nodes.size(); i++){
			string content = "";
			for(int j = 0; j < 26; j++){
				if(nodes[i]->letters[j]){
					content += ('A'+j);
				}
			}
			cout <<"Node ID: "<< nodes[i]->id <<" || Content: " << content <<  endl;
		}
	}
};

Graph::Graph(){
	source = new Node(0, SOURCE);
	nodes.push_back(source);
}

int main(int argc, char* argv[]){
	ifstream file;
	string line;
	Graph *graph = new Graph(); //creates source in constructor
	file.open(argv[1]); //dice file
	int nodeID = 0;
	while(getline(file, line)){
		//Node *dice = new Node(nodeID, DICE, line); //TODO: line may need to be something else Im not sure if I totally understand the use of that argument
		//graph->nodes.push_back(dice);
		nodeID++;
		graph->add_dice_to_graph(line, nodeID);
	}
	file.close();
	file.open(argv[2]); //words file
	while(getline(file, line)){
		int wordLength = 0;
		for(int i = 0; i < line.length(); i++){
			wordLength++;
			graph->add_word_to_graph(string() + line[i], nodeID + wordLength);
		}
		//Nodes are all added
		//Do stuff
		graph->print_graph();
		//Delete Word
		graph->delete_word_from_graph();
	}
	

	return 0;
}
