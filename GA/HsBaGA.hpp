#pragma once

#ifndef HSBA_GA_HPP
#define HSBA_GA_HPP

#ifndef __cpp_concepts
static_assert(false, "Concepts Needed");
#endif // !__cpp_concepts


#include <type_traits>
#include <array>
#include <bitset>
#include <exception>
#include <random>
#include <chrono>
#include <concepts>

#ifdef _DEBUG
#include <iostream>
#endif // _DEBUG

namespace HsBa
{
	namespace GA
	{
		/**
		 * @brief Individual concept
		 * @tparam T encoded type
		 * @tparam TIndividual Individual type
		 * @tparam N encoded size
		 */
		template <typename T, size_t N, typename TIndividual>
		concept Individual = requires(T t, TIndividual individual, std::bitset<N> gene)
		{
			// Encodes T to bitSet
			{ TIndividual::encode(t) } -> std::same_as<std::bitset<N>>;
			// Decodes bitSet to T
			{ TIndividual::decode(gene) } -> std::same_as<T>;
			//constructor from bitset
			{ TIndividual(gene) } -> std::same_as<TIndividual>;
			// Fitness function
			{ individual.fitness() } -> std::convertible_to<double>;
			//Getters
			{ individual.getGenes() } -> std::same_as < std::bitset<N>>;
			{ individual.getFitness() } -> std::convertible_to<double>;
		};
		/**
		 * @brief Stop condition for Genetic Algorithm
		 */
		enum class GAAlgStopCondition
		{
			/**
			 * @brief Stop after a number of generations
			 */
			Generation,  // Stop after a number of generations
			/**
			 * @brief Stop after reaching a certain fitness
			 */
			Fitness,     // Stop after reaching a certain fitness
			/**
			 * @brief Stop after a certain time
			 */
			Time         // Stop after a certain time
		};

		// Genetic Algorithm class

		//GAAlgStopCondition::Generation

