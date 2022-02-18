# __TinyVoc - Yet another vocabulary trainer__
## Overview
TinyVoc is a small tool to train the vocabulary when learning a foreign language.  
It consists of an editor and the actual trainer.  
In the editor, words or phrases can be entered. Each line starts in the user's mother-language followed by an equal sign and then the same word or phrase in the language to learn.  

When learning German as a native English speaker, this could be something like:

    I am ... = Ich bin ...
    happy    = froh
    sad      = traurig
    tired    = müde

Note, that any space or tab outside of the actual phrases are explicitly allowed to ease readability in the editor, but will be trimmed in the trainer-module. Empty lines will be ignored as well.  
With _CTRL+ALT + T_ the trainer can be started with some options on the set of words or phrases (respectively the lines) to train.

The tool then randomly picks the amount of words or phrases from the selected set and asks for the entries in the home-language. The user then needs to enter the correct translation in the language to learn.  
When the reply is correct, the according pair will be removed from the list to answer. Otherwise the tool will show, what was expected - including some marking on where the mistake was - and save the pair for a later cycle.

The training ends, when each of the chosen phrases has been entered correctly.

## Diacritics, Umlauts and weird Latin letters
TinyVoc is Unicode-based, but designed for left-to-right writing. Thus, for any language, which is left-to right (amongst others, everything based on Latin or Cyrillic letters) should work fine. I myself trained Russian with it.

Now, when you happen to learn a language, which has a different character-set than your usual language (e.g. by learning English as Russian or vice versa), you will have to install an according keyboard-layout in Windows.

When you change the key-board-language for the training, the used keyset will be noted by TinyVoc and will be stored, so it is set right from the beginning next time.

There is a small gimmick to give some help when coming from one language based on Latin Characters to another. A support-function was implemented to get Latin letters with diacritics via signal keys. The used signal keys are: 

* Grave accent ( ` )
* Acute accent ( ´ )
* Circumflex ( ^ )
* Tilde ( ~ )
* Quotation marks used for diaeresis ( " )
* Single quote ( ' )
* Slash ( / )
* Over- respectively underscore ( _ )

Thus, when you want to type the name of the kid's area of a big, Swedish furniture warehouse, you could type: _S m ' a l a n d_ and get _Småland_ .

Some notes:  

* The French cedilla _ç_ is defined as grave accent ( ` ) and c.  
* The German sharp s _ß_ is defined as slash ( / ) and s.  
* The caron (like on the s in _Škoda_ ) is defined as single quote ( ' ) and the according letter.  
  As the quote is sometimes required for an actual dot, like in _å_ or _ė_ , this is not available for all letters.  
* The breve, like in _ă_ or _ĕ_ , is not available.  

As seen above, due to the limited amount of possible (and roughly sensible) key-combinations, not all Latin-based Unicode characters could be supported.  
The following characters are currently not available:

  Æ Þ ð þ Ă ă Ą ą ď Ĕ ĕ Ę ę Ě ě Ğ ğ Ĭ ĭ Į į İ ı Ĳ ĳ Ķ ķ ĸ Ļ ļ Ľ ľ Ņ ņ ŉ Ŋ ŋ Ň ň Ŏ ŏ Ő ő Œ œ Ŕ ŕ Ŗ ŗ Ţ ţ ť Ŭ ŭ Ű ű Ų ų

## Personal Note
This project is dedicated to my father. He is someone, for whom listening and understanding has a really high importance. One of his basic attitudes is, that when many say, that someone is bad, you have to know his language in order to find out yourself, whether this is actually true or not. This way, amongst many other languages on his way, he even learnt Russian in Western Germany the 1960ies - a time in which the cold war was at its peak.

And in the nineteen-eighties, when I started to learn English at high-school, he wrote a similar piece of software on the Commodore C64 and had me train 20 phrases each evening. And still today, I benefit from the vocabulary, which I gained by that.

Now, when you are happy with this little tool, I would be happy if you would send me a small tip ( [PayPal Donation](https://www.paypal.com/donate/?hosted_button_id=ZNGQUUBM5SDHE) ) or a small email with a a note, which language you are learning with it.

## Technical Details
When TinyVoc is closed, it stores the last training-options and the used language from the trainer's dialog in an initialization file. This file is placed in one of two locations:

* When the executable file is with in one of Windows' program-directories, TinyVoc considers itself to be a local installation. In this case, it places this file in _%APPDATA%\\TinyVoc_.  
  In windows 10, this expands to something like _C:\\Users\\\<user-name>\\AppData\\Roaming\\TinyVoc_.  
* Otherwise it considers itself to be a portable installation. The file is then stored in _.\\Data\\settings\\_.

## Developer Notes
This project is can be built with Mingw-w64 or Visual Studio. Some compiler switches were added, to ensure support for both environments.  
However, the batch-file, which ships with the source-code, relies on MinGw.  
Note, that the 32-bit version of MinGW caused some trouble, so I decided against using it.  
The help-html file is created at build-time from the read-me file using Pandoc.

## License
Copyright (C) 2022 Jens Daniel Schlachter (<osw.schlachter@mailbox.org>)  
  
This program is free software: you can redistribute it and/or modify  
it under the terms of the GNU General Public License as published by  
the Free Software Foundation, either version 3 of the License, or  
(at your option) any later version.  
  
This program is distributed in the hope that it will be useful,  
but WITHOUT ANY WARRANTY; without even the implied warranty of  
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the  
GNU General Public License for more details.  
  
You should have received a copy of the GNU General Public License  
along with this program.  If not, see <https://www.gnu.org/licenses/>.
