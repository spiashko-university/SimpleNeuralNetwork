#pragma once
#include "ILearningStrategey.h"
#include "..\NeuralNetworks\RecurentNeuralNetwork\IRecurentNeuralNetwork.h"
#include "ContrastiveDivergenceAlgorithmConfig.h"
namespace neuralNet {
	class ContrastiveDivergence : public ILearningStrategy<IRecurentNeuralNetwork> {
		ContrastiveDivergenceAlgorithmConfig _config;
		std::ofstream _logger;

	public:
		ContrastiveDivergence();
		// ������������ ����� ILearningStrategy
		virtual void train(IRecurentNeuralNetwork * network, vector<DataItem<float>>& data) override;
	};
	void ContrastiveDivergence::train(IRecurentNeuralNetwork * network, vector<DataItem<float>>& data)
	{

		float currentError = FLT_MAX;
		float lastError = 0;
		int epochNumber = 0;
		_logger << ("CD-k Start learning...") << std::endl;

		vector<float> nablaThresholdsInput(network->InputLayer()->Neurons().size());
		vector<float> nablaThresholdsOutput(network->OutputLayer()->Neurons().size());
		vector<vector<float>> nablaWeights(network->OutputLayer()->Neurons().size(), 
			vector<float>(network->InputLayer()->Neurons().size()));
		

		do{

			//process data set
			int currentIndex = 0;
			do
			{
				for (int i = 0; i < nablaWeights.size(); i++)
				{
					for (int j = 0; j < nablaWeights[i].size(); j++)
					{
						nablaWeights[i][j] = 0;
					}
					nablaThresholdsOutput[i] = 0;
				}
				for (int i = 0; i < nablaThresholdsInput.size(); i++) {
					nablaThresholdsInput[i] = 0;
				}

				vector<float> startInput = data[currentIndex].Input();
				vector<float> startOutput = network->calculateOutput(startInput);

				vector<float> finishInput;
				vector<float> finishOutput;

				vector<float> prevOutput = startOutput;
				vector<float> prevInput = startInput;
				//��������� k ��������
				for (int i = 0; i < _config.getK(); i++) {
					finishInput = network->calculateInput(prevOutput);
					finishOutput = network->calculateOutput(finishInput);
					//���������� � ������
					for (int neuronIndex = 0; neuronIndex < nablaWeights.size(); neuronIndex++) {
						for (int weightIndex = 0; weightIndex < nablaWeights[neuronIndex].size(); weightIndex++) {
							nablaWeights[neuronIndex][weightIndex] +=
								(finishOutput[neuronIndex] - prevOutput[neuronIndex]) *
								(finishInput[weightIndex]) *
								network->OutputLayer()->Neurons()[neuronIndex]->ActivationFunction()->calculateFirstDerivative(
									network->OutputLayer()->Neurons()[neuronIndex]->getLastState())
								+
								(finishInput[weightIndex] - prevInput[weightIndex]) *
								(prevOutput[neuronIndex]) *
								network->InputLayer()->Neurons()[weightIndex]->ActivationFunction()->calculateFirstDerivative(
									network->InputLayer()->Neurons()[weightIndex]->getLastState());
						}
						nablaThresholdsOutput[neuronIndex] += (finishOutput[neuronIndex] - prevOutput[neuronIndex]) *
							network->OutputLayer()->Neurons()[neuronIndex]->ActivationFunction()->calculateFirstDerivative(
								network->OutputLayer()->Neurons()[neuronIndex]->getLastState());
					}
					for (int weightIndex = 0; weightIndex < nablaThresholdsInput.size(); weightIndex++) {
						nablaThresholdsInput[weightIndex] +=
							(finishInput[weightIndex] - prevInput[weightIndex]) *
							network->InputLayer()->Neurons()[weightIndex]->ActivationFunction()->calculateFirstDerivative(
								network->InputLayer()->Neurons()[weightIndex]->getLastState());
					}

					prevOutput = finishOutput;
					prevInput = finishInput;
				}
				
				//������ ������������� �����

			} while (currentIndex < data.size());
			//��������� ������������������ ������

			epochNumber++;
		} while (epochNumber < _config.getMaxEpoches() &&
			currentError > _config.getMinError() &&
			abs(currentError - lastError) > _config.getMinErrorChange());
	}
}