		/**
		 * @brief Genetic Algorithm
		 * @tparam T type to be encoded
		 * @tparam TIndividual Individual type
		 * @tparam TRandomEngine Random Engine
		 * @tparam N size of encoded type
		 * @tparam IndiviadulSize size of population
		 * @tparam cond stop condition
		 */
		template<typename T, size_t N, typename TIndividual, size_t IndiviadulSize,
			typename TRandomEngine = std::default_random_engine,
			GAAlgStopCondition cond = GAAlgStopCondition::Generation>
			requires Individual<T, N, TIndividual>
		class GeneticAlgorithm
		{
		private:
			//population
			std::array<TIndividual, IndiviadulSize> population;
			//random engine
			TRandomEngine randomEngine;
			//Selection
			void Selection() {
				//sort population by fitness
				std::sort(population.begin(), population.end(), [](const TIndividual& a, const TIndividual& b) {
					return a.getFitness() > b.getFitness();
					});
			}
			//Crossover
			void Crossover() {
				//select parents, best 2 individuals
				size_t parent1 = 0;
				size_t parent2 = 1;
				//select crossover point
				std::uniform_int_distribution<size_t> dist2(0, N - 1);
				size_t crossoverPoint = dist2(randomEngine);
				//crossover,for new popluations
				std::array<TIndividual, IndiviadulSize> newPopulation;
				for (size_t i = 0; i < IndiviadulSize; i++) {
					std::bitset<N> child1 = population[parent1].getGenes();
					std::bitset<N> child2 = population[parent2].getGenes();
					for (size_t j = crossoverPoint; j < N; j++) {
						bool temp = child1[j];
						child1[j] = child2[j];
						child2[j] = temp;
					}
					newPopulation[i] = TIndividual(child1);
					i++;
					if (i < IndiviadulSize) {
						newPopulation[i] = TIndividual(child2);
					}
				}
			}
			//Mutation
			void Mutation() {
				//select individual
				std::uniform_int_distribution<size_t> dist(0, IndiviadulSize - 1);
				size_t individual = dist(randomEngine);
				//select mutation point
				std::uniform_int_distribution<size_t> dist2(0, N - 1);
				size_t mutationPoint = dist2(randomEngine);
				//mutate
				std::bitset<N> genes = population[individual].getGenes();
				genes[mutationPoint] = !genes[mutationPoint];
				population[individual] = TIndividual(genes);
			}
		public:
			GeneticAlgorithm() = default;
			GeneticAlgorithm(const GeneticAlgorithm&) = delete;
			GeneticAlgorithm(GeneticAlgorithm&&) = delete;
			GeneticAlgorithm& operator=(const GeneticAlgorithm&) = delete;
			GeneticAlgorithm& operator=(GeneticAlgorithm&&) = delete;
			~GeneticAlgorithm() = default;
			/**
			 * @brief Initialize population
			 */
			void InitializePopulation() {
				//initialize random engine
				std::random_device rd;
				randomEngine.seed(rd());
				for (auto& individual : population) {
					//randomly generate genes
					std::bitset<N> genes;
					individual = TIndividual(genes);
				}
			}
			/**
			 * @brief initialize population
			 * @param seed random seed
			 */
			void InitializePopulation(const unsigned int seed) {
				randomEngine.seed(seed);
				for (auto& individual : population) {
					//randomly generate genes
					std::bitset<N> genes;
					for (size_t i = 0; i < N; i++) {
						genes[i] = std::uniform_int_distribution<int>(0, 1)(randomEngine);
					}
					individual = TIndividual(genes);
				}
			}
			/**
			 * @brief initialize population with custom genes
			 * @param genes custom genes
			 */
			void InitializePopulation(const std::bitset<N> genes) {
				//initialize random engine
				std::random_device rd;
				randomEngine.seed(rd());
				for (auto& individual : population) {
					individual = TIndividual(genes);
				}
			}
			/**
			 * @brief initialize population with custom genes
			 * @param genes custom genes
			 * @param seed random seed
			 */
			void InitializePopulation(const std::bitset<N> genes,const unsigned int seed) {
				//initialize random engine
				randomEngine.seed(seed);
				for (auto& individual : population) {
					individual = TIndividual(genes);
				}
			}
			/**
			 * @brief run genetic algorithm
			 * @param generation max generation
			 * @return best individual's decoded value
			 */
			T Run(const size_t generation) {
				for (size_t i = 0; i < generation; i++) {
					Selection();
					Crossover();
					Mutation();
				}
				Selection();
				return population[0].decode(population[0].getGenes());
			}
		};

		// GAAlgStopCondition::Fitness

