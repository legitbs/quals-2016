#!/usr/bin/env python

from generator.actions import Actions
import random
from struct import unpack


class SpanishTest(Actions):


    def start(self):

        self.flashcardMagicOffset = 0
        self.quizMagicOffset = 0

        self.wordlist = [
            [ "visitar", "to visit" ],
            [ "ver", "to see" ],
            [ "hablar", "to speak" ],
            [ "tener", "to have" ],
            [ "entender", "to understand" ],
            [ "ser", "to be" ],
            [ "estar", "to be" ],
            [ "ir" , "to go" ],
            [ "haber", "to have" ],
            [ "hacer", "to make" ],
            [ "querer", "to want" ],
            [ "deber", "to have to" ],
            [ "poder", "to be able to" ],
            [ "decir", "to say" ],
            [ "mirar", "to watch" ],
            [ "levantarse", "to get up" ],
            [ "probar", "to taste" ],
            [ "saber", "to know" ], 
            [ "salir", "to leave" ],
            [ "subir", "to go up or climb" ],
            [ "venir", "to come" ],
            [ "caerse", "to fall down" ],
            [ "conocer", "to know people and places" ],
            [ "ducharse", "to shower" ],
            [ "descansar", "to rest" ],
            [ "llover", "to rain" ],
            [ "caminar", "to walk" ],
            [ "cenar", "to have supper" ],
            [ "comer", "to eat" ],
            [ "desear", "to desire or want" ],
            [ "dormir", "to sleep" ],
            [ "entrar", "to enter" ]]
    
        # do this so its not a fixed number of entries in the database to start
        randValue = (unpack('H', self.magic_page[0:2])[0] % 5)

        self.wordlist = self.wordlist[ :-randValue or None]

        self.read(delim='\n', expect = "Bienvenidos to the Spanish Tutor!\n")
        self.read(delim='\n', expect='\n')

        self.read(length=7, expect = 'login: ')
        self.write('huesped\n')
        self.read(length=10, expect = 'password: ')
        self.write('madera\n')

        self.read(delim='\n', expect='\n')
        self.read(delim='\n', expect = 'access granted\n')


    def menu(self):

        self.read(delim='\n', expect='\n')
        self.read(delim='\n', expect='1) Do verb flashcards\n')
        self.read(delim='\n', expect='2) Word quizzes\n')
        self.read(delim='\n', expect='3) Perform word maintenance\n')
        self.read(delim='\n', expect='4) Quit\n')
        self.read(length=2, expect=': ')
   

    def doMaint(self):

        self.write('3\n')


    def maintMenu(self):

        self.read(delim='\n', expect='\n')
        self.read(delim='\n', expect="1) Add New Words\n")
        self.read(delim='\n', expect="2) Delete Words\n")
        self.read(delim='\n', expect="3) Edit Words\n")
        self.read(delim='\n', expect="4) List Words\n")
        self.read(delim='\n', expect="5) Exit Word Maintenance\n")

    def editWord(self):

        self.write('3\n')
        self.read(delim='\n', expect = "Oops, this isn't finished.  Guess you'd better delete and then re-add the word\n")

    def addWords(self):

        self.write('1\n')

        num2add = random.randint(0, 3)

        for i in range(0, num2add):

            word = 'blahblah[]'.format(i)
            meaning = 'to blahblah[]'.format(i)

            self.read(delim='\n', expect = 'Enter infinitive form\n')
            self.write(word + '\n')

            self.read(delim='\n', expect = 'Enter English meaning\n')
            self.write(meaning + '\n')

            self.read(delim='\n', expect = '\n')

            self.wordlist.append([word, meaning])

        self.read(delim='\n', expect = 'Enter infinitive form\n')
        self.write('\n')

    def listWords(self):

        self.write('4\n')

        for i in range(0, len(self.wordlist)):

            expectString = '{}-{}: {}\n'.format(i, self.wordlist[i][0], self.wordlist[i][1])

            self.read(delim='\n', expect=expectString)

    def deleteWords(self):

        self.write('2\n')

        self.read(delim='\n', expect = 'Enter word to delete\n')
        self.write('*\n')

        i = 0
        while i < len(self.wordlist):

            expectString = '{}: {}\n'.format(self.wordlist[i][0], self.wordlist[i][1])
            self.read(delim='\n', expect=expectString)   

            expectString = 'do you wish to delete this word? '
            self.read(length = len(expectString), expect = expectString)

            answer = random.choice(['y','n'])

            self.write(answer + '\n')

            if answer == 'y':
                del self.wordlist[i]
            else:
                i = i + 1


    def doFlashcards(self):

        self.write('1\n')

        if len(self.wordlist) == 0:
            return

        self.read(delim='\n', expect = '\n')

        self.read(delim='\n', expect = 'Hit <Enter> to continue, or q to exit\n')

        howmany = random.randint(1, 10)

        for i in range(0, howmany):

            choice = unpack('H', self.magic_page[self.flashcardMagicOffset:self.flashcardMagicOffset+2])[0]

            self.flashcardMagicOffset += 2

            if self.flashcardMagicOffset > 4094:
                self.flashcardMagicOffset = 0

            choice = choice % len(self.wordlist)

            self.read(delim='\n', expect = '\n')
            self.read(delim='\n', expect = '{}\n'.format(self.wordlist[choice][0]))

            if i==(howmany -1):
                self.write('q\n')
            else:
                self.write('\n')

            self.read(delim='\n', expect = '{}\n'.format(self.wordlist[choice][1]))
            self.read(delim='\n', expect = '\n')



    def doQuiz(self):

        self.write('2\n')

        if len(self.wordlist) == 0:
            return

        answerLetters = ['a', 'b', 'c', 'd']

        self.read(delim='\n', expect = "Answer with 'q' to exit the quiz\n")
        self.read(delim='\n', expect = '\n')

        howmany = random.randint(1, 10)

        for i in range(0, howmany):

            choice = unpack('H', self.magic_page[self.quizMagicOffset:self.quizMagicOffset+2])[0] % len(self.wordlist)

            self.quizMagicOffset += 2

            if self.quizMagicOffset > 4094:
                self.quizMagicOffset = 0

            rightAnswerNum = unpack('H', self.magic_page[self.quizMagicOffset:self.quizMagicOffset+2])[0] % 4

            self.quizMagicOffset += 2

            if self.quizMagicOffset > 4094:
                self.quizMagicOffset = 0

            self.read(delim='\n', expect = '\n')
            self.read(delim='\n', expect = '{}\n'.format(self.wordlist[choice][0]))

            for x in range(0, 4):

                if x==rightAnswerNum:

                    expectString = '  {} - {}\n'.format(answerLetters[x], self.wordlist[choice][1])

                else:

                    badChoice = unpack('H', self.magic_page[self.quizMagicOffset:self.quizMagicOffset+2])[0] % len(self.wordlist)
                    
                    self.quizMagicOffset += 2

                    if self.quizMagicOffset > 4094:
                        self.quizMagicOffset = 0

                    if badChoice == choice:

                        if badChoice < len(self.wordlist)-1:
                            badChoice += 1
                        else:
                            badChoice = 0

                    expectString = '  {} - {}\n'.format(answerLetters[x], self.wordlist[badChoice][1])

                self.read(delim='\n', expect = expectString)


            if i < howmany - 1:
                thisAnswer = random.choice(answerLetters)

                self.write(thisAnswer + '\n')

                if thisAnswer == answerLetters[rightAnswerNum]:

                    expectString = "Correct!\n"

                else:

                    expectString = "Wrong!\n"

                self.read(delim='\n', expect = '\n')
                self.read(delim='\n', expect = expectString)

            else:

                self.write('q\n')



    def backToMain(self):

        self.write('5\n')


    def exit(self):

        self.write('4\n')
        self.read(delim = '\n', expect = 'Adios!\n')



