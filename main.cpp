#include "include/maze/Graph.h"
#include "include/tree/Tree.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <string.h>
#include <cfloat>
#include "perf.h"

using namespace std;

string formatFloat(float value, int precision, int totalLength)
{
    string returnString = "";
    if (value == FLT_MAX)
        returnString = "&";
    else
    {
        std::stringstream stream;
        stream << std::fixed << std::setprecision(precision) << value;
        returnString = stream.str();
    }
    int missingSpaces = totalLength - returnString.length();
    if (missingSpaces > 0)
        returnString.insert(0, missingSpaces, ' ');
    return returnString;
}

string exportGraphToDotFormat(Graph *graph)
{ // dot -Tpng output.dot -o graph1.png
    if (graph == nullptr)
        return "";

    Node *nextNode = graph->getFirstNode();
    string dot = "", connector;
    // bool weightedNode = graph->getWeightedNode();
    // bool weightedEdge = graph->getWeightedEdge();

    dot += "strict ";

    if (false)
    {
        connector = " -> ";
        dot += "di";
    }
    else
        connector = " -- ";

    dot += "graph grafo {\n";
    char aspasDuplas = '"';
    while (nextNode != nullptr)
    {
        dot += "  " + to_string(nextNode->getId()) +
               " [weight = " + formatFloat(nextNode->getId(), 2, 5) +
               ", pos = " + aspasDuplas + nextNode->getPosition() + "!" + aspasDuplas;

        if (nextNode->getTag() == "inicial")
            dot += ", color = red";

        if (nextNode->getTag() == "final")
            dot += ", color = green";

        dot += "];\n";

        nextNode = nextNode->getNextNode();
    }
    nextNode = graph->getFirstNode();
    while (nextNode != nullptr)
    {
        Edge *nextEdge = nextNode->getFirstEdge();
        while ((nextEdge != nullptr))
        {
            if ((nextNode->getId() != graph->getNodeById(nextEdge->getTargetId())->getId()))
            {
                dot += "\n  " + to_string(nextNode->getId()) + connector +
                       to_string(graph->getNodeById(nextEdge->getTargetId())->getId());
                // if (weightedEdge)
                // {
                //     dot += " [weight = " + formatFloat(nextEdge->getWeight(), 2, 5) +
                //            "] [label = " + formatFloat(nextEdge->getWeight(), 2, 5) + "];";
                // }
            }
            nextEdge = nextEdge->getNextEdge();
        }
        nextNode = nextNode->getNextNode();
    }
    dot += "\n}\n";

    return dot;
}

string exportTreeToDotFormat(Tree *tree)
{
    if (tree == nullptr || tree->getRoot() == nullptr)
        return "";

    TreeNode *rootNode = tree->getRoot();
    string dot = "strict digraph tree {\nrankdir=TB;\n";

    // Traverse the tree and add nodes
    tree->traverseAndPrint(rootNode, dot);

    // Close the graph
    dot += "}\n";

    return dot;
}

void writeOutputFile(Graph *graph, Tree *searchTree, int option)
{
    // Especifique o caminho completo para a pasta onde deseja salvar o arquivo "output.dot"
    std::string outputPath;

    if (option == 0)
        outputPath = "../graph.dot";
    else
        outputPath = "../tree.dot";

    // Abra o arquivo "output.dot" no caminho especificado e apague o conteúdo antigo
    std::ofstream outputFile(outputPath, std::ios::trunc);

    if (outputFile.is_open())
    {
        // Chame a função exportGraphToDotFormat e escreva o resultado no arquivo
        if (option == 0)
            outputFile << exportGraphToDotFormat(graph);
        else
            outputFile << exportTreeToDotFormat(searchTree);

        // Feche o arquivo
        outputFile.close();
    }
    else
    {
        std::cerr << "Erro ao abrir o arquivo 'output.dot'." << std::endl;
    }
}

