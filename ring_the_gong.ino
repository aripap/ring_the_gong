//Includes
#include <Process.h>
#include <FileIO.h>
#include <Servo.h>
#include <SoftwareSerial.h>

Servo gongServo;

/* GMAIL SETTINGS */
const String username = "your_email@gmail.com"; //username@gmail.com
const String password = "your_password"; //password
const String label = "Won"; //gmail label, don't use spaces or special chars

//Set the count very high to start, will be reset as soon as we read from gmail
int unread_count = 1000;

void setup() {

  Serial.begin(9600);
  delay(1000);

  Bridge.begin();
  
  // setup File IO
  FileSystem.begin();

  // attach the servo on pin 9
  gongServo.attach(9);

  // return to zero position
  gongServo.write(0);  

  // give everything a chance to get settled
  delay(1000);

  Serial.println("Setup complete, reading email for " + username + " with label " + label);

}


void loop() {

  Serial.println("Starting loop...");

  // change this delay to check email more or less often
  delay(5000);

  // get the last count
  
  if(unread_count == 0) {
    Serial.println("Could not read old count"); //this is obviously not entirely correct 
    return;
  } else {
    Serial.println("Existing unread count is...");
    Serial.println(unread_count);
  }

  Process p;

  // checks for a specified label and returns the number of messages
  p.runShellCommandAsynchronously("curl -u " + username + ":" + password + " \"https://mail.google.com/mail/feed/atom/" + label + "\" -k --silent --max-time 10 | grep -o \"<fullcount>[0-9]*\" | grep -o \"[0-9]*$\"");

  while(p.running()){
    //Serial.println("Trying to read email..."); //uncomment to debug
    delay(5);
  }

  // The shell command above should have passed an integer representing the unread count
  int live_count = p.parseInt(); 

  Serial.println("New unread count is...");
  Serial.println(live_count);

  if (live_count == 0) {

    Serial.println("Problem accessing email"); //also not entirely correct since it could be zero

    // dump whatever we got from the command line
    while (p.available()>0) {
      char c = p.read();
      Serial.print(c);
    }
    p.flush();
    return;

  }

  p.flush();
  
  if(live_count > unread_count) {

    Serial.println("We have new emails!");

    ringTheGong();

    // save the latest result
    unread_count = (int)live_count;

    return;

  }

  if(live_count < unread_count) {

    unread_count = live_count;
    return;

  }
  
  Serial.println("No new emails!");     

  return;
}

  
// this function rings the gong
void ringTheGong() {

  Serial.println("Ringing the GONG!");

  int pos = 0;

  //Serial.println("Forward");

  for (pos = 0; pos <= 130; pos += 1) { // goes from 0 degrees to 130 degrees

    delay(3); //make it move smoothly

    gongServo.write(pos);              // tell servo to go to position in variable 'pos'

  }

  //Serial.println("Back");
  gongServo.write(0);

  return;
}
