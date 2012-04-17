#include <LiquidCrystal.h>
#include <string.h>

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

#define SERIAL

#define MAXROUNDS 13
#define MAXPLAYERS 9
#define DISP_CLEAR_LINE ((char *)0)
#define DISP_LINE1 (disp[0])
#define DISP_LINE2 (disp[1])

// Inputs
#define NO_INPUT 0
#define D1_INPUT 1
#define D2_INPUT 2
#define D3_INPUT 4
#define D4_INPUT 8
#define D5_INPUT 16
#define RL_INPUT 32

// Up-Down aliases...
#define UP_INPUT 1
#define DN_INPUT 2

// Input shortcuts
#define DICE (D1_INPUT | D2_INPUT | D3_INPUT | D4_INPUT | D5_INPUT)
#define UP_DOWN (UP_INPUT | DN_INPUT)

int scores[MAXROUNDS][MAXPLAYERS];
int nPlayers = 1;
int pTurn;
int nRound;

int rolls[5];     // For determining which to roll {1, 1, 1, 1, 1}
int dice[5];      // For holding the values of dice {0, 0, 0, 0, 0}

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
#ifdef SERIAL
  Serial.begin(9600);
#endif
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
  int score_line;
  int hand_score;
  int input;
  char line1[17], line2[17];

  /* Before */
  // Intro: Show welcome screen until roll is pressed
  PrintLCD("Yahtzee!", "Press Roll ->", true);
  while (true)
    if (GetInput(RL_INPUT) != NO_INPUT)
      break;
  
  // Get number of players (needs work)
  sprintf(line2, " ^  v        %d", nPlayers);
  PrintLCD("How Many Players", line2, true);
  while (true) {
    input = GetInput(UP_DOWN | RL_INPUT);
//    Serial.print("Input: ");
//    Serial.println(input);
    if (input & UP_DOWN) {
      nPlayers = min(MAXPLAYERS, max(1, nPlayers + (DN_INPUT & input ? -1 : 1)));
      sprintf(line2, " ^  v        %d", nPlayers);
      PrintLCD(DISP_LINE1, line2, true);
    }
    else if (input & RL_INPUT) {
//      Serial.println("Breaking...");
      break;
    }
  }

  // Reset scores
  for (int i = 0; i < nPlayers; i++)
    for (int j = 0; j < MAXROUNDS; j++)
      scores[j][i] = 0;

  /* During */
  // Loop through 13 turns
  for (int round = 0; round < MAXROUNDS; round++) {
    // Loop through each player
    for (int player = 0; player < nPlayers; player++) {
      sprintf(line1, "Player %d turn", player + 1);
      PrintLCD(line1, "Press Roll ->", true);
      for (int i = 0; i < 5; i++ )
        rolls[i] = 1;
      while (true)
        if (GetInput(RL_INPUT) > 0)
          break;
      // Loop through 3 rolls
      for (int roll = 0; roll < 3; roll++) {
        // Roll & display dice
        RollDice(roll);
        while (true) {
          input = GetInput(DICE | RL_INPUT);
          if (input & RL_INPUT) {
            Serial.println("Roll Break");
            break;                  // Select roll
          }
          else {
            Serial.println("Die pressed");
            switch (input) {
              case D1_INPUT:  PressDieButton(0); break;
              case D2_INPUT:  PressDieButton(1); break;
              case D3_INPUT:  PressDieButton(2); break;
              case D4_INPUT:  PressDieButton(3); break;
              case D5_INPUT:  PressDieButton(4); break;
            }
          }
        }
      }

      // Choose scoring line
      score_line = ScoreSuggest(player);
      hand_score = ScoreHand(score_line);          // Calculate total roll score
      // Display line/score
      while (true) {
        if (input = GetInput(UP_DOWN)) {
          score_line = min(MAXROUNDS - 1, score_line + (DN_INPUT & input ? 1 : -1));
          hand_score = ScoreHand(score_line);      // Calculate total roll score
          // Display line/score
        }
        else if (GetInput(RL_INPUT) > 0) {
          scores[score_line][player] = hand_score;  // Update player score
          break;
        }
      }

      
      // Display player score
    }
    // Display full scoreboard
  }

  /* After */
  // Determine winner
  // Check high score
  // Play again (y or n)?
}

int ScoreSuggest(int pNum) {
  int hand_score;
  int max_score = 0;
  int suggest;
  
  for (int i = 0; i < MAXROUNDS; i++) {
    if (scores[i][pNum] < 0) {
      hand_score = ScoreHand(i);
      if (hand_score > max_score) {
        max_score = hand_score;
        suggest = i;
       }
    }
  }
  return suggest;
}

int GetPlayerScore(int pNum) {
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

void RollDice(int roll_number) {
  char line1[] = "                ";  // a 16-character buffer for display
  Serial.println("Rolling...");
  for (int i = 0; i < 5; i++) {
    if (rolls[i])
      dice[i] = random(1, 7);
    line1[3 * i + 1] = (char)('0' + dice[i]);
  }
  PrintLCD(line1, DISP_LINE2, roll_number < 3);
}

void PressDieButton(int dieNumber) {
  char holds[2][4] = {" H ", "   "};
  char line2[] = "                ";  // a 16-character buffer for display
  char l2[17];

  memset(l2, 0, 17);
  Serial.print("Press Die: ");
  Serial.println(dieNumber);
  rolls[dieNumber] = (rolls[dieNumber] + 1) % 2;
  for (int i = 0; i < 5; i++) {
    Serial.print(rolls[i]);
    line2[3 * i + 1] = (rolls[dieNumber] == 1 ? ' ' : 'H');
//    sprintf(l2, l2, holds[rolls[0]], holds[rolls[1]], holds[rolls[2]], holds[rolls[3]], holds[rolls[4]]);
  }
  Serial.println("\nHolds");
  Serial.println(line2);
  Serial.println(holds[0]);
  PrintLCD(DISP_LINE1, line2, true);
}

void PrintLCD(char *line1, char *line2, boolean prompt) {
  if (line1 == DISP_CLEAR_LINE)
    disp[0] = "";
  else
    disp[0] = line1;
  if (line2 == DISP_CLEAR_LINE)
    disp[1] = "";
  else
    disp[1] = line2;
#ifdef SERIAL
  Serial.println(disp[0]);
  Serial.println(disp[1]);
  return;
#endif
  lcd.clear();
  lcd.noCursor();
  lcd.home();
  lcd.print(disp[0]);
  lcd.setCursor(0, 1);
  lcd.print(disp[1]);
  if (prompt) {
    lcd.setCursor(15, 1);
    lcd.cursor();
    lcd.blink();
  }
}

int GetInput(int input_mask) {
#ifdef SERIAL
  int key;
  while (Serial.available() < 1)
    ;
  key = Serial.read();
  Serial.print("Key: ");
  Serial.println(key);
  switch (key) {
    case '1': return D1_INPUT & input_mask;
    case '2': return D2_INPUT & input_mask;
    case '3': return D3_INPUT & input_mask;
    case '4': return D4_INPUT & input_mask;
    case '5': return D5_INPUT & input_mask;
    case 'r': return RL_INPUT & input_mask;
  }
#endif
  return NO_INPUT;
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

