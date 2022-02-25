#include <string>
#include <sstream>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <cctype>

using namespace std;

int main()
{
	ifstream OneLengthDictionary;
	string dictionaryFolderPath = "C:\\Users\\paul.gontarz\\OneDrive - Thermo Fisher Scientific\\Desktop\\WordleDictionary\\";
	string filePath = "";
	string word, guess, guessResultString, singleLetter, UPPERguess;
	int guessNum = 0, method = 0, quordleMethod = 0;
	int currentscores[5], guessResultInt[4][5] = { 0,0,0,0,0 }, currentSolutionValue = 0, multiplier = 1;
	int possibilityMatrix[4][243];
	int worstCaseCount[4] = { 0,0,0,0 }, totalBestWorstCase = 0, totalBestWordsLeft = 0, bestGuessIndex = 0, countInGuess = 0, countInSolution = 0;
	vector <string> Solutions[4], Guesses, tempSolutions;
	bool FoundSolution[4] = { false, false, false, false }, inputCorrect = true, keepWord = true, inSolution[4] = { false,false,false,false };
	int ExpectedWordsLeft[4] = { 0,0,0,0 }, BestWordsLeft[4] = { 0,0,0,0 }, quordleWord = 0;

	cout << "Let's play quordle!" << endl;
	cout << "Loading possible solutions and guesses." << endl;
	
	filePath = "quordleGuesses.txt";
	OneLengthDictionary.open(filePath);
	while (getline(OneLengthDictionary, word))
	{
		std::transform(word.begin(), word.end(), word.begin(), [](unsigned char c) { return std::tolower(c); });
		Guesses.push_back(word);
	}
	OneLengthDictionary.close();
	filePath ="quordleSolutions.txt";
	OneLengthDictionary.open(filePath);
	while (getline(OneLengthDictionary, word))
	{
		std::transform(word.begin(), word.end(), word.begin(), [](unsigned char c) { return std::tolower(c); });
		Solutions[0].push_back(word);
		Guesses.push_back(word); //Guesses list does not contain solutions words.
	}
	OneLengthDictionary.close();
	Solutions[3] = Solutions[2] = Solutions[1] = Solutions[0];
	cout << "Loaded " << Guesses.size() << " guess words and " << Solutions[0].size() << ", " << Solutions[1].size() << ", " << Solutions[2].size() << ", " << Solutions[3].size() << " solution words." << endl;
	cout << "Choose method for determining best guess." << endl;
	cout << "(1) for MinMax." << endl;
	cout << "(2) for MinAvg" << endl;
	cin >> method;
	if (method == 1)
		cout << "Ok. Using MinMax" << endl;
	else if (method == 2)
		cout << "Ok. Using MinAvg" << endl;
	else
		return 0;

	cout << "Choose solve strategy." << endl;
	cout << "(1) for Least Total Possible Solutions." << endl;
	cout << "(2) for Fastest Solve single word" << endl;
	cin >> quordleMethod;
	if (quordleMethod == 1)
		cout << "Ok. Using Least Total Possible Solutions" << endl;
	else if (quordleMethod == 2)
		cout << "Ok. Using Fastest Solve single word" << endl;
	else
		return 0;

	//Scoring. Match at location: 2. Present but no match: 1. Not present: 0.
	while (FoundSolution[0] == false || FoundSolution[1] == false || FoundSolution[2] == false || FoundSolution[3] == false)
	{
		guessNum++;
		cout << "Choosing guess #" << guessNum << endl;
		totalBestWorstCase = 0;
		totalBestWordsLeft = 0;
		for (int quordleWord = 0; quordleWord < 4; quordleWord++)
		{
			worstCaseCount[quordleWord] = Solutions[quordleWord].size();
			totalBestWorstCase += worstCaseCount[quordleWord];
			totalBestWordsLeft += Solutions[quordleWord].size() * Solutions[quordleWord].size();
		}
		if (guessNum == 1)
		{
			if (method == 1)
			{
				cout << "Guess: RAISE. Worst case scenario is 168, 168, 168, 168 possible words left.\n";
				guess = Guesses[12191];
			}
			else
			{
				cout << "Guess: ROATE. Expected words remaining: 60, 60, 60, 60.\n";
				guess = Guesses[7642];
			}
		}
		else if (FoundSolution[0] == false && Solutions[0].size() == 1)
		{
			guess = Solutions[0][0];
			UPPERguess = guess;
			std::transform(UPPERguess.begin(), UPPERguess.end(), UPPERguess.begin(), [](unsigned char c) { return std::toupper(c); });
			FoundSolution[0] = true;
			Solutions[0].clear();
			cout << "Eliminating word 1: Guess " << UPPERguess << "." << endl;
		}
		else if (FoundSolution[1] == false && Solutions[1].size() == 1)
		{
			guess = Solutions[1][0];
			UPPERguess = guess;
			std::transform(UPPERguess.begin(), UPPERguess.end(), UPPERguess.begin(), [](unsigned char c) { return std::toupper(c); });
			FoundSolution[1] = true;
			Solutions[1].clear();
			cout << "Eliminating word 2: Guess " << UPPERguess << "." << endl;
		}
		else if (FoundSolution[2] == false && Solutions[2].size() == 1)
		{
			guess = Solutions[2][0];
			UPPERguess = guess;
			std::transform(UPPERguess.begin(), UPPERguess.end(), UPPERguess.begin(), [](unsigned char c) { return std::toupper(c); });
			FoundSolution[2] = true;
			Solutions[2].clear();
			cout << "Eliminating word 3: Guess " << UPPERguess << "." << endl;
		}
		else if (FoundSolution[3] == false && Solutions[3].size() == 1)
		{
			guess = Solutions[3][0];
			UPPERguess = guess;
			std::transform(UPPERguess.begin(), UPPERguess.end(), UPPERguess.begin(), [](unsigned char c) { return std::toupper(c); });
			FoundSolution[3] = true;
			Solutions[3].clear();
			cout << "Eliminating word 4: Guess " << UPPERguess << "." << endl;
		}
		else
		{
			for (int i = 0; i < Guesses.size(); i++) //Go through all Guesses.
			{
				for (quordleWord = 0; quordleWord < 4; quordleWord++)
				{
					inSolution[quordleWord] = false;
					for (int j = 0; j < 243; j++) //Set counts for each possibility to 0.
					{
						possibilityMatrix[quordleWord][j] = 0;
					}
					for (int j = 0; j < Solutions[quordleWord].size(); j++)//Go through all Solutions.
					{
						if (Guesses[i].compare(Solutions[quordleWord][j]) == 0)
							inSolution[quordleWord] = true;
						for (int k = 0; k < 5; k++)
						{
							currentscores[k] = -1; //Reset all scores.
							if (Guesses[i][k] == Solutions[quordleWord][j][k]) //If a value matches, set as 2 (green).
								currentscores[k] = 2;
						}
						for (int k = 0; k < 5; k++)
						{
							if (currentscores[k] != 2)
							{
								countInGuess = 0;
								for (int l = 0; l <= k; l++) //Find how many of the letter are in the word up to that point.
								{
									if (Guesses[i][k] == Guesses[i][l]) //Count how many times the letter is found in the guess up to the letter's point. Needed for multiple occurrences of letter in word.
										countInGuess++;
								}
								countInSolution = 0;
								for (int l = 0; l < 5; l++) //Count number of guesses in solution.
								{
									if (Guesses[i][k] == Solutions[quordleWord][j][l]) //Just counting. Ok to check match locations.
										countInSolution++;
								}
								if (countInGuess <= countInSolution) //Since can't be a match, if letter occurs in solution more or equal to guess, letter in wrong place (yellow). Assign 1. 
									currentscores[k] = 1;
								else //If not 2 and not 1, assign 0 (letter not found).
									currentscores[k] = 0;
							}
						}
						//Convert base-3 number into base-10 number.
						currentSolutionValue = 0;
						multiplier = 1;
						for (int k = 5 - 1; k >= 0; k--)
						{
							currentSolutionValue = currentSolutionValue + (multiplier * currentscores[k]);
							multiplier = multiplier * 3;
						}
						possibilityMatrix[quordleWord][currentSolutionValue]++; //Increment solutions of this score.
					}
				}
				if (method == 1) //MinMax method. Expected to have fewest large guess number results.
				{
					for (quordleWord = 0; quordleWord<4; quordleWord++)
					{
						worstCaseCount[quordleWord] = 0; //Look for what the worst number of words kept can be.
						for (int j = 0; j < 243; j++)
						{
							if (possibilityMatrix[quordleWord][j] > worstCaseCount[quordleWord])
								worstCaseCount[quordleWord] = possibilityMatrix[quordleWord][j];
						}
					}
					if (quordleMethod == 1) //Try to leave the fewest total words left.
					{
						if ((worstCaseCount[0] + worstCaseCount[1] + worstCaseCount[2] + worstCaseCount[3]) < totalBestWorstCase) //If the worst case for this word is better than the bestWorstCase word, take new word.
						{
							totalBestWorstCase = worstCaseCount[0] + worstCaseCount[1] + worstCaseCount[2] + worstCaseCount[3];
							bestGuessIndex = i;
							BestWordsLeft[0] = worstCaseCount[0];
							BestWordsLeft[1] = worstCaseCount[1];
							BestWordsLeft[2] = worstCaseCount[2];
							BestWordsLeft[3] = worstCaseCount[3];
						}
						else if ((worstCaseCount[0] + worstCaseCount[1] + worstCaseCount[2] + worstCaseCount[3]) == totalBestWorstCase)
						{
							if (inSolution[0] == true || inSolution[1] == true || inSolution[2] == true || inSolution[3] == true)
							{
								totalBestWorstCase = worstCaseCount[0] + worstCaseCount[1] + worstCaseCount[2] + worstCaseCount[3];
								bestGuessIndex = i;
								BestWordsLeft[0] = worstCaseCount[0];
								BestWordsLeft[1] = worstCaseCount[1];
								BestWordsLeft[2] = worstCaseCount[2];
								BestWordsLeft[3] = worstCaseCount[3];
							}
						}
					}
					else //Try to find a word as fast as possible.
					{
						for (quordleWord = 0; quordleWord < 4; quordleWord++)
						{
							if (FoundSolution[quordleWord] == false)
							{
								if (worstCaseCount[quordleWord] < totalBestWorstCase)
								{
									totalBestWorstCase = worstCaseCount[quordleWord];
									bestGuessIndex = i;
									BestWordsLeft[0] = worstCaseCount[0];
									BestWordsLeft[1] = worstCaseCount[1];
									BestWordsLeft[2] = worstCaseCount[2];
									BestWordsLeft[3] = worstCaseCount[3];
								}
								else if (worstCaseCount[quordleWord] == totalBestWorstCase && inSolution[quordleWord] == true)
								{
									totalBestWorstCase = worstCaseCount[quordleWord];
									bestGuessIndex = i;
									BestWordsLeft[0] = worstCaseCount[0];
									BestWordsLeft[1] = worstCaseCount[1];
									BestWordsLeft[2] = worstCaseCount[2];
									BestWordsLeft[3] = worstCaseCount[3];
								}
							}
						}
					}
				}
				else //MinAvg method. Expected to produce best average result.
				{
					for (quordleWord = 0; quordleWord < 4; quordleWord++)
					{
						ExpectedWordsLeft[quordleWord] = 0;
						for (int j = 0; j < 243; j++) //Expected words left =  SIGMA((words left) * (probability of that result)).
						{
							if (possibilityMatrix[j] != 0) // Don't perform expensive multiplication opperation if zero.
								ExpectedWordsLeft[quordleWord] += possibilityMatrix[quordleWord][j] * possibilityMatrix[quordleWord][j];
						}
					}
					if (quordleMethod == 1) // Try to leave the fewest total words left.
					{
						if (ExpectedWordsLeft[0] + ExpectedWordsLeft[1] + ExpectedWordsLeft[2] + ExpectedWordsLeft[3] < totalBestWordsLeft)
						{
							totalBestWordsLeft = ExpectedWordsLeft[0] + ExpectedWordsLeft[1] + ExpectedWordsLeft[2] + ExpectedWordsLeft[3];
							bestGuessIndex = i;
							BestWordsLeft[0] = ExpectedWordsLeft[0];
							BestWordsLeft[1] = ExpectedWordsLeft[1];
							BestWordsLeft[2] = ExpectedWordsLeft[2];
							BestWordsLeft[3] = ExpectedWordsLeft[3];
						}
						else if (ExpectedWordsLeft[0] + ExpectedWordsLeft[1] + ExpectedWordsLeft[2] + ExpectedWordsLeft[3] == totalBestWordsLeft)
						{
							if (inSolution[0] == true || inSolution[1] == true || inSolution[2] == true || inSolution[3] == true)
							{
								totalBestWordsLeft = ExpectedWordsLeft[0] + ExpectedWordsLeft[1] + ExpectedWordsLeft[2] + ExpectedWordsLeft[3];
								bestGuessIndex = i;
								BestWordsLeft[0] = ExpectedWordsLeft[0];
								BestWordsLeft[1] = ExpectedWordsLeft[1];
								BestWordsLeft[2] = ExpectedWordsLeft[2];
								BestWordsLeft[3] = ExpectedWordsLeft[3];
							}
						}
					}
					else //Try to find a word as fast as possible.
					{
						for (quordleWord = 0; quordleWord < 4; quordleWord++)
						{
							if (FoundSolution[quordleWord] == false)
							{
								if (ExpectedWordsLeft[quordleWord] < totalBestWordsLeft)
								{
									totalBestWordsLeft = ExpectedWordsLeft[quordleWord];
									bestGuessIndex = i;
									BestWordsLeft[0] = ExpectedWordsLeft[0];
									BestWordsLeft[1] = ExpectedWordsLeft[1];
									BestWordsLeft[2] = ExpectedWordsLeft[2];
									BestWordsLeft[3] = ExpectedWordsLeft[3];
								}
								else if (ExpectedWordsLeft[quordleWord] == totalBestWordsLeft && inSolution[quordleWord] == true)
								{
									totalBestWorstCase = worstCaseCount[quordleWord];
									bestGuessIndex = i;
									BestWordsLeft[0] = ExpectedWordsLeft[0];
									BestWordsLeft[1] = ExpectedWordsLeft[1];
									BestWordsLeft[2] = ExpectedWordsLeft[2];
									BestWordsLeft[3] = ExpectedWordsLeft[3];
								}
							}
						}
					}
				}
			}
			guess = Guesses[bestGuessIndex];
			UPPERguess = guess;
			std::transform(UPPERguess.begin(), UPPERguess.end(), UPPERguess.begin(), [](unsigned char c) { return std::toupper(c); });
			if (method == 1)
				cout << "Guess: " << UPPERguess << ". Worst case scenario is " << BestWordsLeft[0] << ", " << BestWordsLeft[1] << ", " << BestWordsLeft[2] << ", " << BestWordsLeft[3] << " possible words left." << endl;
			else
				cout << "Guess: " << UPPERguess << ". Expected words left: " << round(1.0 * BestWordsLeft[0] / Solutions[0].size()) << ", " << round(1.0 * BestWordsLeft[1] / Solutions[1].size()) << ", " << round(1.0 * BestWordsLeft[2] / Solutions[2].size()) << ", " << round(1.0 * BestWordsLeft[3] / Solutions[3].size()) << "." << endl;
			cout << "Index is: " << bestGuessIndex << endl;
		}
		for (quordleWord = 0; quordleWord < 4; quordleWord++)
		{
			if (FoundSolution[quordleWord] == false)
			{
				cout << "Enter numeric guess result for word " << quordleWord + 1 << ". 2 for green, 1 for yellow, 0 for grey. Press [enter] after last number." << endl;
				cin >> guessResultString;
				inputCorrect = true;
				for (int i = 0; i < 5; i++)
				{
					singleLetter = guessResultString[i];
					guessResultInt[quordleWord][i] = atoi(singleLetter.c_str());
					if (guessResultInt[quordleWord][i] > 2 || guessResultInt < 0)
						inputCorrect = false;
				}

				while (guessResultString.size() != 5 || inputCorrect == false)
				{
					if (guessResultString.size() != 5)
						cout << "You entred a result " << guessResultString.size() << " characters long. You need a 5 letter result. Try again." << endl;
					else
						cout << "You entered a number that wasn't 0, 1, or 2. Try again." << endl;
					cout << "Enter numeric guess result for word " << quordleWord + 1 << ". 2 for green, 1 for yellow, 0 for grey. Press [enter] after last number." << endl;
					cin >> guessResultString;
					inputCorrect = true;
					for (int i = 0; i < 5; i++)
					{
						singleLetter = guessResultString[i];
						guessResultInt[quordleWord][i] = atoi(singleLetter.c_str());
						if (guessResultInt[quordleWord][i] > 2 || guessResultInt < 0)
							inputCorrect = false;
					}
				}
			}
		}
		//Check if we found solution.
		for (quordleWord = 0; quordleWord < 4; quordleWord++)
		{
			if (FoundSolution[quordleWord] == false)
			{
				FoundSolution[quordleWord] = true;
				for (int i = 0; i < 5; i++)
				{
					if (guessResultInt[quordleWord][i] != 2)
					{
						FoundSolution[quordleWord] = false;
						i = 5;
					}
				}
				if (FoundSolution[quordleWord] == true)
				{
					Solutions[quordleWord].clear();
				}
				else
				{
					//Keep only words that match guess result.
					tempSolutions.clear();
					for (int j = 0; j < Solutions[quordleWord].size(); j++)//Go through all Solutions.
					{
						for (int k = 0; k < 5; k++)
						{
							currentscores[k] = -1; //Reset all scores.
							if (guess[k] == Solutions[quordleWord][j][k]) //If a value matches, set as 2 (green).
								currentscores[k] = 2;
						}
						for (int k = 0; k < 5; k++)
						{
							if (currentscores[k] != 2)
							{
								countInGuess = 0;
								for (int l = 0; l <= k; l++) //Find how many of the letter are in the word up to that point.
								{
									if (guess[k] == guess[l]) //Count how many times the letter is found in the guess up to the letter's point. Needed for multiple occurrences of letter in word.
										countInGuess++;
								}
								countInSolution = 0;
								for (int l = 0; l < 5; l++) //Count number of guesses in solution.
								{
									if (guess[k] == Solutions[quordleWord][j][l])
										countInSolution++;
								}
								if (countInGuess <= countInSolution) //Since can't be a match, if letter occurs in solution more or equal to guess, letter in wrong place (yellow). Assign 1. 
									currentscores[k] = 1;
								else //If not 2 and not 1, assign 0 (letter not found).
									currentscores[k] = 0;
							}
						}
						keepWord = true;
						for (int k = 0; k < 5; k++)
						{
							if (guessResultInt[quordleWord][k] != currentscores[k])
								keepWord = false;
						}
						if (keepWord == true)
							tempSolutions.push_back(Solutions[quordleWord][j]);
					}
					//Shrink Solutions.
					Solutions[quordleWord] = tempSolutions;
					cout << "After guess #" << guessNum << ", there are " << Solutions[quordleWord].size() << " possible solutions remaining for word " << quordleWord + 1 << "." << endl;
					for (int i = 0; i < Solutions[quordleWord].size(); i++)
						cout << Solutions[quordleWord][i] << " ";
					cout << endl;
				}
			}
		}
	}

	cout << "Congratulations! You solved the quordle. Enter any input to quit!" << endl;
	cin >> guessResultString;
	return 0;
}