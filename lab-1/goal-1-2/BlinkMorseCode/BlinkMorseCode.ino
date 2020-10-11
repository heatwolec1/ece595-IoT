/*
 * International Morse Code rules: (https://en.wikipedia.org/wiki/Morse_code#/media/File:International_Morse_Code.svg)
 *		1. The length of a dot is one unit.
 *		2. A dash is three units.
 *		3. The space between parts of the same letter is one unit.
 *		4. The space between letters is three units.
 *		5. The space between words is seven units.
*/

#define TIME_UNIT 200	// define the Morse code unit of time [ms]
#define LED_PIN   LED_BUILTIN

String message = "hello world";	// Insert the message to blink here
boolean validMessage = true;	// flag for checking if the message can be blinked

void dot(boolean finalPart=false) {
	// A dot is 1 unit long
	digitalWrite(LED_PIN, HIGH);
	delay(TIME_UNIT);
	digitalWrite(LED_PIN, LOW);

	// Pause for 1 unit between parts, 3 units between letters
	if (finalPart) delay(3*TIME_UNIT);
	else delay(TIME_UNIT);
}

void dash(boolean finalPart=false) {
	// A dash is 3 unit long
	digitalWrite(LED_PIN, HIGH);
	delay(3*TIME_UNIT);
	digitalWrite(LED_PIN, LOW);

	// Pause for 1 unit between parts, 3 units between letters
	if (finalPart) delay(3*TIME_UNIT);
	else delay(TIME_UNIT);
}

void blinkLetter(char letter) {
	switch(letter) {
		case 'a': dot();  dash(true); break;					// .-
		case 'b': dash(); dot();  dot();  dot(true);  break;	// -...
		case 'c': dash(); dot();  dash(); dot(true);  break;	// -.-.
		case 'd': dash(); dot();  dot(true);  break;			// -..
		case 'e': dot(true);  break;							// .
		case 'f': dot();  dot();  dash(); dot(true);  break;	// ..-.
		case 'g': dash(); dash(); dot(true);  break;			// --.
		case 'h': dot();  dot();  dot();  dot(true);  break;	// ....
		case 'i': dot();  dot(true);  break;					// ..
		case 'j': dot();  dash(); dash(); dash(true); break;	// .---
		case 'k': dash(); dot();  dash(true); break;			// -.-
		case 'l': dot();  dash(); dot();  dot(true);  break;	// .-..
		case 'm': dash(); dash(true); break;					// --
		case 'n': dash(); dot(true);  break;					// -.
		case 'o': dash(); dash(); dash(true); break;			// ---
		case 'p': dot();  dash(); dash(); dot(true);  break;	// .--.
		case 'q': dash(); dash(); dot();  dash(true); break;	// --.-
		case 'r': dot();  dash(); dot(true);  break;			// .-.
		case 's': dot();  dot();  dot(true);  break;			// ...
		case 't': dash(true); break;							// -
		case 'u': dot();  dot();  dash(true); break;			// ..-
		case 'v': dot();  dot();  dot();  dash(true); break;	// ...-
		case 'w': dot();  dash(); dash(true); break;			// .--
		case 'x': dash(); dot();  dot();  dash(true); break;	// -..-
		case 'y': dash(); dot();  dash(); dash(true); break;	// -.--
		case 'z': dash(); dash(); dot();  dot(true);  break;	// --..
		case ' ': delay(7*TIME_UNIT); break;	// Space means word is over, delay by 7 units
	}
}

void invalidMessageHandler(int blinkTime=50) {
	digitalWrite(LED_PIN, HIGH);
	delay(blinkTime);
	digitalWrite(LED_PIN, LOW);
	delay(blinkTime);
}

void setup() {
	// Use the built-in LED for simplicity
	pinMode(LED_PIN, OUTPUT);

	// Check the message for invalid characters (only lowercase letters are valid)
	message.toLowerCase();	// Morse code can't show case, so make everything lower
	for (unsigned int i = 0; i < message.length(); i++) {
		if (!isAlpha(message.charAt(i)) && message.charAt(i) != ' ') {
			validMessage = false;
			break;	// message is invalid, no point checking the rest of it
		}
	}
}

void loop() {
	// Blink out the message if valid, spaz out if not
	if (validMessage) {
		for (unsigned int i = 0; i < message.length(); i++) {
			blinkLetter(message.charAt(i));
		}
	} else {
		while (true) { // Want to spaz out until reset
			invalidMessageHandler();
		}
	}

	// End of message, insert the "between words" gap before looping
	delay(7*TIME_UNIT);
}
