//By Grant Alderson and Sam Craddock
//This program uses edmonds carp algrithm to find the correct spelling of words using dice
//Citations: Rob's video, Cammile's videos , lab write up
#include<list>
#include<iostream>
#include<fstream>
#include<string>
#include<vector>


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
			for(int i = nodes.size()-1; i >=1; i--){ //Search from end of the list to front of the list
				if(nodes[i]->type == WORD || nodes[i]->type == SINK){
					nodes[i]->adj.clear();
					nodes.erase(nodes.begin() + i);
				}
				else{
					nodes[i]->adj.erase(nodes[i]->adj.begin()+1, nodes[i]->adj.end());
					nodes[i]->visited = 0;
				}
			}
			//reverses effects of BFS for next iteration
			for(int i = 0; i < nodes[0]->adj.size(); i++){
				nodes[0]->adj[i]->original = 1;
				nodes[0]->adj[i]->residual = 0;
				nodes[0]->adj[i]->reverse->original = 0;
				nodes[0]->adj[i]->reverse->residual = 1;
			}
			spellingIds.clear();
		}

		void print_node_order(){ //print spelling Ids and word
			if(spell_word()){
				for(int i = 0; i < spellingIds.size(); i++){
					cout << spellingIds[i];
					if(i != spellingIds.size()-1){
						cout << ",";
					}
				}
				cout << ": " << word << endl;
			}
			else{
				cout << "Cannot spell " << word << endl;
				//cout << spellingIds.size();
			}

		}
		void print_graph(){
			for(int i = 0; i < nodes.size(); i++){
				string content = "";
				for(int j = 0; j < 26; j++){
					if(nodes[i]->letters[j]){
						content.push_back('A'+j);
					}
				}
				cout <<"Node ID: "<< nodes[i]->id <<" || Content: " << content << " || Edges to: ";
				for(int j = 0; j < nodes[i]->adj.size(); j++){
					cout << nodes[i]->adj[j]->to->id;
				}
				cout << endl;
			}
	

		}
};

Graph::Graph(){
	source = new Node(0, SOURCE);
	nodes.push_back(source);
}
//creates edges between both nodes
void createEdge(Node *node1,Node *node2){
	Edge *edge1 = new Edge();
	Edge *edge2 = new Edge();


	edge2->to=node1;
	edge2->from=node2;
	edge2->reverse=edge1;
	edge2->original=0;
	edge2->residual=1;
	node2->adj.push_back(edge2);
	edge1->to=node2;
	edge1->from=node1;
	edge1->reverse=edge2;
	edge1->original=1;
	edge1->residual=0;
	node1->adj.push_back(edge1);




}
int main(int argc, char* argv[]){
	ifstream file;
	string line;
	Graph *graph = new Graph(); //creates source in constructor
	file.open(argv[1]); //dice file
	int nodeID = 0;
	while(getline(file, line)){
		nodeID++;
		graph->add_dice_to_graph(line, nodeID);
	}
	//creates edges from source to dice

	graph->min_nodes =nodeID+1;
	Node *node1=graph->nodes.front();
	for(int i =1;i<graph->min_nodes;i++){
		createEdge(node1,graph->nodes[i]);


	}

	file.close();
	file.open(argv[2]); //words file
	while(getline(file, line)){
		int wordLength = 0;
		for(int i = 0; i < line.length(); i++){
			wordLength++;
			graph->add_word_to_graph(string() + line[i], nodeID + wordLength);
		}
		graph->word = line;
		graph->add_sink_to_graph(nodeID + wordLength + 1);

		//creates edges between dice and word based on the letters
		for(int i=1;i<graph->min_nodes;i++){
			Node *node1=graph->nodes[i];
			for(int k=graph->min_nodes;k<graph->nodes.size()-1;k++){								for(int j =0;j<26;j++){
					if(graph->nodes[k]->letters[j]==true && node1->letters[j]==true){
						createEdge(node1,graph->nodes[k]);
					}
				}

			}


		}



		//creates edges from word to sink
		Node *node2=graph->nodes.back();
		for(int i =graph->min_nodes;i<graph->nodes.size()-1;i++){
			createEdge(graph->nodes[i],node2);


		}

		//Nodes are all added
		//Do stuff
		//graph->print_graph();
		graph->print_node_order();

		//Delete Word
		graph->delete_word_from_graph();
	}


	return 0;
}
//finds each path
//citation:https://www.geeksforgeeks.org/breadth-first-search-or-bfs-for-a-graph/
bool Graph::BFS(){

	list<Node *>queue;


	//(nodes.backedge).assign(nodes.size(),NULL);	
	for(int i=0;i<nodes.size();i++){
		nodes[i]->backedge=NULL;
		nodes[i]->visited=0;
	}

	Node *node =nodes.front();
	queue.push_back(node);

	while(queue.size()){
		node=queue.front();
		queue.pop_front();

		for(int i =0;i<node->adj.size();i++){
			if(node==nodes.back()){
				return true;
			}else{
				if(node->adj[i]->to->visited==0){ 
					if(node->adj[i]->original==1){

						node->adj[i]->to->backedge=node->adj[i];
						node->adj[i]->to->visited=1;
						queue.push_back(node->adj[i]->to);

					}
				}
			}
		}
	}



	return false;


}

//chcks if word can be spelled and adds ids to spellingIds
bool Graph::spell_word(){
	Node* node;
	spellingIds.clear();
    


	
	while(BFS()){//switches original and residual so it doesnt travers the same path

	

		node=nodes.back();

		while(node!=nodes.front()){
			node->backedge->original=0;
			node->backedge->residual=1;
			node->backedge->reverse->original=1;
			node->backedge->reverse->residual=0;

			node=node->backedge->from;
		}


	}

	node=nodes.back();


	for(int i = 0; i < node->adj.size(); i++) {
		if(node->adj[i]->reverse->residual != 1){
			return false;
		}
	}
	for(int k =min_nodes;k<nodes.size()-1;k++){		//adds dice nodes ids to spellingIds
		node=nodes[k];
		for(int j =0;j<node->adj.size();j++){
			if(node->adj[j]->original==1){
				if(node->adj[j]->to->type==DICE){
					spellingIds.push_back((node->adj[j]->to->id)-1);
				}
			}
		}
		}
	
	return true;
}

