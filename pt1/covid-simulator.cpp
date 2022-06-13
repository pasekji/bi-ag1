#include <iostream>
#include <vector>
#include <queue>
#include <stack>

using namespace std;

class Planet
{
public:
	Planet()
		: target(false), infected(false), canCure(false), nLives(-1), prevPlanet(nullptr), prevPlanet2(nullptr)
	{}
	int id;
	bool target;
	bool infected;
	bool canCure;
	vector<Planet*> neigbours;
	int nLives;
	Planet* prevPlanet;
	Planet* prevPlanet2;
};

void writePath(Planet* target)
{
	stack<int> path;
	Planet* planet = target;
	Planet* nextPlanet = nullptr;
	path.push(target->id);
	while (planet->prevPlanet)
	{
		if (planet->prevPlanet2 && planet->prevPlanet2 != nextPlanet)
		{
			Planet* tmp = planet->prevPlanet2;
			planet->prevPlanet2 = nullptr;
			nextPlanet = planet;
			planet = tmp;
		}
		else
		{
			nextPlanet = planet;
			planet = planet->prevPlanet;
		}
		path.push(planet->id);
	}
	while (!path.empty())
	{
		cout << path.top() << " ";
		path.pop();
	}
}

Planet* planets;
Planet* target;
int l;

Planet* readInput()
{
	int N;
	int M;
	int s;
	int t;
	int K;
	int D;

	cin >> N >> M;
	planets = new Planet[N];
	cin >> s >> t >> l;
	for (int i = 0; i < N; i++)
	{
		planets[i].id = i;
	}
	cin >> K;
	for (int i = 0; i < K; i++)
	{
		int planetId;
		cin >> planetId;
		planets[planetId].infected = true;
	}
	cin >> D;
	for (int i = 0; i < D; i++)
	{
		int planetId;
		cin >> planetId;
		planets[planetId].canCure = true;
	}
	for (int i = 0; i < M; i++)
	{
		int x, y;
		cin >> x;
		cin >> y;
		planets[x].neigbours.push_back(&planets[y]);
		planets[y].neigbours.push_back(&planets[x]);
	}
	planets[t].target = true;
	target = &planets[t];
	return &planets[s];
}

int main()
{
	Planet* start = readInput();
	start->nLives = l;
	queue<Planet*> queueSafe;
	queue<Planet*> queueInfected;
	queueSafe.push(start);
	bool pathFound = false;
	bool infected = false;
	while (!pathFound)
	{
		queue<Planet*>& currentQueue = infected ? queueInfected : queueSafe;
		if (currentQueue.empty()) break;
		// Pokus najit cestu bez nakazy
		while (!currentQueue.empty() && !pathFound)
		{
			Planet* planet = currentQueue.front();
			currentQueue.pop();
			for (auto& neighbour : planet->neigbours)
			{
				int nextNumberOfLives = neighbour->canCure ? l :
					infected ? planet->nLives - 1 : planet->nLives;
				if (neighbour->nLives < nextNumberOfLives)
				{
					neighbour->nLives = nextNumberOfLives;
					if (!neighbour->prevPlanet)
						neighbour->prevPlanet = planet;
					else
						neighbour->prevPlanet2 = planet;
					if ((!infected || (planet->nLives > 1)) && neighbour->target)
					{
						pathFound = true;
						break;
					}
					if (!infected)
					{
						if (neighbour->infected)
						{
							//neighbour->nLives = planet->nLives - 1;
							queueInfected.push(neighbour);
						}
						else
							queueSafe.push(neighbour);
					}
					else
					{
						if (neighbour->canCure)
							queueSafe.push(neighbour);
						else if (planet->nLives > 1)
						{
							queueInfected.push(neighbour);
							//neighbour->nLives = planet->nLives - 1;
						}
					}
				}
			}
		}
		infected = !infected;
	}
	if (target->prevPlanet)
	{
		writePath(target);
	}
	else
		cout << -1 << endl;

	delete[] planets;
	return 0;
}