void printSolution(stack<TreeNode *> pilha, string search, Perf::PerformanceTimer clock, int statesNumber, int visitedStatesNumber)
{
    stringstream formattedTime;

    cout << search + "Solution: ";
    while (!pilha.empty())
    {
        TreeNode *node = pilha.top();
        pilha.pop();

        if (pilha.size() != 0)
        {
            cout << node->getId() << " -> ";
        }
        else
            cout << node->getId() << " ";
    }
    cout << " ___ " << endl;

    // transforma em mili segundos
    double elapsedTimeInSeconds = clock.elapsed_time();
    double elapsedTimeInMilliseconds = elapsedTimeInSeconds * 1000;

    formattedTime << std::fixed << std::setprecision(6) << elapsedTimeInMilliseconds;
    string timeStr = formattedTime.str();

    cout << "TEMPO -- " << timeStr << " milisegundos";
    cout << endl;
    cout << "Estados gerados -- " << statesNumber << " estados" << endl;
    cout << "Estados vsitados -- " << visitedStatesNumber << " estados" << endl;
}

int main(int argc, char const *argv[])
{

    Graph *maze = new Graph();
    maze->generateMaze();

    Tree *searchTree = new Tree();
    std::stack<TreeNode *> solution;

    // clock
    Perf::PerformanceTimer clock;

    // aux var
    int statesNumber = 0;
    int visitedStatesNumber = 0;

    int option = 0;

    while (option < 1 || option > 6)
    {

        cout << endl
             << "Selecione a busca: " << endl
             << endl
             << "[1] Busca backtracking" << endl
             << "[2] Busca em largura" << endl
             << "[3] Busca em profundidade" << endl
             << "[4] Busca gulosa" << endl
             << "[5] Busca ordenada" << endl
             << "[6] Busca A*" << endl
             << endl;

        cin >> option;
    }

    switch (option)
    {

    case 1:
        clock.start();
        solution = searchTree->backtrackingSearch(maze);
        statesNumber = searchTree->getStatesNumber();
        visitedStatesNumber = searchTree->getVisitedStatesNumber();
        clock.stop();
        printSolution(solution, "Backtracking Search", clock, statesNumber, visitedStatesNumber);
        break;

    case 2:
        clock.start();
        solution = searchTree->breadthFirstSearch(maze);
        clock.stop();
        statesNumber = searchTree->getStatesNumber();
        visitedStatesNumber = searchTree->getVisitedStatesNumber();
        printSolution(solution, "Breadth-First Search", clock, statesNumber, visitedStatesNumber);
        break;

    case 3:
        clock.start();
        solution = searchTree->depthFirstSearch(maze);
        clock.stop();
        statesNumber = searchTree->getStatesNumber();
        visitedStatesNumber = searchTree->getVisitedStatesNumber();
        printSolution(solution, "Depth-First Search", clock, statesNumber, visitedStatesNumber);
        break;

    case 4:
        clock.start();
        solution = searchTree->greedySearch(maze);
        clock.stop();
        statesNumber = searchTree->getStatesNumber();
        visitedStatesNumber = searchTree->getVisitedStatesNumber();
        printSolution(solution, "Greedy Search", clock, statesNumber, visitedStatesNumber);
        break;

    case 5:
        clock.start();
        solution = searchTree->backtrackingSearch(maze);
        clock.stop();
        statesNumber = searchTree->getStatesNumber();
        visitedStatesNumber = searchTree->getVisitedStatesNumber();
        printSolution(solution, "Best-First Search", clock, statesNumber, visitedStatesNumber);
        break;

    case 6:
        clock.start();
        solution = searchTree->backtrackingSearch(maze);
        clock.stop();
        statesNumber = searchTree->getStatesNumber();
        visitedStatesNumber = searchTree->getVisitedStatesNumber();
        printSolution(solution, "A-Star Search", clock, statesNumber, visitedStatesNumber);
        break;

    default:
        break;
    }

    writeOutputFile(maze, searchTree, 0);
    writeOutputFile(maze, searchTree, 1);

    delete maze;
    delete searchTree;

    return 0;
}
