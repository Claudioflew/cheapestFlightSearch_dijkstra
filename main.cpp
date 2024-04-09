/*
 * Description: Demonstrates Dijkstra's algorithm
 * Author: Koichi Nakata
 * Assumes: Program takes data as vector<vector<int>> flights: {{fromApt, toApt, cost}, {fromApt, toApt, cost}, ... }
 *          Each airport has a unique number in a global variable APT_MAP
 * Made on: April 9, 2024
 * Updated: April 9, 2024
 */

#include <iostream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <stack>

using namespace std;

vector<int> askSrcAndDst();
vector<vector<int>> makeAdjMatrix(const vector<vector<int>>&);
int findCheapestFlightWithDijkstra(const vector<vector<int>>&, int, int);

const unordered_map<int, string> APT_MAP {{0, "SFO"}, {1, "SEA"}, {2, "LAX"},
                                          {3, "DEN"}, {4, "ORD"}, {5, "DFW"},
                                          {6, "IAH"}, {7, "ATL"}, {8, "MIA"},
                                          {9, "JFK"}, {10, "BOS"}};

int main() {
    vector<vector<int>> usFlights {{0, 1, 400}, {0, 2, 150}, {0, 4, 600}, {0, 3, 240},
                                   {1, 0, 350}, {1, 4, 500},
                                   {2, 0, 100}, {2, 3, 300}, {2, 9, 1000},
                                   {3, 0, 200}, {3, 2, 360}, {3, 4, 400}, {3, 5, 210}, {3, 6, 300},
                                   {4, 0, 500}, {4, 1, 450}, {4, 3, 420}, {4, 10, 800}, {4, 9, 600},
                                   {5, 3, 150}, {5, 7, 360},
                                   {6, 3, 240}, {6, 7, 400},
                                   {7, 5, 300}, {7, 6, 360}, {7, 8, 180}, {7, 9, 400}, {7, 10, 650},
                                   {8, 7, 100}, {8, 9, 550},
                                   {9, 4, 630}, {9, 2, 900}, {9, 7, 450}, {9, 8, 470}, {9, 10, 100},
                                   {10, 4, 720}, {10, 9, 150}, {10, 7, 550}};

    vector<vector<int>> adjMatrix = makeAdjMatrix(usFlights);

    while (true) {
        vector<int> srcAndDst = askSrcAndDst();
        int answer = findCheapestFlightWithDijkstra(adjMatrix, srcAndDst[0], srcAndDst[1]);

        cout << "Do you want to continue? Press 1 for yes, any other for no: ";
        string input;
        int choice;
        getline(cin, input);
        stringstream convert(input);  // Want to do input validation
        if (convert >> choice && choice == 1) continue;
        else break;
    }
    cout << "\nThank you for using this program. Have a superior day!" << endl;
}

/*
 * askSrcAndDst:
 * Asks a user to input origin airport and destination airport by number, showing airport code list
 * Receives: void
 * Returns: srcAndDst: vector<int> {fromApt, toApt}
 */
vector<int> askSrcAndDst() {
    vector<int> srcAndDst;
    string aptCode = "\n\t\t<Code List of Airports>\n"
                     "\t\t 0: San Francisco (SFO)   1: Seattle (SEA)      2: Los Angeles (LAX)\n"
                     "\t\t 3: Denver (DEN)          4: Chicago (ORD)      5: Dallas (DFW)\n"
                     "\t\t 6: Houston (IAH)         7: Atlanta (ATL)      8: Miami (MIA)\n"
                     "\t\t 9: New York (JFK)       10: Boston (BOS)\n";

    cout << aptCode << endl;

    string input;
    int choice;
    cout << "Please enter your origin airport by number: ";
    while (true) {
        getline(cin, input);
        stringstream convert(input);
        if (convert >> choice && choice >= 0 && choice <= 10) break;
        cout << "Invalid entry.. Please enter an integer between 0 and 10: ";
    }
    srcAndDst.push_back(choice);

    cout << "Please enter your destination airport by number: ";
    while (true) {
        getline(cin, input);
        stringstream convert(input);
        if (convert >> choice && choice >= 0 && choice <= 10 && choice != srcAndDst[0]) break;
        cout << "Invalid entry.. Please enter an integer between 0 and 10: ";
    }
    srcAndDst.push_back(choice);

    return srcAndDst;
}

