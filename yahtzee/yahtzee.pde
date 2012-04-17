#include <LiquidCrystal.h>

/*
0	DIES1  1's
1	DIES2  2's
2	DIES3  3's
3	DIES4  4's
4	DIES5  5's
5	DIES6  6's
6	KIND3  3-of-a-kind
7	KIND4  4-of-a-kind
8	HOUSE  Full house
9	SMLST  Small straight
10	LRGST  Large straight
11	YAHTZ  Yahtzee
12	CHANC  Chance
*/

#define MAXROUNDS 13
#define MAXPLAYERS 9

int scores[MAXROUNDS][MAXPLAYERS];
int nPlayers;
int pTurn;
int nRound;

int rolls[5];     // For determining which to roll {1, 1, 1, 1, 1}
int dice[5];      // For holding the values of dice {0, 0, 0, 0, 0}

//RollDice();
//// values = [3, 3, 4, 1, 2]
//// >>> rolls[0] = 0
//// >>> rolls[1] = 0
//RollDice();
//// values = [3, 3, 5, 2, 3]

const int speaker = 6;
const int funpin = 7;
const int rollpin = 8;
const int d1pin = 9;
const int d2pin = 10;
const int d3pin = 11;
const int d4pin = 12;
const int d5pin = 13;
int holds[] = {d1pin, d2pin, d3pin, d4pin, d5pin};
char *disp[2];

LiquidCrystal lcd(14, 15, 16, 17, 18, 19);



void setup() {
  // Set pin states for buttons
  for (int i = funpin; i <= d5pin; i++)
    pinMode(i, INPUT);
	
  // Initialize LCD (needs work)
  disp[0] = "";
  disp[1] = "";
//  lcd.begin(16, 2);
	
  // Set pin state for speaker
  pinMode(speaker, OUTPUT);	// possibly redundant
	
  // Display welcome message
  // LCD.display("Welcome to\nYahtzee!");
}

void loop() {
  /* Before */
  // Get number of players (needs work
  nPlayers = 1;
	
  // Reset scores
  for (int i = 0; i < nPlayers; i++)
    for (int j = 0; j < MAXROUNDS; j++)
      scores[j][i] = 0;

  /* During */
  // Loop through 13 turns
  for (int round = 0; round < 13; round++) {
    // Loop through each player
    for (int player = 0; player < nPlayers; player++) {
      // Loop through 3 rolls
      for (int roll = 0; roll < 3; roll++) {
        // Roll
        // Display dice
        // Select holds
        // Select roll
      }

      // Choose scoring line
      // Calculate total roll score
      // Update player score
      // Display player score
    }
    // Display full scoreboard
  }

  /* After */
  // Determine winner
  // Check high score
  // Play again (y or n)?
}

int CalculateScore(int pNum) {
	// p1Score = CalculateScore(0);

	int i;
	int upperScore = 0, lowerScore = 0;

	for (i = 0; i < 6; i++)
		upperScore += scores[i][pNum];

	if (upperScore >= 63)
		upperScore += 35;
	
	for (i = 6; i < 13; i++)
		lowerScore += scores[i][pNum];

	return upperScore + lowerScore;
}

void RollDice() {
  for (int i = 0; i < 5; i++) {
    if (rolls[i])
      dice[i] = random(1, 7);
  }
}

void PressDieButton(int dieNumber) {
  rolls[dieNumber] = (rolls[dieNumber] + 1) % 2;
}

void PrintLCD(char *message, int line) {
  disp[line] = message;
  lcd.clear();
  lcd.home();
  lcd.print(disp[0]);
  lcd.setCursor(0, 1);
  lcd.print(disp[1]);
}

int ScoreHand(int hType) {
  int i;
  int score = 0;
  int sum = 0;
  int pips[] = {-1, 0, 0, 0, 0, 0, 0};
  boolean three = false;
  boolean two = false;

  // Simple case for single digits
  if (hType < 6)
    for (i = 0; i < 5; i++)
      if (dice[i] == hType + 1)
        score += hType + 1;
    return score;

  // First total all the dice and count each pip
  // 2 3 3 4 1 -> {-1, 1, 1, 2, 1, 0, 0}
  for (i = 0; i < 5; i++) {
    sum += dice[i];
    pips[dice[i]]++;
  }

  // Lower half, 6 <= htype <= 12
  switch (hType) {
    case  6:  // 3 of a kind
      for (i = 1; i <= 6; i++)
        if (pips[i] == 3)
          return sum;
      return 0;
    case  7:  // 4 of a kind
      for (i = 1; i <= 6; i++)
        if (pips[i] == 4)
          return sum;
      return 0;
    case  8:  // Full house
      for (i = 1; i <= 6; i++)
        if (pips[i] == 3)
          three = true;
        else if (pips[i] == 2)
          two = true;
      if (two && three)
        return 25;
      return 0;
    case  9:  // Small straight
      if (pips[3] && pips[4])
        if((pips[1] && pips[2]) || (pips[2] && pips[5]) || (pips[5] && pips[6]))
          return 30;
      return 0;
    case 10:  // Large straight
      if (pips[2] && pips[3] && pips[4] && pips[5])
        if(pips[1] || pips[6])
          return 40;
      return 0;
    case 11:  // Yahtzee
      for (i = 1; i <= 6; i++)
        if (pips[i] == 5)
          return 50;
      return 0;
    case 12:  // Chance
      return sum;
  }
}