		/**
		 * @brief Genetic Algorithm with stop condition of reaching a certain fitness
		 * @tparam T type to be encoded
		 * @tparam TIndividual Individual type
		 * @tparam TRandomEngine Random Engine
		 * @tparam N size of encoded type
		 * @tparam IndiviadulSize size of population
		 */
		template<typename T, size_t N, typename TIndividual, size_t IndiviadulSize,
			typename TRandomEngine>
			requires Individual<T, N, TIndividual>
		class GeneticAlgorithm<T, N, TIndividual, IndiviadulSize, TRandomEngine, GAAlgStopCondition::Fitness>
		{
		private:
			//population
			std::array<TIndividual, IndiviadulSize> population;
			//random engine
			TRandomEngine randomEngine;
			//Selection
			void Selection() {
				//sort population by fitness
				std::sort(population.begin(), population.end(), [](const TIndividual& a, const TIndividual& b) {
					return a.getFitness() > b.getFitness();
					});
			}
			//Crossover
			void Crossover() {
				//select parents, best 2 individuals
				size_t parent1 = 0;
				size_t parent2 = 1;
				//select crossover point
				std::uniform_int_distribution<size_t> dist2(0, N - 1);
				size_t crossoverPoint = dist2(randomEngine);
				//crossover,for new popluations
				std::array<TIndividual, IndiviadulSize> newPopulation;
				for (size_t i = 0; i < IndiviadulSize; i++) {
					std::bitset<N> child1 = population[parent1].getGenes();
					std::bitset<N> child2 = population[parent2].getGenes();
					for (size_t j = crossoverPoint; j < N; j++) {
						bool temp = child1[j];
						child1[j] = child2[j];
						child2[j] = temp;
					}
					newPopulation[i] = TIndividual(child1);
					i++;
					if (i < IndiviadulSize) {
						newPopulation[i] = TIndividual(child2);
					}
				}
			}
			//Mutation
			void Mutation() {
				//select individual
				std::uniform_int_distribution<size_t> dist(0, IndiviadulSize - 1);
				size_t individual = dist(randomEngine);
				//select mutation point
				std::uniform_int_distribution<size_t> dist2(0, N - 1);
				size_t mutationPoint = dist2(randomEngine);
				//mutate
				std::bitset<N> genes = population[individual].getGenes();
				genes[mutationPoint] = !genes[mutationPoint];
				population[individual] = TIndividual(genes);
			}
		public:
			GeneticAlgorithm() = default;
			GeneticAlgorithm(const GeneticAlgorithm&) = delete;
			GeneticAlgorithm(GeneticAlgorithm&&) = delete;
			GeneticAlgorithm& operator=(const GeneticAlgorithm&) = delete;
			GeneticAlgorithm& operator=(GeneticAlgorithm&&) = delete;
			~GeneticAlgorithm() = default;
			/**
			 * @brief initialize population
			 */
			void InitializePopulation() {
				//initialize random engine
				std::random_device rd;
				randomEngine.seed(rd());
				for (auto& individual : population) {
					//randomly generate genes
					std::bitset<N> genes;
					for (size_t i = 0; i < N; i++) {
						genes[i] = std::uniform_int_distribution<int>(0, 1)(randomEngine);
					}
					individual = TIndividual(genes);
				}
			}
			/**
			 * @brief initialize population
			 * @param seed random seed
			 */
			void InitializePopulation(const unsigned int seed) {
				randomEngine.seed(seed);
				for (auto& individual : population) {
					//randomly generate genes
					std::bitset<N> genes;
					for (size_t i = 0; i < N; i++) {
						genes[i] = std::uniform_int_distribution<int>(0, 1)(randomEngine);
					}
					individual = TIndividual(genes);
				}
			}
			/**
			 * @brief initialize population with custom genes
			 * @param genes custom genes
			 */
			void InitializePopulation(const std::bitset<N> genes) {
				//initialize random engine
				std::random_device rd;
				randomEngine.seed(rd());
				for (auto& individual : population) {
					individual = TIndividual(genes);
				}
			}
			/**
			 * @brief initialize population with custom genes
			 * @param genes custom genes
			 * @param seed random seed
			 */
			void InitializePopulation(const std::bitset<N> genes,const unsigned int seed) {
				//initialize random engine
				randomEngine.seed(seed);
				for (auto& individual : population) {
					individual = TIndividual(genes);
				}
			}
			/**
			 * @brief run genetic algorithm
			 * @param fitness fitness to reach
			 * @return best individual's decoded value
			 */
			T Run(const double fitness) {
				while (true) {
					Selection();
					if (population[0].getFitness() >= fitness) {
						break;
					}
					Crossover();
					Mutation();
				}
				return population[0].decode(population[0].getGenes());
			}
		};

		// GAAlgStopCondition::Time

