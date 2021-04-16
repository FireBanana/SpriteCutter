#pragma once
#include "GridViewer.h"
#include <array>
#include <fstream>
#include <string>
#include <thread>

using namespace std::literals::chrono_literals;

struct SCFile
{
	std::string path;
	int divisions;
	std::vector<GridViewer::TileState> state_array;
	std::vector<std::string> collection_list;
	bool is_corrupt;
};

void MakeFile(GridViewer::TileState* state_array, int length, const char* path, int& divisions, const std::vector<std::string>& collection_list)
{
	CREATE_FILE_STRUCT();
	ofn.lpstrFilter = "SpriteCutter Files\0*.sc*\0\0";

	if (GetSaveFileName(&ofn) == TRUE) 
	{
		std::fstream stream;
		stream.open(ofn.lpstrFile, std::ios_base::out | std::ios_base::trunc);

		if (stream.fail())
			throw;

		stream << path << std::endl;
		stream << divisions << std::endl;

		for (int i = 0; i < collection_list.size(); i++)
		{
			stream << collection_list[i] << std::endl;
		}

		stream << "---" << std::endl;

		for (int i = 0; i < length; i++)
		{
			stream << state_array[i].selected << " " << state_array[i].layer << " " << state_array[i].layer_name;
			if (i != length - 1)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              
				stream << std::endl;
		}
	}	
}

SCFile LoadFile()
{
	CREATE_FILE_STRUCT();
	ofn.lpstrFilter = "SpriteCutter Files\0*.sc*\0\0";

	SCFile file;
	file.is_corrupt = false;

	if (GetOpenFileName(&ofn))
	{

		try {
			std::fstream stream;
			stream.open(ofn.lpstrFile, std::ios_base::in);

			if (stream.fail())
				throw;

			std::string init_path;
			int init_div;

			if (!(stream >> init_path))
				throw std::exception("read failure");

			file.path = init_path;
			if (!(stream >> init_div))
				throw std::exception("read failure");

			file.divisions = init_div;

			std::string init_layers;
			stream >> init_layers;

			do
			{
				if (init_layers == "---")
					break;

				file.collection_list.push_back(init_layers);
			} while (stream >> init_layers);

			std::string temp, temp2, temp3;

			while (stream >> temp >> temp2 >> temp3)
			{
				file.state_array.push_back(GridViewer::TileState{ temp == "1", std::stoi(temp2), std::string(temp3) });
			}
		}
		catch (std::exception& e)
		{
			file.is_corrupt = true;
		}
	}

	return file;
}