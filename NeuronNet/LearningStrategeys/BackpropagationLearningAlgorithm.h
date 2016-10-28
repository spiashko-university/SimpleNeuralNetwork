#pragma once
#include "..\stdafx.h"
#include "..\LearningStrategeys\ILearningStrategey.h"
#include"..\NeuralNetworks\MultilayerNeuralNetwork\IMultilayerNeuralNetwork.h"
#include"LearningAlgorithmConfig.h"

namespace neuralNet {
	class BackpropagationLearningAlgorithm : public ILearningStrategy<IMultilayerNeuralNetwork> {
	private:
		LearningAlgorithmConfig _config;
		std::ofstream _logger;

		void shuffle(vector<int>& arr);
	public:
		BackpropagationLearningAlgorithm();
		BackpropagationLearningAlgorithm(LearningAlgorithmConfig config);
		~BackpropagationLearningAlgorithm();
		// ������������ ����� ILearningStrategy
		virtual void train(IMultilayerNeuralNetwork* network, vector<DataItem<float>>& data) override;
	};
	BackpropagationLearningAlgorithm::BackpropagationLearningAlgorithm() {
		std::ostringstream ss;
		time_t seconds = time(NULL); // �������� ������� ����, ���������� � ��������
		ss << "logs(data" << (int)seconds << ").log" << std::endl;
		std::string lol = ss.str();
		std::string way(lol.begin(), lol.end() - 1);
		_logger = std::ofstream(way);
	}
	BackpropagationLearningAlgorithm::BackpropagationLearningAlgorithm(LearningAlgorithmConfig config) {
		std::ostringstream ss;
		time_t seconds = time(NULL); // �������� ������� ����, ���������� � ��������
		ss << "logs(data" << (int)seconds << ").log" << std::endl;
		std::string lol = ss.str();
		std::string way(lol.begin(), lol.end() - 1);
		_logger = std::ofstream(way);

		_config = config;
	}
	BackpropagationLearningAlgorithm::~BackpropagationLearningAlgorithm() {
		_logger.close();
	}
	void BackpropagationLearningAlgorithm::shuffle(vector<int>& arr)
	{
		std::mt19937 _generator(clock());
		std::uniform_real_distribution<double> _urd(0, 1);
		std::uniform_int_distribution<int> _uid(0, arr.size() - 1);

		for (int i = 0; i < arr.size() - 1; i++)
		{
			if (_urd(_generator) >= 0.3)
			{
				int newIndex = _uid(_generator);
				int tmp = arr[i];
				arr[i] = arr[newIndex];
				arr[newIndex] = tmp;
			}
		}
	}
	void neuralNet::BackpropagationLearningAlgorithm::train(IMultilayerNeuralNetwork* network, vector<DataItem<float>>& data)
	{
		network->HiddenLayers()[0]->Neurons()[0]->Weights()[0] = -0.5;
		network->HiddenLayers()[0]->Neurons()[0]->Weights()[1] = 0.5;
		network->HiddenLayers()[0]->Neurons()[1]->Weights()[0] = 0.5;
		network->HiddenLayers()[0]->Neurons()[1]->Weights()[1] = -0.5;

		network->HiddenLayers()[1]->Neurons()[0]->Weights()[0] = 1;
		network->HiddenLayers()[1]->Neurons()[0]->Weights()[1] = 1;

		if (_config.getBatchSize() < 1 || _config.getBatchSize() > data.size())
		{
			_config.setBatchSize(data.size());
		}
		float currentError = FLT_MAX;
		float currRegError;
		float lastError = 0;
		int epochNumber = 0;
		_logger << ("BPA Start learning...") << std::endl;

		//#region initialize accumulated error for batch, for weights and biases

		vector<vector<vector<float>>> nablaWeights(network->HiddenLayers().size()) ;
		vector<vector<float>> nablaThresholds(network->HiddenLayers().size());

		for (int i = 0; i < network->HiddenLayers().size(); i++)
		{
			nablaWeights[i].resize(network->HiddenLayers()[i]->Neurons().size());
			nablaThresholds[i].resize(network->HiddenLayers()[i]->Neurons().size());
			for (int j = 0; j < network->HiddenLayers()[i]->Neurons().size(); j++)
			{
				nablaWeights[i][j].resize(network->HiddenLayers()[i]->Neurons()[j]->Weights().size());
			}
		}

		//#endregion

		vector<int> trainingIndices(data.size());
		for (int i = 0; i < data.size(); i++)
		{
			trainingIndices[i] = i;
		}

		do
		{
			lastError = currentError;
			int dtStart = clock();

			//preparation for epoche		
			if (_config.getBatchSize() > 0)
			{
				shuffle(trainingIndices);
			}

			//process data set
			int currentIndex = 0;
			do
			{
				//��������� ������ ������
				for (int i = 0; i < network->HiddenLayers().size(); i++)
				{
					for (int j = 0; j < network->HiddenLayers()[i]->Neurons().size(); j++)
					{
						for (int k = 0; k < network->HiddenLayers()[i]->Neurons()[j]->Weights().size(); k++)
						{
							nablaWeights[i][j][k] = 0;
						}
						nablaThresholds[i][j] = 0;
					}
				}

					//process one batch
					for (int inBatchIndex = currentIndex; inBatchIndex < (currentIndex + _config.getBatchSize()) && inBatchIndex < data.size(); inBatchIndex++)
					{

						//forward pass
						vector<float> realOutput = network->calculateOutput(data[trainingIndices[inBatchIndex]].Input());


						//backward pass, error propagation
						//last layer
						//.......................................��������� ���������� ����
						for (int j = 0; j < network->HiddenLayers()[network->HiddenLayers().size() - 1]->Neurons().size(); j++)
						{
							network->HiddenLayers()[network->HiddenLayers().size() - 1]->Neurons()[j]->LastError() =
								_config.ErrorFunction()->calculatePartialDerivaitve(
									data[trainingIndices[inBatchIndex]].Output(),
									realOutput, j) *
								network->HiddenLayers()[network->HiddenLayers().size() - 1]->Neurons()[j]->ActivationFunction()->
								calculateFirstDerivative(network->HiddenLayers()[network->HiddenLayers().size() - 1]->Neurons()[j]->getLastSum());

							nablaThresholds[network->HiddenLayers().size() - 1][j] += 
								network->HiddenLayers()[network->HiddenLayers().size() - 1]->Neurons()[j]->LastError();

							for (int i = 0; i < network->HiddenLayers()[network->HiddenLayers().size() - 1]->Neurons()[j]->Weights().size(); i++)
							{
								nablaWeights[network->HiddenLayers().size() - 1][j][i] +=
									network->HiddenLayers()[network->HiddenLayers().size() - 1]->Neurons()[j]->LastError() *
									network->HiddenLayers()[network->HiddenLayers().size() - 1 - 1]->Neurons()[i]->getLastState();

							}
						}


						//hidden layers
						//.......................................��������� ������� �����
						for (int hiddenLayerIndex = network->HiddenLayers().size() - 2; hiddenLayerIndex > 0; hiddenLayerIndex--)
						{
							for (int j = 0; j < network->HiddenLayers()[hiddenLayerIndex]->Neurons().size(); j++)
							{
								network->HiddenLayers()[hiddenLayerIndex]->Neurons()[j]->LastError() = 0;
								for (int k = 0; k < network->HiddenLayers()[hiddenLayerIndex + 1]->Neurons().size(); k++)
								{
									network->HiddenLayers()[hiddenLayerIndex]->Neurons()[j]->LastError() +=
										network->HiddenLayers()[hiddenLayerIndex + 1]->Neurons()[k]->Weights()[j] *
										network->HiddenLayers()[hiddenLayerIndex + 1]->Neurons()[k]->LastError();
								}
								network->HiddenLayers()[hiddenLayerIndex]->Neurons()[j]->LastError() *=
									network->HiddenLayers()[hiddenLayerIndex]->Neurons()[j]->ActivationFunction()->
									calculateFirstDerivative(
										network->HiddenLayers()[hiddenLayerIndex]->Neurons()[j]->getLastSum()
									);

								nablaThresholds[hiddenLayerIndex][j] +=
									network->HiddenLayers()[hiddenLayerIndex]->Neurons()[j]->LastError();

								for (int i = 0; i < network->HiddenLayers()[hiddenLayerIndex]->Neurons()[j]->Weights().size(); i++)
								{
									nablaWeights[hiddenLayerIndex][j][i] +=
										network->HiddenLayers()[hiddenLayerIndex]->Neurons()[j]->LastError() *
										(hiddenLayerIndex > 0 ? 
											network->HiddenLayers()[hiddenLayerIndex - 1]->Neurons()[i]->getLastState() : 
											data[trainingIndices[inBatchIndex]].Input()[i]);
										

								}
							}
						}
						//.......................................��������� ������� �������� ����
						for (int j = 0; j < network->HiddenLayers()[0]->Neurons().size(); j++)
						{
							network->HiddenLayers()[0]->Neurons()[j]->LastError() = 0;
							for (int k = 0; k < network->HiddenLayers()[0 + 1]->Neurons().size(); k++)
							{
								network->HiddenLayers()[0]->Neurons()[j]->LastError() +=
									network->HiddenLayers()[1]->Neurons()[k]->Weights()[j] *
									network->HiddenLayers()[1]->Neurons()[k]->LastError();
							}
							network->HiddenLayers()[0]->Neurons()[j]->LastError() *=
								network->HiddenLayers()[0]->Neurons()[j]->ActivationFunction()->
								calculateFirstDerivative(
									network->HiddenLayers()[0]->Neurons()[j]->getLastSum()
								);

							nablaThresholds[0][j] +=
								network->HiddenLayers()[0]->Neurons()[j]->LastError();

							for (int i = 0; i < network->HiddenLayers()[0]->Neurons()[j]->Weights().size(); i++)
							{
								nablaWeights[0][j][i] +=
									network->HiddenLayers()[0]->Neurons()[j]->LastError() *
									data[trainingIndices[inBatchIndex]].Input()[i];


							}
						}

					}

				//update weights and bias
				for (int layerIndex = 0; layerIndex < network->HiddenLayers().size(); layerIndex++)
				{
					//_logger << "layer: " << layerIndex << std::endl;
					for (int neuronIndex = 0; 
						neuronIndex < network->HiddenLayers()[layerIndex]->Neurons().size(); 
						neuronIndex++)
					{
						
						network->HiddenLayers()[layerIndex]->Neurons()[neuronIndex]->Threshold() =
							network->HiddenLayers()[layerIndex]->Neurons()[neuronIndex]->Threshold()*
							(1 - _config.getRegularizationFactor()) +	//��������� �������������
							_config.getLearningRate() * nablaThresholds[layerIndex][neuronIndex];
						//_logger << "T: "<< network->HiddenLayers()[layerIndex]->Neurons()[neuronIndex]->Threshold() << "\t\t";
						for (int weightIndex = 0; 
							weightIndex < network->HiddenLayers()[layerIndex]->Neurons()[neuronIndex]->Weights().size();
							weightIndex++)
						{
							network->HiddenLayers()[layerIndex]->Neurons()[neuronIndex]->Weights()[weightIndex] =
								network->HiddenLayers()[layerIndex]->Neurons()[neuronIndex]->Weights()[weightIndex]*
								(1 - _config.getRegularizationFactor()) - //��������� �������������
								_config.getLearningRate() * nablaWeights[layerIndex][neuronIndex][weightIndex];
							//_logger << network->HiddenLayers()[layerIndex]->Neurons()[neuronIndex]->Weights()[weightIndex] << "\t\t";
						}
						//_logger << std::endl;
					}
				}

				currentIndex += _config.getBatchSize();

			} while (currentIndex < data.size());

			//recalculating error on all data
			//real error
			currentError = 0;
			for (int i = 0; i < data.size(); i++)
			{
				vector<float> realOutput = network->calculateOutput(data[i].Input());
				currentError += _config.ErrorFunction()->calculate(data[i].Output(), realOutput);
			}
			//regularization term
			
				currRegError = 0;
				for (int layerIndex = 0; layerIndex < network->HiddenLayers().size(); layerIndex++)
				{
					for (int neuronIndex = 0; neuronIndex < network->HiddenLayers()[layerIndex]->Neurons().size(); neuronIndex++)
					{
						for (int weightIndex = 0; weightIndex < network->HiddenLayers()[layerIndex]->Neurons()[neuronIndex]->Weights().size(); weightIndex++)
						{
							currRegError += network->HiddenLayers()[layerIndex]->Neurons()[neuronIndex]->Weights()[weightIndex] *
								network->HiddenLayers()[layerIndex]->Neurons()[neuronIndex]->Weights()[weightIndex];
						}
					}
				}
				currRegError = currRegError / 2;
				
				
			
			epochNumber++;

			_logger << "Eposh #" << epochNumber << std::endl
				<< " finished; current error is " << currentError
				<< " current regularization error is " << currRegError
				<< " Summary error is " << currentError + currRegError
				<< "; it takes: " << (clock() - dtStart) << std::endl;
			
			currentError += currRegError;

		} while (epochNumber < _config.getMaxEpoches() &&
			currentError > _config.getMinError() &&
			abs(currentError - lastError) > _config.getMinErrorChange());
	}
}