/*
 * makeAdjMarix:
 * Makes an adjacency matrix representing flight map, converting vector<vector<int>> {fromApt, toApt, cost}
 * Receives: flights: Representing each edge {{fromApt, toApt, cost}, {fromApt, toApt, cost}, ... }
 * Returns: adjMatrix: adjacency matrix
 */

vector<vector<int>> makeAdjMatrix(const vector<vector<int>>& flights) {
    int numOfApt = APT_MAP.size();
    vector<vector<int>> adjMatrix (numOfApt, vector<int>(numOfApt, 0));
    /* adjMatrix looks like
     *    1|2|3
     * 1| 0 0 3
     * 2| 4 0 0
     * 3| 5 0 0
     * From Apt1 to Apt3, costs 3
     * From Apt2 to Apt1, costs 4
     * From Apt3 to Apt2, not connected (0 is disconnected)
     */

    for (auto flight : flights) {
        adjMatrix[flight[0]][flight[1]] = flight[2];
        // adjMatrix[fromApt][toApt] = flightCost
    }
    return adjMatrix;
}

/*
 * findCheapestFlightWithDijkstra:
 * Finds cheapest flight route, using Dijkstra's algorithm
 * Receives: adjMatrix: graph representation, src: origin apt by number, dst: destination apt by number
 * Returns: costs[dst][0]: cheapest cost
 */
int findCheapestFlightWithDijkstra(const vector<vector<int>>& adjMatrix, int src, int dst) {
    int numOfApt = APT_MAP.size();
    vector<vector<int>> costs; // costs = {AptIndex: {Cost, PrevApt}, AptIndex: {Cost, PrevApt}, ... }
    priority_queue<vector<int>, vector<vector<int>>, greater<vector<int>>> minHeap;
    // minHeap = {{Cost, AptIndex}, {Cost, AptIndex}, ... }

    // Initialize all airport with infinity cost and null previous airport except source airport
    for (int i = 0; i < numOfApt; i++) {
        if (i == src) costs.push_back({0, -1});
        else costs.push_back({INT_MAX, -1});
    }

    minHeap.push({costs[src][0], src});

    while (!minHeap.empty()) {
        auto current = minHeap.top(); // {Cost, AptIndex, PrevApt}
        int cost = current[0];
        int currApt = current[1];
        minHeap.pop();

        for (int nextApt = 0; nextApt < numOfApt; nextApt++) {
            if (adjMatrix[currApt][nextApt] != 0) { // If connected
                int flightCost = adjMatrix[currApt][nextApt];
                if (cost + flightCost < costs[nextApt][0]) {
                    costs[nextApt][0] = cost + flightCost; // Update destination cost
                    costs[nextApt][1] = currApt; // Update previous apt
                    minHeap.push({costs[nextApt][0], nextApt});
                }

            }
        }
    }

    // Can't use [] operator because it can modify values and APT_MAP is const
    cout << "\nThe cheapest flight cost from " << APT_MAP.at(src) << " to "
         << APT_MAP.at(dst) << ": " << costs[dst][0] << endl;

    stack<string> route;
    int curr = dst;
    while (curr != src) {
        route.push(APT_MAP.at(curr));
        curr = costs[curr][1]; // Want to go back to the origin airport from the destination
    }

    cout << "The route: " << APT_MAP.at(src);
    while (!route.empty()) {
        cout << " -> " << route.top();
        route.pop();
    }
    cout << endl;
    cout << endl;

    return costs[dst][0];
}