		/**
		 * @brief Genetic Algorithm with stop condition of reaching a certain time
		 * @tparam T type to be encoded
		 * @tparam TIndividual Individual type
		 * @tparam TRandomEngine Random Engine
		 * @tparam N size of encoded type
		 * @tparam IndiviadulSize size of population
		 */
		template<typename T, size_t N, typename TIndividual, size_t IndiviadulSize,
			typename TRandomEngine>
			requires Individual<T, N, TIndividual>
		class GeneticAlgorithm<T, N, TIndividual, IndiviadulSize, TRandomEngine, GAAlgStopCondition::Time>
		{
		private:
			//population
			std::array<TIndividual, IndiviadulSize> population;
			//random engine
			TRandomEngine randomEngine;
			//Selection
			void Selection() {
				//sort population by fitness
				std::sort(population.begin(), population.end(), [](const TIndividual& a, const TIndividual& b) {
					return a.getFitness() > b.getFitness();
					});
			}
			//Crossover
			void Crossover() {
				//select parents, best 2 individuals
				size_t parent1 = 0;
				size_t parent2 = 1;
				//select crossover point
				std::uniform_int_distribution<size_t> dist2(0, N - 1);
				size_t crossoverPoint = dist2(randomEngine);
				//crossover,for new popluations
				std::array<TIndividual, IndiviadulSize> newPopulation;
				for (size_t i = 0; i < IndiviadulSize; i++) {
					std::bitset<N> child1 = population[parent1].getGenes();
					std::bitset<N> child2 = population[parent2].getGenes();
					for (size_t j = crossoverPoint; j < N; j++) {
						bool temp = child1[j];
						child1[j] = child2[j];
						child2[j] = temp;
					}
					newPopulation[i] = TIndividual(child1);
					i++;
					if (i < IndiviadulSize) {
						newPopulation[i] = TIndividual(child2);
					}
				}
			}
			//Mutation
			void Mutation() {
				//select individual
				std::uniform_int_distribution<size_t> dist(0, IndiviadulSize - 1);
				size_t individual = dist(randomEngine);
				//select mutation point
				std::uniform_int_distribution<size_t> dist2(0, N - 1);
				size_t mutationPoint = dist2(randomEngine);
				//mutate
				std::bitset<N> genes = population[individual].getGenes();
				genes[mutationPoint] = !genes[mutationPoint];
				population[individual] = TIndividual(genes);
			}
		public:
			GeneticAlgorithm() = default;
			GeneticAlgorithm(const GeneticAlgorithm&) = delete;
			GeneticAlgorithm(GeneticAlgorithm&&) = delete;
			GeneticAlgorithm& operator=(const GeneticAlgorithm&) = delete;
			GeneticAlgorithm& operator=(GeneticAlgorithm&&) = delete;
			~GeneticAlgorithm() = default;
			/**
			 * @brief initialize population
			 */
			void InitializePopulation() {
				//initialize random engine
				std::random_device rd;
				randomEngine.seed(rd());
				for (auto& individual : population) {
					//randomly generate genes
					std::bitset<N> genes;
					for (size_t i = 0; i < N; i++) {
						genes[i] = std::uniform_int_distribution<int>(0, 1)(randomEngine);
					}
					individual = TIndividual(genes);
				}
			}
			/**
			 * @brief initialize population
			 * @param seed random seed
			 */
			void InitializePopulation(const unsigned int seed) {
				randomEngine.seed(seed);
				for (auto& individual : population) {
					//randomly generate genes
					std::bitset<N> genes;
					for (size_t i = 0; i < N; i++) {
						genes[i] = std::uniform_int_distribution<int>(0, 1)(randomEngine);
					}
					individual = TIndividual(genes);
				}
			}
			/**
			 * @brief initialize population with custom genes
			 * @param genes custom genes
			 */
			void InitializePopulation(const std::bitset<N> genes) {
				//initialize random engine
				std::random_device rd;
				randomEngine.seed(rd());
				for (auto& individual : population) {
					individual = TIndividual(genes);
				}
			}
			/**
			 * @brief initialize population with custom genes
			 * @param genes custom genes
			 * @param seed random seed
			 */
			void InitializePopulation(const std::bitset<N> genes,const unsigned int seed) {
				//initialize random engine
				randomEngine.seed(seed);
				for (auto& individual : population) {
					individual = TIndividual(genes);
				}
			}
			/**
			 * @brief run genetic algorithm
			 * @param time stop time
			 * @return best individual's decoded value
			 */
			T Run(const double time) {
				auto start = std::chrono::system_clock::now();
				while (true) {
					auto end = std::chrono::system_clock::now();
					std::chrono::duration<double> elapsed_seconds = end - start;
					if (elapsed_seconds.count() >= time) {
						break;
					}
					Selection();
					Crossover();
					Mutation();
				}
				Selection();
				return population[0].decode(population[0].getGenes());
			}
		};
	}
} // namespace HsBa::GA

#endif // !HSBA_GA_HPP

