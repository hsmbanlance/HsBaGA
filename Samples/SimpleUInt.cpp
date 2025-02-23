#include "../GA/HsBaGA.hpp"

#include <iostream>

using namespace HsBa::GA;

class UIntIndividual
{
public:
	UIntIndividual() = default;
	UIntIndividual(const UIntIndividual&) = default;
	UIntIndividual(UIntIndividual&&) = default;
	UIntIndividual& operator=(const UIntIndividual&) = default;
	UIntIndividual& operator=(UIntIndividual&&) = default;
	~UIntIndividual() = default;
	static std::bitset<32> encode(const unsigned int value)
	{
		return std::bitset<32>(value);
	}
	static unsigned int decode(const std::bitset<32>& bitset)
	{
		return static_cast<unsigned int>(bitset.to_ulong());
	}
	UIntIndividual(const std::bitset<32>& genes) : genes(genes) {}
	double fitness() const
	{
		return static_cast<double>(decode(genes));
	}
	std::bitset<32> getGenes() const
	{
		return genes;
	}
	double getFitness() const
	{
		return fitness();
	}
private:
	std::bitset<32> genes;
};

int main()
{
	static_assert(Individual<unsigned int, 32, UIntIndividual>);
	std::cout << "SimpleUInt" << std::endl;
	std::cout << "Running Genetic Algorithm..." << std::endl;
	GeneticAlgorithm<unsigned int, 32, UIntIndividual, 100, std::mt19937, GAAlgStopCondition::Generation> ga;
	ga.InitializePopulation(std::bitset<32>{});
	unsigned int result = ga.Run(1000);
	std::cout<<"Result: "<< result << std::endl;
	std::cout << "Gold Result: 4294967295" << std::endl;
	std::cout << "Running Genetic Algorithm with Fitness..." << std::endl;
	GeneticAlgorithm<unsigned int, 32, UIntIndividual, 10, std::mt19937, GAAlgStopCondition::Fitness> ga2;
	ga2.InitializePopulation(std::bitset<32>{});
	unsigned int result2 = ga2.Run(4000000000);
	std::cout << "Result: " << result2 << std::endl;
	std::cout << "Gold Result: 4294967295" << std::endl;
	std::cout << "Running Genetic Algorithm with Time..." << std::endl;
	GeneticAlgorithm<unsigned int, 32, UIntIndividual, 100, std::mt19937, GAAlgStopCondition::Time> ga3;
	ga3.InitializePopulation(std::bitset<32>{});
	unsigned int result3 = ga3.Run(1.0);
	std::cout << "Result: " << result3 << std::endl;
	std::cout << "Gold Result: 4294967295" << std::endl;
	return 0;
}