//code for 2 line micro exchange
//only one call at a time - but sort of generalized.
//arduino c++ style. Not pyhton !
// vars and consts

int linepins[] = {3,4};
int ringpins[] = {5,6};
const int linecount = 2;
const int dialingdelay = 5000; // ms
const int debounce = 500; // ms
long unsigned int ringtimeout = 60000; // ms
const int dtpin = 11; // dialtone pin
//setup
void setup(){
pinMode(dtpin, OUTPUT);
for (int thisPin = 0; thisPin < linecount; thisPin++) {

    pinMode(linepins[thisPin], INPUT_PULLUP);
	  pinMode(ringpins[thisPin], OUTPUT);
    digitalWrite(ringpins[thisPin], LOW );
}
}
// Functions ?

void loop(){
// sits waiting for a line to go off hook

for (int line = 0; line < linecount; line++){ 
	bool state = ( digitalRead(linepins[line]) == LOW );
	if (state) { // a line has gone off hook !
    delay(debounce);
    if ( digitalRead(linepins[line]) == LOW ) {
      Dialtone(dialingdelay, line); 
      if ( digitalRead(linepins[line]) == LOW ) {
		// delay(dialingdelay);
		if (ring(!line) == 1 ) { waitforclear(line); }
		else {
			waitforcalltoend();
		}}
}}
}
}

//This is the standard BT ring
//Ring for 60 seconds, 400ms ring, 200ms pause, 400ms ring, 2000ms pause.

int ring(int ringline) {
bool bail = false ;
bool ringing = false;
bool f1;
bool f2;
long unsigned int t1 = millis();
long unsigned int t2 = micros();
long unsigned int t3 = t2 ;
long unsigned int t4 = t1 ;
while (!bail) {

long unsigned int mafs = millis() - t1 ;
long unsigned int mafs2 = micros() - t2 ;
long unsigned int mafs3 = micros() - t3 ;
long unsigned int mafs4 = millis() - t4 ;

if ((mafs < 400) || (mafs > 600 && mafs < 1000)) {
digitalWrite(ringpins[ringline], HIGH) ;
ringing = true;
}
else { 
digitalWrite(ringpins[ringline], LOW) ;
ringing = false;
}
if ( mafs > 3000 ) { t1 = millis(); }
if ( mafs4 > ringtimeout ) { 
  digitalWrite(ringpins[ringline], LOW );
  digitalWrite(dtpin, LOW);
  return 1; // timeout
bail = true ; // will never execute ? 
 }
if ( digitalRead(linepins[!ringline]) == HIGH ) { // clalling line hung up before timeout
  delay(debounce);
  if ( digitalRead(linepins[!ringline]) == HIGH ) {
    
    digitalWrite(ringpins[ringline], LOW );
    digitalWrite(dtpin, LOW);
    return 0;
  }
  }

if ( digitalRead(linepins[ringline]) == LOW ) { // call answerered
  delay(debounce);
  if ( digitalRead(linepins[ringline]) == LOW ) {
    digitalWrite(ringpins[ringline], LOW );
    digitalWrite(dtpin, LOW);
return 2 ; 
bail = true ;// will never execute ? 
}
}
if (ringing) { // code for audio ringing tone 
  // 400hz period 1/400 = 0.0025
  // 450hz period 1/450 = 0.002222222 etc..
  if ( mafs2 < 1250 ) {f1 = true; }
  else { f1 = false; }
  if ( mafs2 > 2500 ) { t2 = micros(); }
  if ( mafs3 < 1111 ) { f2 = true ; }
  else { f2 = false; }
  if ( mafs3 > 2222 ) { t3 = micros(); }
  if ( f1 | f2 ) { digitalWrite(dtpin, HIGH); } // & and | seem to work ok. Dunno why ! mafs.. innit!
  else { digitalWrite(dtpin, LOW); }
  }
}
}

void waitforclear(int rungline) {
  long unsigned int mictime400 = micros();
	while (true) {
    long unsigned int tot = micros() - mictime400 ; //400hz period = 1/400 = 0.0025
    if ( tot < 1250 ) { digitalWrite(dtpin, HIGH); }
    else { digitalWrite(dtpin, LOW); }
    if ( tot > 2500 ) { mictime400 = micros(); }
    
		if (digitalRead(linepins[rungline])==HIGH) { 
      digitalWrite(dtpin, LOW);
		  return; }
	}
}
	
void waitforcalltoend() {
	// count all lines off hook bail if == 0
	while (true) {
int	acc = 0;
		for (int line = 0; line < linecount; line++){ 
			acc = acc + (digitalRead(linepins[line])== HIGH) ; 
		}
		if ( acc == linecount ) { return; } // lines are high when on hook
	}
}

void Dialtone(int tonetime, int line) {
  long unsigned int miltime = millis();
  long unsigned int mictime450 = micros();
  long unsigned int mictime350 = mictime450;
  bool flag450;
  bool flag350;
  
  //450hz period = 1/450 = 0.002222222222222222 ish
  //350hz period = 1/350 = 0.00285714285 ish
  //for 50% duty on for half period off for half.
  
  while (true) {
    // did we hangup before we connected started ringing  ?

    
    if ( digitalRead(linepins[line]) == HIGH ) {
      delay(debounce);
      if ( digitalRead(linepins[line]) == HIGH ) {
        return ;
      }
    }
    long unsigned int dif450 = micros() - mictime450 ;
    long unsigned int dif350 = micros() - mictime350 ;
    if (dif450 < 1111) {
      flag450 = true;
    }
    else
    { flag450 = false; }
    if (dif450 > 2222) { mictime450 = micros(); }

    if (dif350 < 1428) {
      flag350 = true;
    }
    else {
      flag350 = false;
    }
    if (dif350 > 2857) { mictime350 = micros(); }
    if ((millis() - miltime) > tonetime ) { return; }

    if ( flag450 | flag350 ) {
      digitalWrite(dtpin, HIGH);
    }
    else
    {
      digitalWrite(dtpin, LOW);
    }
  }}
    
