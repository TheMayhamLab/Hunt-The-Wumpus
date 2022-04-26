
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN    PB15

#define LEFT_SWITCH PB14
#define RIGHT_SWITCH PB13

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 3

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)

// constant value 2d array to represent the dodecahedron
// room structure
const static int adjacentRooms[20][3] = {
  {1, 4, 7},   {0, 2, 9},   {1, 3, 11},   {2, 4, 13},    {0, 3, 5},
  {4, 6, 14},  {5, 7, 16},    {0, 6, 8},   {7, 9, 17},   {1, 8, 10},
  {9, 11, 18}, {2, 10, 12}, {11, 13, 19},  {3, 12, 14},  {5, 13, 15},
  {14, 16, 19}, {6, 15, 17},  {8, 16, 18}, {10, 17, 19}, {12, 15, 18}
};

int lightMode;
int unlocked;
int unlockarraypos;
// 1 indicates a left click, 2 indicates a right click
int fibbonaci[33] = {1, 2, 1, 1, 2, 2, 2, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

void KevClearSerial();
int KevGetSerialInt();
char KevGetSerialChar();

int LeftClick()
{
  // Buttons use pullups so 0 is pressed
  int buttonState;
  buttonState = digitalRead(LEFT_SWITCH);
  return ! buttonState;
}

int RightClick()
{
  // Buttons use pullups so 0 is pressed
  int buttonState;
  buttonState = digitalRead(RIGHT_SWITCH);
  return ! buttonState ;
}

void LightsOff()
{
  colorWipe(strip.Color(0, 0, 0), 50);
}

void CheckLights()
{
  if (unlocked)
  {
    if (LeftClick())
    {
      CycleLights();
    }
    else if (RightClick())
    {
      LightsOff();
    }
  }
  else
  {
    // is left or right pressed?
    if (LeftClick())
    {
      if (fibbonaci[unlockarraypos] == 1)
      {
        colorWipe(strip.Color(  0, 255,   0), 5); // Green
        delay(200);
        colorWipe(strip.Color(  0, 0,   0), 5); // black
        unlockarraypos++;
      }
      else
      {
        colorWipe(strip.Color(  255, 0,   0), 5); // Red
        delay(200);
        colorWipe(strip.Color(  0, 0,   0), 5); // black
        delay(200);
        colorWipe(strip.Color(  255, 0,   0), 5); // Red
        delay(200);
        colorWipe(strip.Color(  0, 0,   0), 5); // black
        unlockarraypos = 0;
      }
    }
    else if (RightClick())
    {
      if (fibbonaci[unlockarraypos] == 2)
      {
        colorWipe(strip.Color(  0, 255,   0), 5); // Green
        delay(200);
        colorWipe(strip.Color(  0, 0,   0), 5); // black
        unlockarraypos++;
      }
      else
      {
        colorWipe(strip.Color(  255, 0,   0), 5); // Red
        delay(200);
        colorWipe(strip.Color(  0, 0,   0), 5); // black
        delay(200);
        colorWipe(strip.Color(  255, 0,   0), 5); // Red
        delay(200);
        colorWipe(strip.Color(  0, 0,   0), 5); // black
        unlockarraypos = 0;
      }
    }
    if (unlockarraypos > 32 )
    {
      rainbow(1);
      unlocked = 1;
    }
  }
  delay(50);
}

class WumpusGame {

  private:
    // Data Members
    int numRooms;
    int currentRoom, startingPosition; // currentRoom is an integer variable that stores the room the player is currently in (between 0-20)
    int wumpusRoom, batRoom1, batRoom2, pitRoom1, pitRoom2; // Stores the room numbers of the respective
    int wumpusStart, bat1Start, bat2Start;
    bool playerAlive, wumpusAlive; // Are the player and wumpus still alive? True or false.
    int numArrows; //store arrow count

    // private functions
    void PlacePits();
    void PlaceBats();
    void PlaceWumpus();
    void PlacePlayer();
    bool IsValidMove(int);
    bool IsRoomAdjacent(int, int);
    int Move(int);
    void InspectCurrentRoom();
    void PerformAction(int);
    void MoveStartledWumpus(int);
    void PlayGame();
    void PlayAgain();
    void PrintInstructions();

    // Access specifier
  public:
    //public functions
    void StartGame();
    WumpusGame();
};

//#include <iostream>
//#include <stdlib.h>     /* srand, rand */
//#include <time.h>       /* time */
//#include <vector>
//#include <cstring>
using namespace std;

// default constructor
WumpusGame::WumpusGame() {
  numRooms = 20;
}

// This function prints the instructions for the game
// to the console
void WumpusGame::PrintInstructions() {
  char wait1;
  KevClearSerial();
  Serial.println( " \n\nWelcome to 'Hunt the Wumpus'! " );
  Serial.println( " The wumpus lives in a cave of 20 rooms. Each room has 3 tunnels leading to" );
  Serial.println( " other rooms. (Look at a dodecahedron to see how this works - if you don't know" );
  Serial.println( " what a dodecahedron is, ask someone). \n" );
  Serial.println( " Hazards: \n" );
  Serial.println( " Bottomless pits - two rooms have bottomless pits in them. If you go there, you " );
  Serial.println( " fall into the pit (& lose!) \n" );
  Serial.println( " Super bats - two other rooms have super bats.  If you go there, a bat grabs you" );
  Serial.println( " and takes you to some other room at random. (Which may be troublesome). Once the" );
  Serial.println( " bat has moved you, that bat moves to another random location on the map.\n\n" );

  Serial.println( " Wumpus" );
  Serial.println( " The wumpus is not bothered by hazards (he has sucker feet and is too big for a" );
  Serial.println( " bat to lift).  Usually he is asleep.  Two things wake him up: you shooting an" );
  Serial.println( " arrow or you entering his room. If the wumpus wakes he moves (p=.75) one room or " );
  Serial.println( " stays still (p=.25). After that, if he is where you are, he eats you up and you lose!\n" );

  Serial.println( " You \n" );
  Serial.println( " Each turn you may move, save or shoot an arrow using the commands move, save, & shoot." );
  Serial.println( " Moving: you can move one room (thru one tunnel)." );
  Serial.println( " Arrows: you have 5 arrows. You lose when you run out. You aim by telling the" );
  Serial.println( " computer the rooms you want the arrow to go to.  If the arrow can't go that way" );
  Serial.println( " (if no tunnel), the arrow will not fire." );

  Serial.println( " Warnings" );
  Serial.println( " When you are one room away from a wumpus or hazard, the computer says:" );

  Serial.println( " Wumpus: 'I smell a wumpus'" );
  Serial.println( " Bat: 'Bats nearby'" );
  Serial.println( " Pit: 'I feel a draft'" );

  Serial.println();
  Serial.println( "Press Y to return to the main menu." );
  wait1 = KevGetSerialChar();

}

// This function will place two bats throughout the map
// this ensures that the bats will not be place in the same
// room as another bat or the wumpus
void WumpusGame::PlaceBats() {
  //srand (time(NULL));
  bool validRoom = false;
  while (!validRoom) {
    batRoom1 = rand() % 20 + 1;
    if (batRoom1 != wumpusRoom)
      validRoom = true;
  }

  validRoom = false;
  while (!validRoom) {
    batRoom2 = rand() % 20 + 1;
    if (batRoom2 != wumpusRoom && batRoom2 != batRoom1)
      validRoom = true;
  }
  bat1Start = batRoom1;
  bat2Start = batRoom2;
}

// this function randomly places the pits
// throughout the map excluding room 0
void WumpusGame::PlacePits() {
  //srand (time(NULL));
  pitRoom1 = rand() % 20 + 1;
  pitRoom2 = rand() % 20 + 1;
}

// this function randomly places the wumpus in a room
// without being in room number 0
void WumpusGame::PlaceWumpus() {
  //srand (time(NULL));
  int randomRoom = rand() % 20 + 1;
  wumpusRoom = randomRoom;
  wumpusStart = wumpusRoom;
}

// place the player in room 0
void WumpusGame::PlacePlayer() {
  startingPosition = 0;
  currentRoom = Move(0);
}

// This is a  method that checks if the user inputted a valid room to move to or not.
// The room number has to be between 0 and 20, but also must be adjacent to the current room.
bool WumpusGame::IsValidMove(int roomID) {
  if (roomID < 0) return false;
  if (roomID > numRooms) return false;
  if (!IsRoomAdjacent(currentRoom, roomID)) return false;

  return true;
}

// This method returns true if roomB is adjacent to roomA, otherwise returns false.
// It is a helper method that loops through the adjacentRooms array to check.
// It will be used throughout the app to check if we are next to the wumpus, bats, or pits
// as well as check if we can make a valid move.
bool WumpusGame::IsRoomAdjacent(int roomA, int roomB)
{
  for (int j = 0; j < 3; j++)
  {
    if (adjacentRooms[roomA][j] == roomB) {
      return true;
    }
  }
  return false;
}

// This method moves the player to a new room and returns the new room. It performs no checks on its own.
int WumpusGame::Move(int newRoom)
{
  return newRoom;
}

// Inspects the current room.
// This method check for Hazards such as being in the same room as the wumpus, bats, or pits
// It also checks if you are adjacent to a hazard and handle those cases
// Finally it will just print out the room description
void WumpusGame::InspectCurrentRoom() {
  //srand (time(NULL));
  if (currentRoom == wumpusRoom)
  {
    KevClearSerial();
    Serial.println( "The Wumpus ate you!!!" );
    Serial.println( "LOSER!!!" );
    PlayAgain();
  }
  else if (currentRoom == batRoom1 || currentRoom == batRoom2)
  {
    int roomBatsLeft = currentRoom;
    bool validNewBatRoom = false;
    bool isBatRoom = false;
    KevClearSerial();
    Serial.println( "Snatched by superbats!!" );
    if (currentRoom == pitRoom1 || currentRoom == pitRoom2)
      Serial.println( "Luckily, the bats saved you from the bottomless pit!!" );
    while (!isBatRoom) {
      currentRoom = Move(rand() % 20 + 1);
      if (currentRoom != batRoom1 && currentRoom != batRoom2)
        isBatRoom = true;
    }
    KevClearSerial();
    Serial.print( "The bats moved you to room ");
    Serial.println( currentRoom );
    InspectCurrentRoom();

    if (roomBatsLeft == batRoom1) {
      while (!validNewBatRoom) {
        batRoom1 = rand() % 19 + 1;
        if (batRoom1 != wumpusRoom && batRoom1 != currentRoom)
          validNewBatRoom = true;
      }
    } else {
      while (!validNewBatRoom) {
        batRoom2 = rand() % 19 + 1;
        if (batRoom2 != wumpusRoom && batRoom2 != currentRoom)
          validNewBatRoom = true;
      }
    }
  }
  else if (currentRoom == pitRoom1 || currentRoom == pitRoom2)
  {
    KevClearSerial();
    Serial.println( "YYYIIIIIEEEEE.... fell in a pit!!!" );
    Serial.println( "GAME OVER LOSER!!!" );
    PlayAgain();
  }
  else
  {
    KevClearSerial();
    Serial.print ("\nYou are in room ");
    Serial.println( currentRoom );
    if (IsRoomAdjacent(currentRoom, wumpusRoom)) {
      Serial.println( "** You smell a horrid stench... **" );
    }
    if (IsRoomAdjacent(currentRoom, batRoom1) || IsRoomAdjacent(currentRoom, batRoom2)) {
      Serial.println( "** Bats nearby... **" );
    }
    if (IsRoomAdjacent(currentRoom, pitRoom1) || IsRoomAdjacent(currentRoom, pitRoom2)) {
      Serial.println( "** You feel a draft... **" );
    }
    Serial.println( "Tunnels lead to rooms " );
    for (int j = 0; j < 3; j++)
    {
      Serial.print (adjacentRooms[currentRoom][j]);
      Serial.print ( " ");
    }
    Serial.println();
  }
}

// Method accepts an int which is the command the user inputted.
// This method performs the action of the command or prints out an error.
void WumpusGame::PerformAction(int cmd) {
  int newRoom;
  switch (cmd)
  {

    case 1:
      KevClearSerial();
      Serial.println( "Which room? " );
      //						try
      //						{
      newRoom = KevGetSerialInt();
      // Check if the user inputted a valid room id, then simply tell the player to move there.
      if (IsValidMove(newRoom))
      {
        currentRoom = Move(newRoom);
        InspectCurrentRoom();
      }
      else
      {
        Serial.println( "You cannot move there." );
        Serial.println( "Tunnels lead to rooms " );
        for (int j = 0; j < 3; j++)
        {
          Serial.print (adjacentRooms[currentRoom][j]);
          Serial.print ( " ");
        }
        Serial.println();
      }
      //						}
      //						catch (...) // Try...Catch block will catch if the user inputs text instead of a number.
      //						{
      //								Serial.println( "You cannot move there." );
      //						}
      break;
    case 2:
      if (numArrows > 0) {
        KevClearSerial();
        Serial.println( "Which room? " );
        //								try
        //								{
        newRoom = KevGetSerialInt();
        // Check if the user inputted a valid room id, then simply tell the player to move there.
        if (IsValidMove(newRoom))
        {
          numArrows--;
          if (newRoom == wumpusRoom) {
            KevClearSerial();
            Serial.println( "ARGH.. Splat!" );
            Serial.println( "Congratulations! You killed the Wumpus! You Win." );
            Serial.println( "Press 'Y' to return to the main menu." );
            wumpusAlive = false;
            newRoom = KevGetSerialInt();
            KevClearSerial();
            KevClearSerial();
          }
          else
          {
            KevClearSerial();
            Serial.println( "Miss! But you startled the Wumpus" );
            MoveStartledWumpus(wumpusRoom);
            Serial.print ( "Arrows Left: ");
            Serial.println( numArrows );
            if (wumpusRoom == currentRoom) {
              Serial.println( "The wumpus attacked you! You've been killed." );
              Serial.println( "Game Over!" );
              PlayAgain();
            }

          }
        }
        else
        {
          KevClearSerial();
          Serial.println( "You cannot shoot there." );
          Serial.println( "Tunnels lead to rooms " );
          for (int j = 0; j < 3; j++)
          {
            Serial.print (adjacentRooms[currentRoom][j]);
            Serial.print ( " ");
          }
          Serial.println();
        }
        //								}
        //								catch (...) // Try...Catch block will catch if the user inputs text instead of a number.
        //								{
        //										Serial.println( "You cannot shoot there." );
        //								}
      } else
      {
        KevClearSerial();
        Serial.println( "You do not have any arrows!" );
      }
      break;
    case 3:
      KevClearSerial();
      Serial.println( "Quitting the current game." );
      playerAlive = false;
      break;
    default:
      KevClearSerial();
      Serial.println( "You cannot do that. You can move, shoot, save or quit." );
      break;
  }
}

// this function moves the wumpus randomly to a room that is adjacent to
// the wumpus's current position
void WumpusGame::MoveStartledWumpus(int roomNum) {
  //srand (time(NULL));
  int rando = rand() % 3;
  if (rando != 3)
    wumpusRoom = adjacentRooms[roomNum][rando];
}

// This restarts the map from the begiinning
void WumpusGame::PlayAgain() {
  char reply;
  KevClearSerial();
  Serial.println( "Would you like to replay the same map? Enter Y to play again." );
  reply = KevGetSerialChar();
  if (reply == 'y' || reply == 'Y') {
    currentRoom = startingPosition;
    wumpusRoom = wumpusStart;
    batRoom1 = bat1Start;
    batRoom2 = bat2Start;
    KevClearSerial();
    Serial.println( "Try not to die this time. \n" );
    InspectCurrentRoom();
  } else {
    playerAlive = false;
  }

}

// PlayGame() method starts up the game.
// It houses the main game loop and when PlayGame() quits the game has ended.
void WumpusGame::PlayGame()
{
  int choice;
  bool validChoice = false;

  KevClearSerial();
  Serial.println( "Running the game..." );

  // Initialize the game
  PlaceWumpus();
  PlaceBats();
  PlacePits();
  PlacePlayer();

  // game set up
  playerAlive = true;
  wumpusAlive = true;
  numArrows = 5;

  //Inspects the initial room
  InspectCurrentRoom();

  // Main game loop.
  while (playerAlive && wumpusAlive)
  {
    KevClearSerial();
    Serial.println( "Enter an action choice." );
    Serial.println( "1) Move" );
    Serial.println( "2) Shoot" );
    Serial.println( "3) Quit" );
    Serial.print( ">>> ");

    do
    {
      validChoice = true;
      KevClearSerial();
      Serial.print ( "Please make a selection: ");
      //						try
      //						{
      choice = KevGetSerialInt();
      switch (choice)
      {
        case 1:
          PerformAction(choice);
          break;
        case 2:
          PerformAction(choice);
          break;
        case 3:
          PerformAction(choice);
          break;
        default:
          validChoice = false;
          KevClearSerial();
          Serial.println( "Invalid choice. Please try again." );
          Serial.println( "1) Move" );
          Serial.println( "2) Shoot" );
          Serial.println( "3) Quit" );
          Serial.print( ">>> ");
          KevClearSerial();
          break;
      }
      //						}
      //						catch (...)
      //						{
      //								validChoice = false;
      //								Serial.println( "Invalid choice. Please try again." );
      //								KevClearSerial();
      //								KevClearSerial();
      //						}

    } while (validChoice == false);
  }
}

// this function begins the game loop
void WumpusGame::StartGame() {

  //srand (time(NULL));
  int choice;
  bool validChoice;
  bool keepPlaying;
  wumpusStart = bat1Start = bat2Start = -1;

  do {
    keepPlaying = true;
    KevClearSerial();
    Serial.println( "Welcome to Hunt The Wumpus." );
    Serial.println( "1) Play Game" );
    Serial.println( "2) Print Instructions" );
    Serial.println( "3) Quit" );

    do
    {
      validChoice = true;
      KevClearSerial();
      Serial.println( "Please make a selection: ");
      //					try
      //					{
      choice = KevGetSerialInt();
      switch (choice)
      {
        case 1:
          PlayGame();
          break;
        case 2:
          PrintInstructions();
          break;
        case 3:
          KevClearSerial();
          Serial.println( "Quitting." );
          keepPlaying = false;
          break;
        default:
          validChoice = false;
          KevClearSerial();
          Serial.println( "Invalid choice. Please try again." );
          Serial.println( "\nWelcome to Hunt The Wumpus." );
          Serial.println( "1) Play Game" );
          Serial.println( "2) Print Instructions" );
          Serial.println( "3) Quit" );
          KevClearSerial();
          break;
      }
      //					}
      //					catch (...)
      //					{
      //							validChoice = false;
      //  						Serial.println( "Invalid choice. Please try again." );
      //							KevClearSerial();
      //							KevClearSerial();
      //					}

    } while (validChoice == false);
  } while (keepPlaying);
}

void CycleLights()
{
  lightMode = lightMode + 1;
  switch (lightMode)
  {
    case 1:
      colorWipe(strip.Color(255,   0,   0), 50); // Red
      break;
    case 2:
      colorWipe(strip.Color(  0, 255,   0), 50); // Green
      break;
    case 3:
      colorWipe(strip.Color(  0,   0, 255), 50); // Blue
      break;
    case 4:
      theaterChase(strip.Color(127, 127, 127), 20); // White, half brightness
      break;
    case 5:
      theaterChase(strip.Color(127,   0,   0), 20); // Red, half brightness
      break;
    case 6:
      rainbow(1);             // Flowing rainbow cycle along the whole strip
      break;
    case 7:
      theaterChaseRainbow(5); // Rainbow-enhanced theaterChase variant
      break;
    default:
      theaterChase(strip.Color(  0,   0, 127), 20); // Blue, half brightness;
      lightMode = 0;
      break;
  }
  // Fill along the length of the strip in various colors...

}

int KevGetSerialInt()
{
  char myinchar;
  int myinint;
  myinint = 0;

  myinchar = 0;
  while (Serial.available() < 1)
  {
    //Serial.print ("WFI-");
    //delay(200);
    CheckLights();
  }
  myinchar = Serial.read();
  while ( (myinchar != 10) and (myinchar != 13))
  {
    Serial.write(myinchar);
    myinint = (10 * myinint) + myinchar - 48;
    //Serial.print ("\nMyInChar Is '");
    //Serial.print (myinchar, DEC);
    //Serial.println ("'\n");
    while (Serial.available() < 1)
    {
      //Serial.print ("WFI-");
      //delay(200);
      CheckLights ();
    }
    myinchar = Serial.read();
  }

  Serial.println();
  Serial.flush();
  //Serial.print ("\nMyInIn Is '");
  //Serial.println(myinint, DEC);
  //Serial.println ("'\n");
  return myinint;
}

int KevGetSerialInt2()
{
  int myinint;
  //CycleLights();
  //myinint = -1;
  while (Serial.available() < 1)
  {
    Serial.print ("WFI-");
    //delay(500);
    CheckLights();
  }
  //while (myinint < 0)
  //{
  myinint = Serial.parseInt();
  //}
  Serial.flush();
  Serial.println(myinint, DEC);
  return myinint;
}


char KevGetSerialChar()
{
  char myinchar;

  while (Serial.available() < 1)
  {
    //delay(100);
    CheckLights();
  }
  myinchar = Serial.read();
  Serial.write(myinchar);
  Serial.flush();
  //Serial.println(myinchar);
  return myinchar;
}

void KevClearSerial()
{
  Serial.flush();
}

// Some functions of our own for creating animated effects -----------------

// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// strip.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
void colorWipe(uint32_t color, int wait) {
  for (int i = 0; i < strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}

// Theater-marquee-style chasing lights. Pass in a color (32-bit value,
// a la strip.Color(r,g,b) as mentioned above), and a delay time (in ms)
// between frames.
void theaterChase(uint32_t color, int wait) {
  for (int a = 0; a < 10; a++) { // Repeat 10 times...
    for (int b = 0; b < 3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in steps of 3...
      for (int c = b; c < strip.numPixels(); c += 3) {
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show(); // Update strip with new contents
      delay(wait);  // Pause for a moment
    }
  }
}

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(int wait) {
  // Hue of first pixel runs 5 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
  // means we'll make 5*65536/256 = 1280 passes through this loop:
  for (long firstPixelHue = 0; firstPixelHue < 5 * 65536; firstPixelHue += 256) {
    // strip.rainbow() can take a single argument (first pixel hue) or
    // optionally a few extras: number of rainbow repetitions (default 1),
    // saturation and value (brightness) (both 0-255, similar to the
    // ColorHSV() function, default 255), and a true/false flag for whether
    // to apply gamma correction to provide 'truer' colors (default true).
    strip.rainbow(firstPixelHue);
    // Above line is equivalent to:
    // strip.rainbow(firstPixelHue, 1, 255, 255, true);
    strip.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}

// Rainbow-enhanced theater marquee. Pass delay time (in ms) between frames.
void theaterChaseRainbow(int wait) {
  int firstPixelHue = 0;     // First pixel starts at red (hue 0)
  for (int a = 0; a < 30; a++) { // Repeat 30 times...
    for (int b = 0; b < 3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in increments of 3...
      for (int c = b; c < strip.numPixels(); c += 3) {
        // hue of pixel 'c' is offset by an amount to make one full
        // revolution of the color wheel (range 65536) along the length
        // of the strip (strip.numPixels() steps):
        int      hue   = firstPixelHue + c * 65536L / strip.numPixels();
        uint32_t color = strip.gamma32(strip.ColorHSV(hue)); // hue -> RGB
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show();                // Update strip with new contents
      delay(wait);                 // Pause for a moment
      firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
    }
  }
}

void setup() {
  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):

#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.
  lightMode = 0;
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(33); // Set BRIGHTNESS to about 1/5 (max = 255)
  // initialize serial communication at 9600 bits per second:

  Serial.begin(9600);

  unlocked = 0;
  unlockarraypos = 0;
  pinMode(RIGHT_SWITCH, INPUT_PULLUP);
  pinMode(LEFT_SWITCH, INPUT_PULLUP);


}

// the loop routine runs over and over again forever:
void loop() {
  KevClearSerial();

  Serial.println("Starting Wumpus Game\n");
  //delay(1);        // delay in between reads for stability
  // create wumpus game object
  WumpusGame game;
  // start the game
  game.StartGame();

}
