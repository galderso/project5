#include<list>
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
		this->letters[word[i] - 'A'] = true;
	}
	this->type = type;
	this->id = id;
}

class Edge{
	public:
		//from -> to
		class Node *to; //node edge is pointing to
		class Node *from; //node edge is pointing from
	//	Edge(class Node *to, class Node *from, bool reverse_edge = false); //constructor for edges
		~Edge(){}; //default destructor
		Edge *reverse; //edge going the other way
		int original; //original weight per edge
		int residual; //allows for updated weighting during Edmonds-Karp
};

//Edge::Edge(class Node *to, class Node *from, bool reverse_edge){

//}

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
		
		void add_sink_to_graph(int id){
			sink = new Node(id, SINK);
			nodes.push_back(sink);
		}

		bool BFS(); //breadth first search for Edmonds-Karp
		
		bool spell_word(); //runs Edmonds-Karp to see if we can spell the word
		
		void delete_word_from_graph(){ //deletes the word nodes but leaves the dice nodes
			for(int i = nodes.size()-1; i >=0; i--){ //Search from end of the list to front of the list
				if(nodes[i]->type == WORD || nodes[i]->type == SINK){
					nodes[i]->adj.clear();
					nodes.erase(nodes.begin() + i);
				}
			}
		}
		
		void print_node_order(string word); //print spelling Ids and word
		
		void print_graph(){
			for(int i = 0; i < nodes.size(); i++){
				string content = "";
				for(int j = 0; j < 26; j++){
					if(nodes[i]->letters[j]){
						content.push_back('A'+j);
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

void createEdge(Node *n1,Node *n2){
	Edge *edge1 = new Edge();
	Edge *edge2 = new Edge();


	edge2->to=n1;
	edge2->from=n2;
	edge2->reverse=edge1;
	edge2->original=0;
	edge2->residual=1;
	n2->adj.push_back(edge2);
	edge1->to=n2;
	edge1->from=n1;
	edge1->reverse=edge2;
	edge1->original=1;
	edge1->residual=0;
	n1->adj.push_back(edge1);



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
	//creates edges from source to dice

	graph->min_nodes =nodeID+1;
	Node *n1=graph->nodes[0];
	for(int i =1;i<graph->min_nodes;i++){
		Node *n2=graph->nodes[i];

		createEdge(n1,n2);


	}

	file.close();
	file.open(argv[2]); //words file
	while(getline(file, line)){
		int wordLength = 0;
		for(int i = 0; i < line.length(); i++){
			wordLength++;
			graph->add_word_to_graph(string() + line[i], nodeID + wordLength);
		}
		graph->add_sink_to_graph(nodeID + wordLength + 1);

		//creates edges between dice and word based on the letters
		for(int i=1;i<graph->min_nodes;i++){
			Node *n1=graph->nodes[i];
			for(int k=graph->min_nodes;k<graph->nodes.size()-1;k++){
				Node *n2=graph->nodes[k];
				for(int j =0;j<26;j++){
					if(graph->nodes[k]->letters[j]==true && n1->letters[j]==true){
						createEdge(n1,graph->nodes[k]);
					}


				}

			}


		}



		//creates edges from word to sink
		Node *n2=graph->nodes[graph->nodes.size()-1];
		for(int i =graph->min_nodes;i<graph->nodes.size()-1;i++){
			Node *n1=graph->nodes[i];

			createEdge(n1,n2);


		}

		//Nodes are all added
		//Do stuff
		graph->print_graph();
		//Delete Word
		graph->delete_word_from_graph();
	}


	return 0;
}

bool Graph::BFS(){
	
	list<Node*>queue;
	

	//(nodes.backedge).assign(nodes.size(),NULL);	
		for(int i=0;i<nodes.size();i++){
		nodes[i]->backedge=NULL;
		nodes[i]->visited=0;
		}

	Node *n =nodes.front();
	queue.push_back(n);

	while(queue.size()){
	queue.front();
	queue.pop_front();


	for(int i =0;i<n->adj.size();i++){
		if(n->adj[i]->to->visited==0 &&n->adj[i]->original==1){
		n->adj[i]->to->backedge=n->adj[i];
		n->adj[i]->to->visited=1;
		queue.push_back(n->adj[i]->to);


		}
	}
	}



if(n==nodes.back()){
return true;
}else{

return false;
}

}


bool Graph::spell_word(){
Node* n;
	while(BFS()){
	n=nodes.back();

	while(n->type!=SOURCE){
	n->backedge->original=0;
	n->backedge->residual=1;
	n->backedge->reverse->original=1;
	n->backedge->reverse->residual=0;
	
	n=n->backedge->from;
	}


}

n=nodes.back();

for(int i =min_nodes;i<nodes.size()-1;i++){
	n=nodes[i];
	for(int j =0;j<n->adj.size();j++){
		if(n->adj[j]->reverse->residual!=0){
		if(n->adj[j]->to->type==DICE&&n->adj[j]->original==1){
			spellingIds.push_back(n->adj[j]->to->id);
		}
		}else{
			return false;
		}

	}

}
return true;
}
//void Graph::delete_word_from_graph(){
//}

