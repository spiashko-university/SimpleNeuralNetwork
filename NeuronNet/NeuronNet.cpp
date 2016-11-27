// NeuronNet.cpp: ���������� ����� ����� ��� ����������� ����������.
//

#include "stdafx.h"

#include"NeuralNetworks\MultilayerNeuralNetwork\MLP.h"
#include"LearningStrategeys\BackpropagationLearningAlgorithm.h";
#include"ActivationFunctions\Relu.h"
#include"ActivationFunctions\Sigmoid.h"
#include"ActivationFunctions\Linear.h"
#include"Data\DataItem.h"
#include "NeuralNetworks\RecurentNeuralNetwork\OLRNN.h"
#include"LearningStrategeys\ContrastiveDivergence.h";
#include "LearningStrategeys\RestrictedBoltzmannMachines.h"
#include"Data\MNISTReader.h"

#include <iomanip>
using namespace neuralNet;

void print(vector<float> v) {
	for (int i = 0; i < v.size(); i++) {
		std::cout << v[i] << " ";
	}
	std::cout << std::endl;
}

void show(DataItem<float> data) {
	for (int i = 0; i < 28; i++, cout << endl)
		for (int j = 0; j < 28; j++)
			if (data.Input()[i * 28 + j] != 0)
				cout << std::setw(4) << data.Input()[i * 28 + j];
			else
				cout << std::setw(4)<<" ";
	int label;
	for (int i = 0; i < data.Output().size(); i++)
		if (data.Output()[i] == 1.)
			label = i;
	cout << "Label: " << label << endl;
}

int main()
{
	vector<DataItem<float>> data;
	//MNISTReader rd;
	//data = rd.LoadData("train-images.idx3-ubyte", "train-labels.idx1-ubyte");
	
	/*
	data.push_back(DataItem<float>({ 0,0,0 }, { 0 }));
	data.push_back(DataItem<float>({ 0,0,1 }, { 1 }));
	data.push_back(DataItem<float>({ 0,1,0 }, { 1 }));
	data.push_back(DataItem<float>({ 0,1,1 }, { 1 }));
	data.push_back(DataItem<float>({ 1,0,0 }, { 0 }));
	data.push_back(DataItem<float>({ 1,0,1 }, { 1 }));
	data.push_back(DataItem<float>({ 1,1,0 }, { 1 }));
	data.push_back(DataItem<float>({ 1,1,1 }, { 1 }));
	*/
	DataItem<float> tmp;

	tmp.Input() = { 0,0 };
	tmp.Output() = { 0 };
	data.push_back(tmp);
	tmp.Input() = { 0,1 };
	tmp.Output() = { 1 };
	data.push_back(tmp);
	tmp.Input() = { 1,0 };
	tmp.Output() = { 1 };
	data.push_back(tmp);
	tmp.Input() = { 1,1 };
	tmp.Output() = { 0 };
	data.push_back(tmp);
	
	MLP mlp(2, { 2 }, 1, new Relu(), new Sigmoid(),
			new BackpropagationLearningAlgorithm()
		//	, new RestrictedBoltzmannMachines()
		);
		//mlp.preTrain(data);
	mlp.save("XOR.txt");

	MLP mlp2;
	mlp2.open("XOR.txt");
	mlp2.save("XOR2.txt");

	/*
	print(mlp.calculateOutput({ 0,0 }));
	print(mlp.calculateOutput({ 0,1 }));
	print(mlp.calculateOutput({ 1,0 }));
	print(mlp.calculateOutput({ 1,1 }));
	*/

    return 0;
}

