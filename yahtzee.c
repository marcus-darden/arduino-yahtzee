int scores[10];
int players;

const int d1pin = 8;
const int d2pin = 9;
const int d3pin = 10;
const int d4pin = 11;
const int d5pin = 12;
const int rollpin = 13;
const int speaker = 7;

void setup() {
	// Set pin states for buttons
	for (int i = 8; i < 14; i++)
		pinMode(i, INPUT);
	
	// Initialize LCD (needs work)
	
	// Set pin state for speaker
	pinMode(speaker, OUTPUT);	// possibly redundant
	
	// Display welcome message
	// LCD.display("Welcome to\nYahtzee!");
}

void loop() {
	/* Before */
	// Get number of players (needs work
	players = 1;
	
	// Reset scores
	for (int i = 0; i < players; i++)
		scores[i] = 0;

	/* During */
	// Loop through 13 turns
	for (int turn = 0; turn < 13; turn++) {
		// Loop through each player
		for (int player = 0; player < players; player++) {
			// Loop through 3 rolls
			for (int roll = 0; roll < 3; roll++) {
				// Roll
				// Display dice
				// Select holds
				// Select roll
			}
			
			// Choose scoring line
			// Calculate score
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
