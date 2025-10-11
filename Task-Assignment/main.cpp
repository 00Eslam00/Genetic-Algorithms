// this line is to solve and auto close issue #2
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <fstream>

using namespace std;

double generateRandomDouble(double min, double max) {
    return min + static_cast<double>(rand()) / RAND_MAX * (max - min);
}

class GA {
private:
    int pop_size;
    vector<vector<int>> chromosomes;
    int num_tasks;
    vector<int> task_times;
    int max_time_limit;
    double pm;
    double pc;

public:
    GA(int n, int tasks, int maxTime, double mutationRate, double crossOverRate, vector<int> &taskTimes)
            : pop_size(n), num_tasks(tasks), max_time_limit(maxTime),
              pm(mutationRate), pc(crossOverRate), task_times(taskTimes) {
        chromosomes.resize(pop_size, vector<int>(num_tasks));
    }

    void Initiate_Population() {
        srand(time(0));
        for (auto &chromosome: chromosomes) {
            for (int &gene: chromosome) {
                gene = rand() % 2; // Randomly assign tasks to core 0 or core 1
            }
        }
        sort(chromosomes.begin(), chromosomes.end(),
             [this](const vector<int> &a, const vector<int> &b) {
                 return Fitness(a) > Fitness(b); // Sort in descending order of fitness
             });
    }

    int Fitness(const vector<int> &chromosome) {
        int core0_time = 0, core1_time = 0;

        // Calculate execution times for each core
        for (int i = 0; i < num_tasks; ++i) {
            if (chromosome[i] == 0)
                core0_time += task_times[i];
            else
                core1_time += task_times[i];
        }
        return max(core1_time, core0_time);
    }
    
    void Selection(int k, vector<vector<int>> &selected_parents) {
        // Roulette Wheel Selection based on fitness
        // Only select k bad solutions for crossover and mutation

        vector<int> fitnesses;
        double total_fitness = 0;
        for (const auto &chromosome: chromosomes) {
            int fit = Fitness(chromosome);
            fitnesses.push_back(fit);
            total_fitness += fit;
        }
        for (int i = 0; i < k; ++i) {
            double r = generateRandomDouble(0, 1) * total_fitness;
            double cumulative_fitness = 0;
            for (int j = 0; j < pop_size; ++j) {
                cumulative_fitness += fitnesses[j];
                if (cumulative_fitness >= r) {
                    selected_parents.push_back(chromosomes[j]);
                    break;
                }
            }
        }
    }

    void Crossover(vector<vector<int>> &selected_parents) {
        // Single Point Crossover
        int size = (int) selected_parents.size();
        for (int i = 0; i < size; i += 2) {
            if (i + 1 < size) {
                // Generate a random double between 0 and 1
                double r2 = generateRandomDouble(0, 1);

                // Check if crossover should occur
                if (r2 <= pc) {
                    // Choose a random crossover point
                    int r1 = rand() % num_tasks;

                    // Perform single-point crossover
                    for (int j = r1; j < num_tasks; ++j) {
                        swap(selected_parents[i][j], selected_parents[i + 1][j]);
                    }
                }
            }
        }
    }

    void Mutation(vector<vector<int>> &offspring) {
        for (auto &chromosome: offspring) {
            for (int &gene: chromosome) {
                double ri = generateRandomDouble(0, 1);
                if (ri <= pm) {
                    gene ^= 1;
                }
            }
        }
    }

    void Replacement(vector<vector<int>> &selected_parents) {

        vector<pair<int, vector<int>>> fitness_chromosomes;

        // combine all solutions to select the best
        for (const auto &chromosome: chromosomes) {
            fitness_chromosomes.emplace_back(Fitness(chromosome), chromosome);
        }
        for (const auto &chromosome: selected_parents) {
            fitness_chromosomes.emplace_back(Fitness(chromosome), chromosome);
        }

        sort(fitness_chromosomes.begin(), fitness_chromosomes.end(),
             [](const pair<int, vector<int>> &a, const pair<int, vector<int>> &b) {
                 return a.first < b.first; // Sort in ascending order of fitness
             });

        for (int i = 0; i < pop_size; ++i) {
            chromosomes[i] = fitness_chromosomes[i].second;
        }
    }

    void RunGA(int generations, int elite_count) {

        for (int generation = 0; generation < generations; ++generation) {
            vector<vector<int>> selected_parents;

            Selection(num_tasks - elite_count, selected_parents);

            Crossover(selected_parents);

            Mutation(selected_parents);

            Replacement(selected_parents);
        }
    }

    void DisplayBestSolution(int test) {
        int best_fitness = Fitness(chromosomes[0]);
        const vector<int> *best_solution = &chromosomes[0];
        for (const auto &chromosome: chromosomes) {
            int fitness = Fitness(chromosome);
            if (fitness < best_fitness) {
                best_fitness = fitness;
                best_solution = &chromosome;
            }
        }
        cout << "Testcase# " << test << endl;
        int core0_time = 0, core1_time = 0;
        cout << "Chromosome representation: ";
        for (auto c: *best_solution)
            cout << c << ' ';
        cout << endl;
        cout << "Best Allocation:\n";
        for (int i = 0; i < num_tasks; ++i) {
            if ((*best_solution)[i] == 0) {
                cout << "Task " << i << " -> Core 1\n";
                core0_time += task_times[i];
            } else {
                cout << "Task " << i << " -> Core 2\n";
                core1_time += task_times[i];
            }
        }
        cout << "Core 1 Time: " << core0_time << ", Core 2 Time: " << core1_time << endl;
        cout << "Solution score " << best_fitness << endl;
        cout << "+++++++++++++++++++++++++++++++++++++++++\n";
    }
};


int main() {
    ifstream infile("tasks.txt");
    if (!infile.is_open()) {
        cerr << "Error opening file!" << endl;
        return 1;
    }

    string line;
    if (!getline(infile, line)) {
        cerr << "Error reading the first line for number of test cases!" << endl;
        return 1;
    }

    int test_cases;
    try {
        test_cases = stoi(line);
    } catch (const exception &e) {
        cerr << "Error parsing number of test cases: " << e.what() << endl;
        return 1;
    }


    for (int test = 1; test <= test_cases; test++) {
        int max_time, num_tasks;

        if (!(infile >> max_time >> num_tasks)) {
            cerr << "Error reading max time or number of tasks!" << endl;
            break;
        }

        vector<int> task_times(num_tasks);
        for (int i = 0; i < num_tasks; ++i) {
            if (!(infile >> task_times[i])) {
                cerr << "Error reading task time for task " << i << endl;
                break;
            }
        }

        srand(static_cast<unsigned>(time(nullptr)));

        // Generate random mutation rate between 0.001 and 0.1
        double pm = generateRandomDouble(0.001, 0.1);

        // Generate random crossover rate between 0.4 and 0.7
        double pc = generateRandomDouble(0.4, 0.7);
        int gens_number = (int) generateRandomDouble(0, 9);
        vector<int> pop_sizes(10);
        for (int i = 0; i < 10; ++i) {
            pop_sizes[i] = (i + 1) * 50;
        }
        GA ga(pop_sizes[gens_number], num_tasks, max_time, pm, pc, task_times);
        ga.Initiate_Population();
        int elite = (int) generateRandomDouble(0, num_tasks);
        ga.RunGA(20, elite);
        ga.DisplayBestSolution(test);
    }

    infile.close();
    return 0;
}