/*
"Expert" Data Mining with Hansel Chains

Author: Harlow Huber
Supervisor: Dr. Boris Kovalerchuk
*/

#include <fstream>
#include <iostream>
#include <string>
#include <vector>


int main()
{
	std::cout << "Expert Data Mining with Hansel Chains.\n" << std::endl;

	// binary vectors are from "Consistent and complete data and 'expert' data mining"
	std::vector<std::string> hansel_chains[10];
	hansel_chains[0] = { "01100", "11100" };	
	hansel_chains[1] = { "01010", "11010" };	
	hansel_chains[2] = { "11000", "11001" };	
	hansel_chains[3] = { "10010", "10110" };	
	hansel_chains[4] = { "10100", "10101" };	
	hansel_chains[5] = { "00010", "00110", "01110", "11110" };
	hansel_chains[6] = { "00100", "00101", "01101", "11101" };
	hansel_chains[7] = { "01000", "01001", "01011", "11011" };
	hansel_chains[8] = { "10000", "10001", "10011", "10111" };
	hansel_chains[9] = { "00000", "00001", "00011", "00111", "01111", "11111" };

	std::vector<bool> visited[10];
	visited[0].resize(2);
	visited[1].resize(2);
	visited[2].resize(2);
	visited[3].resize(2);
	visited[4].resize(2);
	visited[5].resize(4);
	visited[6].resize(4);
	visited[7].resize(4);
	visited[8].resize(4);
	visited[9].resize(6);

	// ask expert what class each vector in each chain belongs to
	for (int i = 0; i < 10; i++)
	{
		int size = (int)hansel_chains[i].size();

		for (int j = 0; j < size; j++)
		{
			int vector_class = -1;

			// if vector has not been visited, then ask user class
			// else, retrieve class
			if (!visited[i][j])
			{
				std::cout << "What class is the vector " + hansel_chains[i][j] + "?" << std::endl;
				std::cin >> vector_class;
				std::cin.clear();
				std::cin.ignore(1000, '\n');
				hansel_chains[i][j] += "," + std::to_string(vector_class);
				visited[i][j] = true;
			}
			else vector_class = hansel_chains[i][j][6] - '0'; // convert ASCII 0 OR 1 to integer representation

			// expand vectors
			if (vector_class)
			{
				// expand within chain
				for (int k = j + 1; k < size; k++)
				{
					if (!visited[i][k])
					{
						hansel_chains[i][k] += ",1";
						visited[i][k] = true;
					}
				}

				// expand outside of chain (hardcoded)
				if (i == 0 && j == 0)	// 1.1
				{
					if (!visited[5][2]) // 6.3
					{
						hansel_chains[5][2] += ",1";
						visited[5][2] = true;
					}

					if (!visited[6][2]) // 7.3
					{
						hansel_chains[6][2] += ",1";
						visited[6][2] = true;
					}
				}
				else if (i == 0 && j == 1)	// 1.2
				{
					if (!visited[5][3]) // 6.4
					{
						hansel_chains[5][3] += ",1";
						visited[5][3] = true;
					}

					if (!visited[6][3]) // 7.4
					{
						hansel_chains[6][3] += ",1";
						visited[6][3] = true;
					}
				}
				else if (i == 1 && j == 0)	// 2.1
				{
					if (!visited[5][2]) // 6.3
					{
						hansel_chains[5][2] += ",1";
						visited[5][2] = true;
					}

					if (!visited[7][2]) // 8.3
					{
						hansel_chains[7][2] += ",1";
						visited[7][2] = true;
					}
				}
				else if (i == 1 && j == 1)	// 2.2
				{
					if (!visited[5][3]) // 6.4
					{
						hansel_chains[5][3] += ",1";
						visited[5][3] = true;
					}

					if (!visited[7][3]) // 8.4
					{
						hansel_chains[7][3] += ",1";
						visited[7][3] = true;
					}
				}
				else if (i == 2 && j == 1)	// 3.2
				{
					if (!visited[6][3]) // 7.4
					{
						hansel_chains[6][3] += ",1";
						visited[6][3] = true;
					}

					if (!visited[7][3]) // 8.4
					{
						hansel_chains[7][3] += ",1";
						visited[7][3] = true;
					}
				}
				else if (i == 3 && j == 0)	// 4.1
				{
					if (!visited[8][2]) // 9.3
					{
						hansel_chains[8][2] += ",1";
						visited[8][2] = true;
					}
				}
				else if (i == 3 && j == 1)	// 4.2
				{
					if (!visited[6][3]) // 6.4
					{
						hansel_chains[6][3] += ",1";
						visited[6][3] = true;
					}

					if (!visited[8][3]) // 9.4
					{
						hansel_chains[8][3] += ",1";
						visited[8][3] = true;
					}
				}
				else if (i == 4 && j == 1)	// 5.2
				{
					if (!visited[6][3]) // 7.4
					{
						hansel_chains[6][3] += ",1";
						visited[6][3] = true;
					}

					if (!visited[8][3]) // 9.4
					{
						hansel_chains[8][3] += ",1";
						visited[8][3] = true;
					}
				}
				else if (i == 5 && j == 0)	// 6.1
				{
					if (!visited[9][2]) // 10.3
					{
						hansel_chains[9][2] += ",1";
						visited[9][2] = true;
					}
				}
				else if (i == 5 && j == 1)	// 6.2
				{
					if (!visited[9][3]) // 10.4
					{
						hansel_chains[9][3] += ",1";
						visited[9][3] = true;
					}
				}
				else if (i == 5 && j == 2)	// 6.3
				{
					if (!visited[9][4]) // 10.5
					{
						hansel_chains[9][4] += ",1";
						visited[9][4] = true;
					}
				}
				else if (i == 5 && j == 3)	// 6.4
				{
					if (!visited[9][5]) // 10.6
					{
						hansel_chains[9][5] += ",1";
						visited[9][5] = true;
					}
				}
				else if (i == 6 && j == 0)	// 7.1
				{
					if (!visited[9][3]) // 10.4
					{
						hansel_chains[9][3] += ",1";
						visited[9][3] = true;
					}
				}
				else if (i == 6 && j == 1)	// 7.2
				{
					if (!visited[9][3]) // 10.4
					{
						hansel_chains[9][3] += ",1";
						visited[9][3] = true;
					}
				}
				else if (i == 6 && j == 2)	// 7.3
				{
					if (!visited[9][4]) // 10.5
					{
						hansel_chains[9][4] += ",1";
						visited[9][4] = true;
					}
				}
				else if (i == 6 && j == 3)	// 7.4
				{
					if (!visited[9][5]) // 10.6
					{
						hansel_chains[9][5] += ",1";
						visited[9][5] = true;
					}
				}
				else if (i == 7 && j == 3)	// 8.4
				{
					if (!visited[9][5]) // 10.6
					{
						hansel_chains[9][5] += ",1";
						visited[9][5] = true;
					}
				}
				else if (i == 8 && j == 3)	// 9.4
				{
					if (!visited[9][5]) // 10.6
					{
						hansel_chains[9][5] += ",1";
						visited[9][5] = true;
					}
				}

			}
			else if (!vector_class)
			{
				// expand outside of chain (hardcoded)	
				if (i == 0 && j == 0)	// 1.1
				{
					if (!visited[6][0]) // 7.1
					{
						hansel_chains[6][0] += ",0";
						visited[6][0] =true;
					}

					if (!visited[7][0]) // 8.1
					{
						hansel_chains[7][0] += ",0";
						visited[7][0] = true;
					}
				}
				else if (i == 0 && j == 1)	// 1.2
				{
					if (!visited[4][0]) // 5.1
					{
						hansel_chains[4][0] += ",0";
						visited[4][0] = true;
					}

					if (!visited[2][0]) // 3.1
					{
						hansel_chains[2][0] += ",0";
						visited[2][0] = true;
					}
				}
				else if (i == 1 && j == 0)	// 2.1
				{
					if (!visited[5][0]) // 6.1
					{
						hansel_chains[5][0] += ",0";
						visited[5][0] = true;
					}

					if (!visited[7][0]) // 8.1
					{
						hansel_chains[7][0] += ",0";
						visited[7][0] = true;
					}
				}
				else if (i == 1 && j == 1)	// 2.2
				{
					if (!visited[2][0]) // 3.1
					{
						hansel_chains[2][0] += ",0";
						visited[2][0] = true;
					}

					if (!visited[5][0]) // 6.1
					{
						hansel_chains[5][0] += ",0";
						visited[5][0] = true;
					}
				}
				else if (i == 2 && j == 0)	// 3.1
				{
					if (!visited[7][0]) // 8.1
					{
						hansel_chains[7][0] += ",0";
						visited[7][0] = true;
					}

					if (!visited[8][0]) // 9.1
					{
						hansel_chains[8][0] += ",0";
						visited[8][0] = true;
					}
				}
				else if (i == 2 && j == 1)	// 3.2
				{
					if (!visited[7][1]) // 8.2
					{
						hansel_chains[7][1] += ",0";
						visited[7][1] = true;
					}

					if (!visited[8][1]) // 9.2
					{
						hansel_chains[8][1] += ",0";
						visited[8][1] = true;
					}
				}
				else if (i == 3 && j == 0)	// 4.1
				{
					if (!visited[5][0]) // 6.1
					{
						hansel_chains[5][0] += ",0";
						visited[5][0] = true;
					}

					if (!visited[8][0]) // 9.1
					{
						hansel_chains[8][0] += ",0";
						visited[8][0] = true;
					}
				}
				else if (i == 3 && j == 1)	// 4.2
				{
					if (!visited[5][1]) // 6.2
					{
						hansel_chains[5][1] += ",0";
						visited[5][1] = true;
					}

					if (!visited[4][0]) // 5.1
					{
						hansel_chains[4][0] += ",0";
						visited[4][0] = true;
					}
				}
				else if (i == 4 && j == 0)	// 5.1
				{
					if (!visited[6][0]) // 7.1
					{
						hansel_chains[6][0] += ",0";
						visited[6][0] = true;
					}

					if (!visited[8][0]) // 9.1
					{
						hansel_chains[8][0] += ",0";
						visited[8][0] = true;
					}
				}
				else if (i == 4 && j == 1)	// 5.2
				{
					if (!visited[6][1]) // 7.2
					{
						hansel_chains[6][1] += ",0";
						visited[6][1] = true;
					}

					if (!visited[8][1]) // 9.2
					{
						hansel_chains[8][1] += ",0";
						visited[8][1] = true;
					}
				}
				else if (i == 5 && j == 0)	// 6.1
				{
					if (!visited[9][0]) // 10.1
					{
						hansel_chains[9][0] += ",0";
						visited[9][0] = true;
					}
				}
				else if (i == 5 && j == 1)	// 6.2
				{
					if (!visited[6][0]) // 7.1
					{
						hansel_chains[6][0] += ",0";
						visited[6][0] = true;
					}
				}
				else if (i == 6 && j == 0)	// 7.1
				{
					if (!visited[9][0]) // 10.1
					{
						hansel_chains[9][0] += ",0";
						visited[9][0] = true;
					}
				}
				else if (i == 6 && j == 1)	// 7.2
				{
					if (!visited[9][1]) // 10.2
					{
						hansel_chains[9][1] += ",0";
						visited[9][1] = true;
					}
				}
				else if (i == 6 && j == 2)	// 7.3
				{
					if (!visited[7][1]) // 8.2
					{
						hansel_chains[7][1] += ",0";
						visited[7][1] = true;
					}

					if (!visited[9][1]) // 10.2
					{
						hansel_chains[9][1] += ",0";
						visited[9][1] = true;
					}
				}
				else if (i == 7 && j == 0)	// 8.1
				{
					if (!visited[9][0]) // 10.1
					{
						hansel_chains[9][0] += ",0";
						visited[9][0] = true;
					}
				}
				else if (i == 7 && j == 1)	// 8.2
				{
					if (!visited[9][1]) // 10.2
					{
						hansel_chains[9][1] += ",0";
						visited[9][1] = true;
					}
				}
				else if (i == 7 && j == 2)	// 8.3
				{
					if (!visited[9][2]) // 10.3
					{
						hansel_chains[9][2] += ",0";
						visited[9][2] = true;
					}
				}
				else if (i == 7 && j == 3)	// 8.4
				{
					if (!visited[8][2]) // 9.3
					{
						hansel_chains[8][2] += ",0";
						visited[8][2] = true;
					}
				}
				else if (i == 8 && j == 0)	// 9.1
				{
					if (!visited[9][0]) // 10.1
					{
						hansel_chains[9][0] += ",0";
						visited[9][0] = true;
					}
				}
				else if (i == 8 && j == 1)	// 9.2
				{
					if (!visited[9][1]) // 10.2
					{
						hansel_chains[9][1] += ",0";
						visited[9][1] = true;
					}
				}
				else if (i == 8 && j == 2)	// 9.3
				{
					if (!visited[9][2]) // 10.3
					{
						hansel_chains[9][2] += ",0";
						visited[9][2] = true;
					}
				}
				else if (i == 8 && j == 3)	// 9.4
				{
					if (!visited[9][3]) // 10.4
					{
						hansel_chains[9][3] += ",0";
						visited[9][3] = true;
					}
				}
			}
		}
	}

	// restore monotone Boolean function
	std::string boolFunc = "";

	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < (int)hansel_chains[i].size(); j++)
		{
			if ((hansel_chains[i][j][6] - '0') == 1)
			{
				for (int k = 0; k < 5; k++)
				{
					if (hansel_chains[i][j][k] == '1')
					{
						boolFunc += "x" + std::to_string(k + 1);
					}
				}

				break;
			}
		}

		if (i < 9) boolFunc += " v ";
	}

	// print vectors and monotone Boolean Function
	std::cout << "\nMonotone Boolean Function:\n" + boolFunc << std::endl;
	std::cout << "\nVectors, classes:" << std::endl;

	for (auto hansel_chain : hansel_chains)
	{
		for (auto vector : hansel_chain)
		{
			std::cout << vector << std::endl;
		}
	}

	return EXIT_SUCCESS;
}
